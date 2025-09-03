#pragma once

namespace binassets
{
    #define BSST_ASSET_MAX_NAME 32ULL
    struct Asset_Name;
    struct AssetNameAtlas; 
    typedef Asset_Name AssetNameIMG, AssetNameShader, AssetNameSubtex;

    typedef struct Asset_Name
    {
        char name[BSST_ASSET_MAX_NAME];
    } AssetNameIMG, AssetNameShader, AssetNameSubtex;

    struct AssetNameAtlas
    {
        AssetNameSubtex *subtex;
        char name[BSST_ASSET_MAX_NAME];
        int subtex_n;
    };

    struct AssetNames
    {
        char            *_bp     = nullptr;
        AssetNameAtlas  *atlases = nullptr;
        AssetNameIMG    *imgs    = nullptr;
        AssetNameShader *shaders = nullptr;
        int size_atlases = 0;
        int size_imgs    = 0;
        int size_shaders = 0;
        int size_subtex  = 0;
    };

    void assets_test(AssetNames &names);
    void assets_load_json(AssetNames &names, const char *json_file);
    void assets_free(AssetNames &names);
}

namespace bsst = binassets;