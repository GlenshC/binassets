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
#include "binassets/binasset_write.h"

static char*        string_readf(const char* path, int &size);
static std::string  get_dir_fpath(const std::string &filepath);

/* STRUCT FUNCTIONS */
namespace binassets
{
    void assets_write_json(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases,std::string path_json_out);
    static void assets_write_bin(std::vector<AssetDataIMG> &imgs,  std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *bin_path_out);
    static void assets_load_json(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *json_file);
    static void assets_load_xml(AssetDataAtlas &atlas, const char *xml_file);

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
            float x, y, w, h;

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

    
    void assets_create_bin(const char *json_file, const char *bin_path_out)
    {
        if (!json_file || !bin_path_out) return;
        std::vector<binassets::AssetDataIMG>    imgs;
        std::vector<binassets::AssetDataShader> shaders;
        std::vector<binassets::AssetDataAtlas>  atlases;
        
        DEBUG_LOG("Creating asset binaries...\n");
        assets_load_json(imgs, shaders, atlases, json_file);
        assets_write_bin(imgs, shaders, atlases, bin_path_out);
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
        std::string path_json_out = bin_path + ".json";

        AssetCounts dcounts = {};
        dcounts.imgs = imgs.size();
        dcounts.atlases = atlases.size();
        dcounts.shaders = shaders.size();
        dcounts.subtex = 0;
        {
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

            /* PER IMG, ATLAS, SHADER INFOS */

            std::vector<SubTextureDims> sub_tex;
            std::vector<int> buffer;
            for (auto &img : imgs)
            {
                buffer.push_back(img.channels);
                buffer.push_back(img.x);
                buffer.push_back(img.y);
            }

            for (auto &atlas : atlases)
            {
                int sub_n = (int)atlas.sprite_names.size();
                dcounts.subtex += sub_n;

                buffer.push_back(atlas.channels);
                buffer.push_back(atlas.x);
                buffer.push_back(atlas.y);
                buffer.push_back(sub_n);
                DEBUG_LOG("Atlas: %d %d %d %d\n", atlas.channels, atlas.x, atlas.y, sub_n);

                for (int i = 0; i < (int)atlas.sprite_dims.size(); i++)
                {
                    auto &sprite_dim = atlas.sprite_dims[i];
                    sub_tex.push_back({sprite_dim.x,
                                       sprite_dim.y,
                                       sprite_dim.width,
                                       sprite_dim.height});
                }
            }

            for (auto &shader : shaders)
            {

                buffer.push_back(shader.count);
                DEBUG_LOG("shader[i].count = %d;\n", shader.count);
            }

            /* DATA COUNTS */
            std::fwrite(&dcounts, sizeof(int), 4, file);

            /* INFOS */
            std::fwrite(buffer.data(), sizeof(int), buffer.size(), file);

            std::fwrite(sub_tex.data(), sizeof(SubTextureDims), sub_tex.size(), file);

            /* SUBTEX */
            
            // REMOVE
    
            /* PER IMG, ATLAS, SHADER DATA */
            for (auto &img : imgs) 
            {
                std::fwrite(img.data, sizeof(unsigned char), img.channels * img.x * img.y, file);
                free(img.data);
            }
    
            for (auto &atlas : atlases) 
            {
                std::fwrite(atlas.data, sizeof(unsigned char), atlas.channels * atlas.x * atlas.y, file);
                free(atlas.data);
            }
           
            for (auto &shader : shaders) 
            {
                std::fwrite(shader.data, sizeof(char), shader.count, file);
                free(shader.data);
            }
    
            DEBUG_LOG("Successfully compiled asset binaries %s\n", path_out.c_str());
    
            std::fclose(file);
        }

        /* WRITE .h FILE */
        {
            DEBUG_LOG("Creating enums .h file %s\n", path_h_out.c_str())
            FILE *file = std::fopen(path_h_out.c_str(), "wb");

            if (!file)
            {
                DEBUG_LOG("Failed to create file %s\n", path_h_out.c_str());
                return;
            }

            std::string h_header = "#pragma once\n";
            std::string img_header = "namespace BSST_IMG {\n    enum {";
            std::string atlas_header = "namespace BSST_ATLAS {\n    enum {";
            std::string shader_header = "namespace BSST_SHADER {\n    enum {";
            std::string bracket = "};\n";
            std::string names_var = "char *NAMES[] = {";

            const size_t buffer_cap = 4092;
            size_t buffer_size = 0;
            char *out_buffer = (char *)std::malloc(buffer_cap + 1);
            out_buffer[buffer_size] = 0;
            std::string indent1 = "    ";
            std::string indent2 = "        ";

            auto writeout_raw = [&](const std::string &s)
            {
                if (buffer_size + s.size() >= buffer_cap)
                {
                    std::fwrite(out_buffer, 1, buffer_size, file);
                    buffer_size = 0;
                }
                std::memcpy(out_buffer + buffer_size, s.data(), s.size());
                buffer_size += s.size();
            };

            auto writeout = [&](const std::string &s)
            {
                if (buffer_size + s.size() + 1 >= buffer_cap)
                {
                    std::fwrite(out_buffer, 1, buffer_size, file);
                    buffer_size = 0;
                }
                std::memcpy(out_buffer + buffer_size, s.data(), s.size());
                buffer_size += s.size();
                out_buffer[buffer_size++] = '\n';
            };
            auto writeoutln = [&](const std::string &s)
            {
                if (buffer_size + s.size() + 2 >= buffer_cap)
                {
                    std::fwrite(out_buffer, 1, buffer_size, file);
                    buffer_size = 0;
                }
                std::memcpy(out_buffer + buffer_size, s.data(), s.size());
                buffer_size += s.size();
                out_buffer[buffer_size++] = ',';
                out_buffer[buffer_size++] = '\n';
            };

            auto writeout_str = [&](const std::string &s)
            {
                if (buffer_size + s.size() + 4 >= buffer_cap)
                {
                    std::fwrite(out_buffer, 1, buffer_size, file);
                    buffer_size = 0;
                }
                out_buffer[buffer_size++] = '"';
                std::memcpy(out_buffer + buffer_size, s.data(), s.size());
                buffer_size += s.size();
                out_buffer[buffer_size++] = '"';
                out_buffer[buffer_size++] = ',';
                out_buffer[buffer_size++] = '\n';
            };
            // TODO finish header output

            /* IMG ENUM CLASS */
            writeout(h_header);
            if (dcounts.imgs)
            {
                writeout(img_header); // header
                for (auto &img : imgs)
                {
                    writeout_raw(indent2);
                    writeoutln(img.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout_raw(indent1);
                writeout(names_var);
                for (auto &img : imgs)
                {
                    writeout_raw(indent2);
                    writeout_str(img.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout(bracket);
            }

            /* SHADER ENUM CLASS */
            if (dcounts.shaders)
            {
                writeout(shader_header);
                for (auto &shader : shaders)
                {
                    writeout_raw(indent2);
                    writeoutln(shader.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout_raw(indent1);
                writeout(names_var);
                for (auto &shader : shaders)
                {
                    writeout_raw(indent2);
                    writeout_str(shader.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout(bracket);
            }

            /* ATLAS ENUM CLASS */
            if (dcounts.atlases)
            {
                writeout(atlas_header); // header
                for (auto &atlas : atlases)
                {
                    writeout_raw(indent2);
                    writeoutln(atlas.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout_raw(indent1);
                writeout(names_var);
                for (auto &atlas : atlases)
                {
                    writeout_raw(indent2);
                    writeout_str(atlas.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout(bracket);
            }

            /* WRITE SUB TEX */
            if (dcounts.atlases)
            {
                for (auto &atlas : atlases)
                {
                    /* SUBTEX ENUM CLASS */
                    std::string subtex_header = "namespace BSST_" + atlas.hash_key + " {\n    enum {";
                    writeout(subtex_header); // header
                    for (auto &s : atlas.sprite_names)
                    {
                        writeout_raw(indent2);
                        writeoutln(s);
                    }
                    writeout_raw(indent1);
                    writeout(bracket);
                    writeout_raw(indent1);
                    writeout(names_var);
                    for (auto &s : atlas.sprite_names)
                    {
                        writeout_raw(indent2);
                        writeout_str(s);
                    }

                    writeout_raw(indent1);
                    writeout(bracket);
                    writeout(bracket);
                }
            }

            std::fwrite(out_buffer, sizeof(char), buffer_size, file); // leftovers
        
            std::fclose(file);
        }
        assets_write_json(imgs, shaders, atlases, path_json_out);
    }
    
    void assets_write_json(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases,std::string path_json_out)
    {
        (void) imgs, (void) shaders, (void) atlases;
        const size_t buffer_cap = 4092;
        size_t buffer_size = 0;
        char *out_buffer = (char *)std::malloc(buffer_cap + 1);
        out_buffer[buffer_size] = 0;
        std::string indent1 = "  ";

        std::FILE *file = std::fopen(path_json_out.c_str(), "wb");
        auto writeout_raw = [&](const std::string &s)
        {
            if (buffer_size + s.size() >= buffer_cap)
            {
                std::fwrite(out_buffer, 1, buffer_size, file);
                buffer_size = 0;
            }
            std::memcpy(out_buffer + buffer_size, s.data(), s.size());
            buffer_size += s.size();
        };

        auto writeout_str = [&](const std::string &s)
        {
            if (buffer_size + s.size() + 4 >= buffer_cap)
            {
                std::fwrite(out_buffer, 1, buffer_size, file);
                buffer_size = 0;
            }
            out_buffer[buffer_size++] = '"';
            std::memcpy(out_buffer + buffer_size, s.data(), s.size());
            buffer_size += s.size();
            out_buffer[buffer_size++] = '"';
        };

        writeout_raw("{\n  \"imgs\": [\n");
        for (size_t i = 0; i < imgs.size(); i++)
        {
            writeout_raw("    ");
            writeout_str(imgs[i].hash_key);
            if (i < imgs.size() - 1) writeout_raw(",\n");
            else writeout_raw("\n");
        }

        writeout_raw("  ],\n  \"atlases\": [\n");
        for (size_t i = 0; i < atlases.size(); i++)
        {
            writeout_raw("    {\n      \"name\":\""+atlases[i].hash_key+"\",\n      \"sub_tex\": [\n");
            auto &subtex = atlases[i].sprite_names;
            for (size_t j = 0; j < atlases[i].sprite_names.size(); j++)
            {
                writeout_raw("        ");
                writeout_str(subtex[j]);
                if (j < subtex.size() - 1) writeout_raw(",\n");
                else writeout_raw("\n");
            }
            writeout_raw("      ]\n    }");

            if (i < atlases.size() - 1) writeout_raw(",\n");
            else writeout_raw("\n");
        }
        writeout_raw("  ],\n  \"shaders\": [\n");
        for (size_t i = 0; i < shaders.size(); i++)
        {
            writeout_raw("    ");
            writeout_str(shaders[i].hash_key);
            if (i < shaders.size() - 1) writeout_raw(",\n");
            else writeout_raw("\n");
        }
        writeout_raw("  ]\n}\n");
        std::fwrite(out_buffer, sizeof(char), buffer_size, file); // leftovers
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

