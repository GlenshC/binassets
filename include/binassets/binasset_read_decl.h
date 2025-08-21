#pragma once

namespace binassets
{
    struct AssetIMG;
    struct AssetAtlas;
    struct AssetShader;
    struct AssetData;
    
    extern bool g_asset_free_on_load; 
    extern AssetData     g_assets;
    extern AssetShader *&g_shaders;
    extern AssetIMG    *&g_imgs;
    extern AssetAtlas  *&g_atlases;
}