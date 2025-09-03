#include "nlohmann/json.hpp"
#include "nlohmann/tinyxml2.h"
#include "binassets/binasset_stl_read.h"
#include "binassets/binasset_write.h"

#ifdef ADOBO_GAME_ENGINE
#   include "util/debug.h"
#   include "renderer/texture.h"
#   include "core/constants.h"
#else
#   define STB_IMAGE_IMPLEMENTATION
#   include "util_debug.h"
#endif

#include "stb_image/stb_image.h"

#include <fstream>
#include <string>
#include <vector>

static char*        string_readf(const char* path, int &size);
static std::string  get_dir_fpath(const std::string &filepath);
static void replace_char_and_upper(char *str, char find, char replace);
namespace binassets
{

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

        data = stbi_load((dir + "\\" + file).c_str(), &x, &y, &channels, 0);
        if (data)
        {
            DEBUG_LOG("AssetDataAtlas: read success %s.\n", (dir + "\\" + file).c_str());
            return;
        }
        data = nullptr;
        DEBUG_ERR("AssetDataAtlas: failed to load xml %s.\n", fxml_path.c_str());
    }

    AssetDataAtlas::AssetDataAtlas(unsigned char *fdata, const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key), data(fdata)
        {}









   /**************************************************
    *                                                * 
    *                                                * 
    * FUNCTIONS                                      * 
    *                                                * 
    *                                                * 
    **************************************************/ 
    void assets_free(AssetData &assets)
    {
        for (auto &e : assets.atlases)
        {
            #ifdef ADOBO_GAME_ENGINE
            texture::remove_tex2D(e.tex);
            #endif 
            
            if (e.data) {
                std::free(e.data);
                e.data = nullptr;
            }
        }
        for (auto &e : assets.shaders)
        {
            #ifdef ADOBO_GAME_ENGINE
            // shader::remove_shader(e.shader)
            #endif
            if (e.data) {
                std::free(e.data);
                e.data = nullptr;
            }
        }
        assets.atlases.clear();
        assets.shaders.clear();
        assets.loaded.clear();
    }

    void assets_load_xml(AssetDataAtlas &atlas, const char *xml_file)
    {
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(xml_file) != tinyxml2::XML_SUCCESS)
        {
            DEBUG_ERR("Failed to load XML: %s.\n", xml_file);
            return;
        }

        auto root = doc.FirstChildElement("TextureAtlas");
        if (!root)
        {
            DEBUG_LOG("No <TextureAtlas> element found\n");
            return;
        }
        auto &subtex = atlas.subtex;
        auto &subtex_names = atlas.subtex_names;

        atlas.file = (root->Attribute("imagePath"));
        
        for (auto elem = root->FirstChildElement("SubTexture"); elem; elem = elem->NextSiblingElement("SubTexture"))
        {
            float x, y, w, h;

            std::string name = elem->Attribute("name");
            
            replace_char_and_upper(name.data(), '.', '_');
            
            elem->QueryAttribute("x", &x);
            elem->QueryAttribute("y", &y);
            elem->QueryAttribute("width", &w);
            elem->QueryAttribute("height", &h);
            
            subtex.push_back({x, y, w, h});
            subtex_names.emplace_back(name);
        }
    }

    int assets_load_atlas(AssetData &assets, const char *path, const char* name)
    {
        if (assets.loaded[path]) {DEBUG_ERR("BSST_ASSET_LOAD_ATLAS: ALREADY LOADED THAT\n"); return -1;}
        if (!path || !name){DEBUG_ERR("BSST_ASSET_LOAD_ATLAS: INVALID PATH OR NAME\n"); return -1;}
        auto &atlas = assets.atlases.emplace_back(path, name);  
        assets.loaded[path] = true;

        #ifdef ADOBO_GAME_ENGINE
        texture::loadAtlas2D(atlas, 0x1908);
        #endif
        
        return 0;
    }
    
    // depreciated
    int assets_load_img(AssetData &assets, const char *path, const char* name)
    {
        if (assets.loaded[path]) {DEBUG_ERR("BSST_ASSET_LOAD_ATLAS: ALREADY LOADED THAT\n"); return -1;}
        if (!path || !name){DEBUG_ERR("BSST_ASSET_LOAD_ATLAS: INVALID PATH OR NAME\n"); return -1;}
        // assets.imgs.emplace_back(path, name);
        assets.loaded[path] = true;
        return 0;
    }

    int assets_load_shader(AssetData &assets, const char *path, const char* name)
    {
        if (assets.loaded[path]) {DEBUG_ERR("BSST_ASSET_LOAD_SHADER: ALREADY LOADED THAT\n"); return -1;}
        if (!path || !name){DEBUG_ERR("BSST_ASSET_LOAD_ATLAS: INVALID PATH OR NAME\n"); return -1;}
        assets.shaders.emplace_back(path, name);
        assets.loaded[path] = true;
        return 0;
    }
    
    void assets_load_assetlist_json(AssetData &assets, const char *json_file)
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

        auto &atlases = assets.atlases;
        auto &shaders = assets.shaders;

        // image files
        // for (const auto &entry: j["imgs"])
        // {
        //     std::string path = entry["path"].get<std::string>();
        //     std::string key  = entry["key"].get<std::string>();
            
        //     imgs.emplace_back(path, key);
        // }

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

}

static void replace_char_and_upper(char *str, char find, char replace)
{
    for (char *p = str; *p != '\0'; p++)
    {
        // Replace specific char
        if (*p == find)
        {
            *p = replace;
        }
        // Convert lowercase to uppercase
        else if (islower((unsigned char)*p))
        {
            *p = toupper((unsigned char)*p);
        }
    }
}

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