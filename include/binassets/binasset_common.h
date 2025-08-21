#pragma once

namespace binassets
{
    struct SubTextureDims
    {
        float x, y, width, height;
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
}