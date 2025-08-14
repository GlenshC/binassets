#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <vector>
#include "util_debug.h"

namespace ggb
{
    struct SubTextureDims
    {
        int x, y, width, height;
    };

    struct AssetIMG
    {
        unsigned char *data;
        int channels;
        int x;
        int y;
    };

    struct AssetAtlas
    {
        SubTextureDims *sprite_dims; // an arr
        unsigned char  *data;
        int channels;
        int x;
        int y;

        SubTextureDims& operator[](size_t index)
        {
            return sprite_dims[index];
        }
    };
    
    struct AssetShader
    {
        char *data;
        int count;
    };
    
    struct AssetData
    {
        AssetAtlas  *atlases = nullptr;
        AssetIMG    *imgs    = nullptr;
        AssetShader *shaders = nullptr;
        size_t atlases_size  = 0;
        size_t imgs_size     = 0;
        size_t shaders_size  = 0;
    };

    // subtext

    struct PathExt
    {
        std::string dir;
        std::string ext;
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

        operator AssetIMG() const 
        {
            return AssetIMG{
                .data     = data,
                .channels = channels,
                .x        = x,
                .y        = y,
            };
        }
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

        operator AssetAtlas() const
        {
            return AssetAtlas{
                .sprite_dims = {},
                .data = data,
                .channels = channels,
                .x = x,
                .y = y
            };
        }

        AssetDataAtlas(const std::string &fxml_path, const std::string &fhash_key);
        AssetDataAtlas(unsigned char *data, const std::string &fpath, const std::string &fhash_key);

        // operator AssetAtlas() const {}
    };

    struct AssetDataShader
    {
        std::string file;
        std::string hash_key;
        char *data;
        int count;

        AssetDataShader(const std::string &fpath, const std::string &fhash_key);
        AssetDataShader(char *fdata, const std::string &fpath, const std::string &fhash_key);

        operator AssetShader() const 
        {
            return AssetShader{
                .data     = data,
                .count    = count
            };
        }

    };

    void assets_create_bin(const char *json_file, const char *bin_path_out);
    void assets_load_bin(AssetData &data_out, const char *bin_path);
    void assets_data_free(AssetData &data);

    void assets_read_bin(AssetData &data_out, const char *bin_path);
    void assets_write_bin(std::vector<AssetDataIMG> &imgs,  std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *bin_path_out);
    void assets_load_json(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *json_file);
    void assets_load_xml(AssetDataAtlas &atlas, const char *xml_file);
    void assets_vector_free(AssetDataIMG &img);
    void assets_vector_free(AssetDataShader &shader);
    void assets_free(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders);

}