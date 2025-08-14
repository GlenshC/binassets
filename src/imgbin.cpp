#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "nlohmann/json.hpp"
#include "nlohmann/tinyxml2.h"
#include <cstdio>
#include <iostream>
#include <fstream>

#include <cstdlib>
#include <vector>
#include "./imgbin.h"

namespace ggb
{
    void replace_char_and_upper(char *str, char find, char replace) {
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
    void assets_load_json(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *json_file)
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

    void assets_load_xml(AssetDataAtlas &atlas, const char *xml_file)
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

    void assets_vector_free(AssetDataIMG &img)
    {
        std::free(img.data);
        img.data = NULL;         
    }
    
    void assets_vector_free(AssetDataShader &shader)
    {
        std::free(shader.data);
        shader.data = NULL;
    }

    void assets_free(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders)
    {
        for (auto &img : imgs)
        {
            ggb::assets_vector_free(img);
        }
        for (auto &shader : shaders)
        {
            ggb::assets_vector_free(shader);
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
        assets_free(imgs, shaders);
    }

    void assets_load_bin(AssetData &data_out, const char *bin_path)
    {        
        if (!bin_path) return;

        DEBUG_LOG("Loading asset binaries...\n");
        assets_read_bin(data_out, bin_path);
    }


    void assets_data_free(AssetData &data)
    {
        std::free(data.atlases);
        data.atlases = nullptr;
        data.imgs = nullptr;
        data.shaders = nullptr;
        data.atlases_size = 0;
        data.imgs_size = 0;
        data.shaders_size = 0;

        DEBUG_LOG("Binary Assets Freed.\n");
    }
}