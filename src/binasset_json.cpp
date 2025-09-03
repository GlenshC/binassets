
#include <fstream>
#include <string>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "nlohmann/json.hpp"

#ifdef ADOBO_GAME_ENGINE
#include "util/debug.h"
#else
#include "util_debug.h"
#endif

#include "binassets/binasset_json.h"

namespace binassets
{
    void assets_test(AssetNames &names)
    {
        if (!names._bp)
        {
            DEBUG_ERR("ERROR: AssetNames doesn't exist.\n");
            return;
        }
        DEBUG_LOG("IMGS:\n");
        for (int i =0; i < names.size_imgs; i++)
        {
            DEBUG_LOG("  Name: %s\n", names.imgs[i].name);
        }

        DEBUG_LOG("Atlases:\n");
        for (int i =0; i < names.size_atlases; i++)
        {
            DEBUG_LOG("  Name: %s\n    Subtex:\n", names.atlases[i].name);
            for (int j = 0; j < names.atlases[i].subtex_n; j++)
            {
                DEBUG_LOG("      %s\n", names.atlases[i].subtex[j].name);
            }
        }

        DEBUG_LOG("Shaders:\n");
        for (int i =0; i < names.size_shaders; i++)
        {
            DEBUG_LOG("  Name: %s\n", names.shaders[i].name);
        }
    }

    void assets_free(AssetNames &names)
    {
        if (names._bp)
        {
            DEBUG_LOG("Freed AssetNames\n");
            std::free(names._bp);
            names._bp     = nullptr;
            names.imgs    = nullptr;
            names.atlases = nullptr;
            names.shaders = nullptr;
            return;
        }
        DEBUG_ERR("Error freeing AssetNames: %p\n", names._bp);
    }

    void assets_load_json(AssetNames &names, const char *json_file)
    {
        DEBUG_LOG("Loading json file %s...\n", json_file);
        std::ifstream file(json_file);
        if (!file.is_open())
        {
            DEBUG_LOG("Failed to open %s", json_file);
            return;
        }

        nlohmann::json fjson;
        file >> fjson;

        size_t size_imgs = fjson["imgs"].size();
        size_t size_atlases = fjson["atlases"].size(); 
        size_t size_shaders = fjson["shaders"].size(); 
        size_t size_subtex = 0;
        for (const auto &entry: fjson["atlases"])
        {
            size_subtex += entry["sub_tex"].size(); 
        }
        size_t total_bytes = (size_imgs + size_shaders + size_subtex) * sizeof(Asset_Name) + size_atlases * sizeof(AssetNameAtlas);
        
        size_t basemem_offset = 0;
        char *base_mem = (char *)malloc(total_bytes);
        if (!base_mem)
        {
            DEBUG_ERR("Malloc error: returned %p\n", base_mem);
            return;
        }
        
        if (total_bytes)
        {
            names._bp = base_mem;
        }
        if (size_atlases)
        {
            names.atlases = (AssetNameAtlas *)(base_mem + basemem_offset);
            basemem_offset += sizeof(AssetNameAtlas) * size_atlases;
            names.size_atlases = size_atlases;
            names.size_subtex  = size_subtex;
        }
        if (size_imgs)
        {
            names.imgs    = (Asset_Name *) (base_mem + basemem_offset);
            basemem_offset += sizeof(Asset_Name) * size_imgs;
            names.size_imgs    = size_imgs;
        }
        if (size_shaders)
        {
            names.shaders = (Asset_Name *) (base_mem + basemem_offset);
            basemem_offset += sizeof(Asset_Name) * size_shaders;
            names.size_shaders = size_shaders;
        }
        
        auto copystr = [&](char* dest, std::string &src)
        {
            size_t size = std::min(BSST_ASSET_MAX_NAME - 1, src.size());
            std::memcpy(dest, src.data(), size);
            dest[size] = 0;
        };
        
        // image files
        int i = 0, j = 0;
        for (const auto &entry: fjson["imgs"])
        {
            std::string name = entry.get<std::string>();
            copystr(names.imgs[i].name, name);
            i++;
        }
        
        i = 0, j = 0;
        for (const auto &entry: fjson["atlases"])
        {
            std::string name = entry["name"].get<std::string>();
            copystr(names.atlases[i].name, name);
            
            names.atlases[i].subtex_n = entry["sub_tex"].size();
            names.atlases[i].subtex   = (Asset_Name *) (base_mem + basemem_offset);
            basemem_offset += sizeof(Asset_Name) * entry["sub_tex"].size();

            
            j = 0;
            for (const auto &subtex : entry["sub_tex"])
            {
                std::string subtex_name = subtex.get<std::string>();
                
                copystr(names.atlases[i].subtex[j].name, subtex_name);
                j++;
            }
            i++;
        }
        
        i = 0, j = 0;
        for (const auto &entry: fjson["shaders"])
        {
            std::string name = entry.get<std::string>();
            copystr(names.shaders[i].name, name);
            
            i++;
        }

        DEBUG_LOG("Loaded AssetNames json %s\n", json_file);
    }
}