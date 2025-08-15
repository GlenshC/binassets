#pragma once

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
        
        explicit operator bool() const {
            return atlases != nullptr;
        }
    };
    
    /* READING  */
    void assets_load_bin(AssetData &data_out, const char *bin_path);
    void assets_data_free(AssetData &data);
}