#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstring>
#include <fstream>
#include <cstdio>
#include <cstdlib>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "nlohmann/json.hpp"
#include "nlohmann/tinyxml2.h"
#include "./binasset_write.h"

static char*        string_readf(const char* path, int &size);
static std::string  get_dir_fpath(const std::string &filepath);

/* STRUCT FUNCTIONS */
namespace ggb
{
    static void assets_write_bin(std::vector<AssetDataIMG> &imgs,  std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *bin_path_out);
    static void assets_load_json(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *json_file);
    static void assets_load_xml(AssetDataAtlas &atlas, const char *xml_file);
    static void assets_free(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataAtlas> atlases, std::vector<AssetDataShader> &shaders);

/******************************************************** 
 *                                                      *
 * STRUCT FUNCTIONS                                     *
 *                                                      *
 ********************************************************/
    AssetDataIMG::AssetDataIMG(const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key)
    {
        data = stbi_load((file).c_str(), &x, &y, &channels, 0);
        if (data)
        {
            DEBUG_LOG("AssetDataIMG: read success %s.\n", file.c_str());
            return;
        }
        DEBUG_LOG("AssetDataIMG: failed to initialize %s.\n", file.c_str());
    }
    AssetDataIMG::AssetDataIMG(unsigned char *fdata, const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key), data(fdata)
    {
    }

    AssetDataShader::AssetDataShader(const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key)
    {
        data = string_readf((file).c_str(), count);
        if (data)
        {
            DEBUG_LOG("AssetDataShader: read success %s.\n", file.c_str());
            return;
        }
        DEBUG_LOG("AssetDataShader: failed to read %s.\n", file.c_str());
    }

    AssetDataShader::AssetDataShader(char *fdata, const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key), data(fdata)
    {
    }

    AssetDataAtlas::AssetDataAtlas(const std::string &fxml_path, const std::string &fhash_key)
        : xml_path(fxml_path), hash_key(fhash_key)
    {
        dir = get_dir_fpath(xml_path);
        assets_load_xml(*this, fxml_path.c_str());

        data = stbi_load((dir + "/" + file).c_str(), &x, &y, &channels, 0);
        if (data)
        {
            DEBUG_LOG("AssetDataAtlas: read success %s.\n", (dir + "/" + file).c_str());
            return;
        }
        DEBUG_LOG("AssetDataAtlas: loaded xml");
    }

    AssetDataAtlas::AssetDataAtlas(unsigned char *fdata, const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key), data(fdata)
    {}
    

/******************************************************** 
 *                                                      *
 * HELPER FUNCTIONS                                     *
 *                                                      *
 ********************************************************/

static void replace_char_and_upper(char *str, char find, char replace) {
        for (char *p = str; *p != '\0'; p++) {
            // Replace specific char
            if (*p == find) {
                *p = replace;
            }
            // Convert lowercase to uppercase
            else if (islower((unsigned char)*p)) {
                *p = toupper((unsigned char)*p);
            }
        }
    }

    /* PUBLIC FUNCTIONS */
    static void assets_load_json(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *json_file)
    {
        DEBUG_LOG("Loading json file %s...\n", json_file);
        std::ifstream file(json_file);
        if (!file.is_open())
        {
            DEBUG_LOG("Failed to open %s", json_file);
            return;
        }

        nlohmann::json j;
        file >> j;

        // image files
        for (const auto &entry: j["imgs"])
        {
            std::string path = entry["path"].get<std::string>();
            std::string key  = entry["key"].get<std::string>();
            
            imgs.emplace_back(path, key);
        }

        // vert/frag or any glsl
        for (const auto &entry: j["shaders"])
        {
            std::string path = entry["path"].get<std::string>();
            std::string key  = entry["key"].get<std::string>();
            
            shaders.emplace_back(path, key);
        }
        
        // xml
        for (const auto &entry: j["atlases"])
        {
            std::string path = entry["path"].get<std::string>();
            std::string key  = entry["key"].get<std::string>();
            
            atlases.emplace_back(path, key);
        }

    }

    static void assets_load_xml(AssetDataAtlas &atlas, const char *xml_file)
    {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(xml_file) != tinyxml2::XML_SUCCESS)
        {
            DEBUG_LOG("Failed to load XML\n");
            return;
        }

        auto root = doc.FirstChildElement("TextureAtlas");
        if (!root)
        {
            DEBUG_LOG("No <TextureAtlas> element found\n");
            return;
        }
        auto &sprite_names = atlas.sprite_names;
        auto &sprite_dims = atlas.sprite_dims;

        atlas.file = (root->Attribute("imagePath"));
        
        for (auto elem = root->FirstChildElement("SubTexture"); elem; elem = elem->NextSiblingElement("SubTexture"))
        {
            int x, y, w, h;

            std::string name = elem->Attribute("name");
            replace_char_and_upper(name.data(), '.', '_');
            sprite_names.push_back(name);
            
            elem->QueryAttribute("x", &x);
            elem->QueryAttribute("y", &y);
            elem->QueryAttribute("width", &w);
            elem->QueryAttribute("height", &h);

            sprite_dims.push_back({x, y, w, h});
        }
    }

    static void assets_free(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataAtlas> atlases, std::vector<AssetDataShader> &shaders)
    {
        for (auto &img : imgs)
        {
            std::free(img.data);
            img.data = NULL;
        }
        for (auto &atlas : atlases)
        {
            std::free(atlas.data);
            atlas.data = NULL;
        }
        for (auto &shader : shaders)
        {
            std::free(shader.data);
            shader.data = NULL;
        }
        DEBUG_LOG("Assets resources free.\n");
    }

    
    void assets_create_bin(const char *json_file, const char *bin_path_out)
    {
        if (!json_file || !bin_path_out) return;
        std::vector<ggb::AssetDataIMG>    imgs;
        std::vector<ggb::AssetDataShader> shaders;
        std::vector<ggb::AssetDataAtlas>  atlases;
        
        DEBUG_LOG("Creating asset binaries...\n");
        assets_load_json(imgs, shaders, atlases, json_file);
        assets_write_bin(imgs, shaders, atlases, bin_path_out);
        assets_free(imgs, atlases, shaders);
    }





    

/******************************************************** 
 *                                                      *
 * WRITING FUNCTIONS                                    *
 *                                                      *
 ********************************************************/

    /* WRITING */
    void assets_write_bin(std::vector<AssetDataIMG> &imgs,  std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *bin_path_out)
    {
        char header[] = "glnsh";
        std::string bin_path = bin_path_out; 
        std::string path_out = bin_path + ".glnsh";
        std::string path_h_out = bin_path + ".h";

        DEBUG_LOG("Writing asset binaries %s...\n", path_out.c_str());
        std::FILE *file = std::fopen(path_out.c_str(), "wb");

        if (!file) 
        {
            DEBUG_LOG("Failed to compile %s", path_out.c_str());
            return;
        }

        /* header */
        std::fwrite(header, sizeof(char), 5, file);
        
        /* IMG, ATLAS, SHADER, SUBTEX COUNTS */
        AssetCounts dcounts = {};
        dcounts.imgs = imgs.size();
        dcounts.atlases = atlases.size();
        dcounts.shaders = shaders.size();
        dcounts.subtex = 0;
        
        std::vector<int> buffer;
        std::vector<SubTextureDims> sub_tex;

        size_t total_hash_chars = 0;
        size_t img_hash_chars = 0;
        size_t atlas_hash_chars = 0;
        size_t shader_hash_chars = 0;
        size_t total_subtex_chars = 0;
        
        /* PER IMG, ATLAS, SHADER INFOS */

        for (auto &img : imgs) 
        {
            buffer.push_back(img.channels);
            buffer.push_back(img.x);
            buffer.push_back(img.y);

            img_hash_chars += img.hash_key.size() + 1;
        }
        
        for (auto &atlas : atlases)
        {
            int sub_n = (int)atlas.sprite_names.size();
            dcounts.subtex += sub_n;
            
            buffer.push_back(atlas.channels);
            buffer.push_back(atlas.x);
            buffer.push_back(atlas.y);
            buffer.push_back(sub_n);

            atlas_hash_chars += atlas.hash_key.size() + 1;
            
            for (int i =0; i < (int)atlas.sprite_dims.size(); i++)
            {
                sub_tex.push_back(atlas.sprite_dims[i]);
                total_subtex_chars += atlas.sprite_names[i].size() + 1;
            }
        }
        
        for (auto &shader : shaders) 
        {
            shader_hash_chars += shader.hash_key.size() + 1;

            buffer.push_back(shader.count);
        }
        total_hash_chars = img_hash_chars + atlas_hash_chars + shader_hash_chars;

        /* DATA COUNTS */
        std::fwrite(&dcounts, sizeof(int), 4, file);

        /* INFOS */
        std::fwrite(buffer.data(), sizeof(int), buffer.size(), file);
        buffer = {};

        /* SUBTEX */
        std::fwrite(sub_tex.data(), sizeof(SubTextureDims), sub_tex.size(), file);
        
        // REMOVE

        /* PER IMG, ATLAS, SHADER DATA */
        for (auto &img : imgs) 
        {
            std::fwrite(img.data, sizeof(unsigned char), img.channels * img.x * img.y, file);
        }

        for (auto &atlas : atlases) 
        {
            std::fwrite(atlas.data, sizeof(unsigned char), atlas.channels * atlas.x * atlas.y, file);
        }
       
        for (auto &shader : shaders) 
        {
            std::fwrite(shader.data, sizeof(char), shader.count, file);
        }

        DEBUG_LOG("Successfully compiled asset binaries %s\n", path_out.c_str());

        std::fclose(file);

        /* WRITE .h FILE */
        DEBUG_LOG("Creating enums .h file %s\n", path_h_out.c_str())
        file = std::fopen(path_h_out.c_str(), "wb");

        if (!file)
        {
            DEBUG_LOG("Failed to create file %s\n", path_h_out.c_str());
            return;
        }

        char *img_hash_keys = nullptr;
        char *atlas_hash_keys = nullptr;
        char *shader_hash_keys = nullptr;
        (void) img_hash_keys;
        (void) atlas_hash_keys;
        (void) shader_hash_keys;

        std::string img_header = "#pragma once\n\nnamespace IMG {\nenum {";
        std::string atlas_header = "namespace ATLAS {\nenum {";
        std::string shader_header = "namespace SHADER {\nenum {";
        std::string footer = "};\n}\n";

        std::vector<size_t> atlas_subtex_keys(dcounts.atlases);
        std::vector<char> hash_keys(total_hash_chars + img_header.size() + atlas_header.size() + shader_header.size() + 64);
        std::vector<char> subtex_keys(total_subtex_chars + atlas_hash_chars + shader_header.size() * dcounts.atlases + footer.size() * dcounts.atlases + 64);

        size_t hash_offset = 0;
        size_t subtex_offset = 0;
        
        auto copy_str = [&](std::vector<char>& buffer, size_t& offset, const std::string& s) {
            std::memcpy(buffer.data() + offset, s.data(), s.size());
            offset += s.size();
            buffer.data()[offset++] = '\n';
        };
        auto copy_str_comma = [&](std::vector<char>& buffer, size_t& offset, const std::string& s) {
            std::memcpy(buffer.data() + offset, s.data(), s.size());
            offset += s.size();
            buffer.data()[offset++] = ',';
            buffer.data()[offset++] = '\n';
        };
        // TODO finish header output

        /* IMG ENUM CLASS */
        img_hash_keys = hash_keys.data();
        copy_str(hash_keys, hash_offset, img_header); //header
        for (auto &img : imgs) 
        {
            copy_str_comma(hash_keys, hash_offset, img.hash_key);
        }
        copy_str(hash_keys, hash_offset, footer);
        
        /* SHADER ENUM CLASS */
        shader_hash_keys = hash_keys.data() + hash_offset; // header
        copy_str(hash_keys, hash_offset, shader_header);
        for (auto &shader : shaders) 
        {
            copy_str_comma(hash_keys, hash_offset, shader.hash_key);
        }
        copy_str(hash_keys, hash_offset, footer);
        
        /* ATLAS ENUM CLASS */
        atlas_hash_keys = hash_keys.data() + hash_offset;
        copy_str(hash_keys, hash_offset, atlas_header); //header
        for (auto &atlas : atlases)
        {
            copy_str_comma(hash_keys, hash_offset, atlas.hash_key);
            
            /* SUBTEX ENUM CLASS */
            atlas_subtex_keys.push_back(subtex_offset);
            std::string subtex_header = "namespace " + atlas.hash_key + " {\nenum {";
            copy_str(subtex_keys, subtex_offset, subtex_header); // header
            for (auto &s : atlas.sprite_names)
            {
                copy_str_comma(subtex_keys, subtex_offset, s);
            }
            copy_str(subtex_keys, subtex_offset, footer); // header
        }
        copy_str(hash_keys, hash_offset, footer);

        std::fwrite(hash_keys.data(), sizeof(char), hash_offset, file);
        std::fwrite(subtex_keys.data(), sizeof(char), subtex_offset, file);

        std::fclose(file);
    }
}
    






/******************************************************** 
 *                                                      *
 * HELPER STATIC FUNCTIONS                              *
 *                                                      *
 ********************************************************/
static char* string_readf(const char* path, int &size)
{
    std::FILE *file = std::fopen(path, "r");
    if (!file)
    {
        return NULL;
    }

    std::fseek(file, 0, SEEK_END);
    size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    
    char *str = (char *)std::malloc(size + 1);
    str[std::fread(str, sizeof(char), size, file)] = '\0';

    return str;
}

static std::string get_dir_fpath(const std::string &filepath)
{
    size_t pos = filepath.find_last_of("/\\");
    if (pos == std::string::npos)
        return ""; // no directory part
    return filepath.substr(0, pos);
}

