#pragma once

#ifdef ADOBO_GAME_ENGINE
#include "util/debug.h"
#include "renderer/texture.h"
#include "renderer/shader.h"
#else
#include "util_debug.h"
#endif

#include "binasset_common.h"
#include "binasset_read_decl.h" // global externs and forward declares

namespace binassets
{
    /* FUNCTIONS */
    void assets_load_bin(const char *bin_path); // globals
    void assets_load_bin_s(AssetData &data_out, const char *bin_path);
    void assets_data_free(AssetData &data);
    void assets_free_on_load(bool enabled);

    #ifdef ADOBO_GAME_ENGINE
    void assets_upload_atlases(AssetData &data);
    #endif

    
    /* TYPE DEFS */
    struct AssetIMG
    {
        unsigned char *data = nullptr;
        int channels = 0;
        int x = 0;
        int y = 0;
    #ifdef ADOBO_GAME_ENGINE
        texture::Texture tex;
        operator const texture::Texture&()
        {
            return tex;
        }
    #endif
    };

    struct AssetAtlas
    {
        SubTextureDims *dims = nullptr; // an arr
        unsigned char  *data   = nullptr;
        
        int subtex_n = 0; 
        int channels = 0;
        int x = 0;
        int y = 0;

    #ifdef ADOBO_GAME_ENGINE
        texture::Texture tex;
        operator const texture::Texture&()
        {
            return tex;
        }
        texture::TextureRef operator()()
        {
            return tex();
        }
        const adobo::vec4f& operator[](size_t index)
        {
            return tex[index];
        }
    #endif

    };
    
    struct AssetShader
    {
        char *data = nullptr;
        int count  = 0;

        #ifdef ADOBO_GAME_ENGINE
        shader::Shader shader;
        operator const shader::Shader &()
        {
            return shader;
        }
        #endif
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
}

namespace bsst = binassets;