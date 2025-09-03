#pragma once

namespace binassets
{
    struct AssetIMG;
    struct AssetAtlas;
    struct AssetShader;
    struct AssetBinData;
    
    extern bool g_asset_free_on_load; 
    extern AssetBinData  g_assets;
    extern AssetShader  *g_shaders;
    extern AssetIMG     *g_imgs;
    extern AssetAtlas   *g_atlases;
}