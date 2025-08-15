#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif
#include "binassets/binasset_read.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define GC_BINCEIL32(x) (--(x), (x) |= (x) >> 1, (x) |= (x) >> 2, (x) |= (x) >> 4, (x) |= (x) >> 8, (x) |= (x) >> 16, ++(x))


namespace ggb
{
    static void assets_read_bin(AssetData &data_out, const char *bin_path);


    void assets_load_bin(AssetData &data_out, const char *bin_path)
    {        
        if (!bin_path) return;

        DEBUG_LOG("Loading asset binaries...\n");
        assets_read_bin(data_out, bin_path);
    }

    void assets_data_free(AssetData &data)
    {
        if (data.atlases == nullptr)
        {
            DEBUG_LOG("Failed to free binary assets resources.\n");
            return;
        }
        std::free(data.atlases);
        data.atlases = nullptr;
        data.imgs = nullptr;
        data.shaders = nullptr;
        data.atlases_size = 0;
        data.imgs_size = 0;
        data.shaders_size = 0;

        DEBUG_LOG("Binary Assets Freed.\n");
    }

    /* READING */
    static void assets_read_bin(AssetData &data_out, const char *bin_path)
    {
        char buffer[6] = {};
        char header[] = "glnsh";

        DEBUG_LOG("Reading asset binaries %s...\n", bin_path);
        std::FILE *file = std::fopen(bin_path, "rb");
        if (!file) 
        {
            DEBUG_LOG("File not found %s\n", bin_path);
            return;
        }

        // header
        std::fread(buffer, sizeof(char), 5, file);
        if (strcmp(buffer, header) != 0)
        {
            DEBUG_LOG("Wrong file format: %s\n", bin_path);
            std::fclose(file);
            return;   
        }
        
        /* DATA COUNTS */
        AssetCounts dcounts = {};
        std::fread(&dcounts, sizeof(int), 4, file);

        size_t total_data_size = 
            dcounts.atlases * sizeof(AssetAtlas) + 
            dcounts.subtex * sizeof(SubTextureDims) + 
            dcounts.imgs * sizeof(AssetIMG) + 
            dcounts.shaders * sizeof(AssetShader);

        unsigned char  *data_mem = (unsigned char *)malloc(total_data_size);
        AssetAtlas     *atlases  = (AssetAtlas *)data_mem;
        AssetIMG       *imgs     = (AssetIMG *) (atlases + dcounts.atlases);
        AssetShader    *shaders  = (AssetShader *) (imgs + dcounts.imgs);
        SubTextureDims *subtex   = (SubTextureDims *) (shaders + dcounts.shaders);

        data_out.atlases_size = dcounts.atlases;
        data_out.imgs_size = dcounts.imgs;
        data_out.shaders_size = dcounts.shaders;

        data_out.atlases = atlases;
        data_out.imgs    = imgs;
        data_out.shaders = shaders;

        int total_info_count = dcounts.imgs * 3 + dcounts.atlases * 4 + dcounts.shaders;
        
        int                 *base_buffer  = (int *)malloc(sizeof(int) * (total_info_count));
        ggb::AssetIMGInfo   *img_infos    = (ggb::AssetIMGInfo *)base_buffer;
        ggb::AssetAtlasInfo *atlas_infos  = (ggb::AssetAtlasInfo *) img_infos + dcounts.imgs;
        int                 *shader_infos = (int *) (atlas_infos + dcounts.atlases);

        /* INFOS */
        std::fread(base_buffer, sizeof(int), total_info_count, file);

        /* SUBTEX */
        std::fread(subtex, sizeof(SubTextureDims), dcounts.subtex, file);

        /* PER DATA */

        for (int i = 0; i < dcounts.imgs; i++)
        {
            size_t size = img_infos[i].channels * img_infos[i].x * img_infos[i].y;
            
            unsigned char *data = (unsigned char *)std::malloc(size);
            std::fread(data, sizeof(unsigned char), size, file);

            imgs[i].data = data;
            imgs[i].channels = img_infos[i].channels;
            imgs[i].x = img_infos[i].x;
            imgs[i].y = img_infos[i].y;
        }
        
        for (int i =0; i < dcounts.atlases; i++)
        {
            size_t size = atlas_infos[i].channels * atlas_infos[i].x * atlas_infos[i].y;

            unsigned char *data = (unsigned char *)std::malloc(size);
            std::fread(data, sizeof(unsigned char), size, file);

            auto &atlas = atlases[i];
            atlas.channels    = atlas_infos[i].channels;
            atlas.x           = atlas_infos[i].x;
            atlas.y           = atlas_infos[i].y;
            atlas.sprite_dims = subtex;

            subtex += atlas_infos[i].sub_n;
        }
        
        for (int i = 0; i < dcounts.shaders; i++)
        {
            size_t size = shader_infos[i];
            
            char * data = (char *)std::malloc(size + 1);
            std::fread(data, sizeof(char), size, file);
            data[size] = 0;

            auto &shader = shaders[i];
            shader.data  = data; 
            shader.count = size;
        }

        DEBUG_LOG("Successfully loaded binaries %s\n", bin_path);
        std::free(base_buffer);

        std::fclose(file);
    }
}
