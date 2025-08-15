#pragma once


#include <string>
#include <vector>

#include "util_debug.h"

namespace ggb
{

#ifndef BINASSET_COMMON_TYPES
#define BINASSET_COMMON_TYPES
    struct SubTextureDims
    {
        int x, y, width, height;
    };

    struct AssetCounts
    {
        int imgs;
        int atlases;
        int shaders;
        int subtex;
    };

    struct AssetIMGInfo
    {
        int channels, x, y;
    };
    struct AssetAtlasInfo
    {
        int channels, x, y, sub_n;
    };
#endif

    struct AssetDataIMG
    {
        std::string file;
        std::string hash_key;
        unsigned char *data;
        int channels;
        int x;
        int y;

        AssetDataIMG(const std::string &file, const std::string &fhash_key);
        AssetDataIMG(unsigned char *fdata, const std::string &file, const std::string &fhash_key); 

        // operator AssetIMG() const 
        // {
        //     return AssetIMG{
        //         .data     = data,
        //         .channels = channels,
        //         .x        = x,
        //         .y        = y,
        //     };
        // }
    };
    
    // TODO: rm ext member
    struct AssetDataAtlas
    {
        std::string xml_path; // full
        std::string dir;
        std::string file;

        std::vector<SubTextureDims> sprite_dims;
        std::vector<std::string>    sprite_names;

        std::string hash_key;
        unsigned char *data;
        int channels;
        int x;
        int y;

        // operator AssetAtlas() const
        // {
        //     return AssetAtlas{
        //         .sprite_dims = {},
        //         .data = data,
        //         .channels = channels,
        //         .x = x,
        //         .y = y
        //     };
        // }

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

        // operator AssetShader() const 
        // {
        //     return AssetShader{
        //         .data     = data,
        //         .count    = count
        //     };
        // }
    };

    /* WRITING */
    void assets_create_bin(const char *json_file, const char *bin_path_out);
}