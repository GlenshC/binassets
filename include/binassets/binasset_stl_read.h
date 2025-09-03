#pragma once

#include "binassets/binasset_common.h"
#include "binassets/binasset_stl_read_decl.h"

#ifdef ADOBO_GAME_ENGINE
#include "renderer/texture.h"
#endif

#include <vector>
#include <string>
#include <unordered_map>
namespace binassets
{
    struct AssetData
    {
        // std::vector<AssetDataIMG>    imgs;
        std::vector<AssetDataAtlas>  atlases;
        std::vector<AssetDataShader> shaders;
        std::unordered_map<std::string, bool> loaded;
    };

    typedef SubTextureDims AssetDataSubtex;

    struct AssetDataIMG
    {
        std::string    file;
        std::string    hash_key;
        unsigned char *data;
        int channels;
        int x;
        int y;

        AssetDataIMG(const std::string &file, const std::string &fhash_key);
        AssetDataIMG(unsigned char *fdata, const std::string &file, const std::string &fhash_key); 
    };
    
    // TODO: rm ext member
    struct AssetDataAtlas
    {
        std::string xml_path; // full xml file dir
        std::string file;
        std::string dir;

        std::vector<AssetDataSubtex>  subtex;
        std::vector<std::string>  subtex_names;

        std::string hash_key;
        unsigned char *data;
        int channels;
        int x;
        int y;

        #ifdef ADOBO_GAME_ENGINE
        texture::Texture tex;
        #endif

        AssetDataAtlas(const std::string &fxml_path, const std::string &fhash_key);
        AssetDataAtlas(unsigned char *data, const std::string &fpath, const std::string &fhash_key);
    };

    struct AssetDataShader
    {
        std::string file;
        std::string hash_key;
        char *data;
        int count;

        AssetDataShader(const std::string &fpath, const std::string &fhash_key);
        AssetDataShader(char *fdata, const std::string &fpath, const std::string &fhash_key);
    };

    int assets_load_atlas(AssetData &assets, const char *path, const char* name);
    int assets_load_img(AssetData &asset, const char *path, const char* name);
    int assets_load_shader(AssetData &assets, const char *path, const char* name);
    void assets_load_assetlist_json(AssetData &assets, const char *json_file);
    void assets_load_xml(AssetDataAtlas &atlas, const char *xml_file);

    void assets_free(AssetData &assets);
}
namespace bsst = binassets;