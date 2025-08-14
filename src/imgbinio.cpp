#include "./imgbin.h"

#include <cstring>
#include <cstdio>

uint32_t fill_bits(uint32_t x) {
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x;
}

#define GC_BINCEIL32(x) (--(x), (x) |= (x) >> 1, (x) |= (x) >> 2, (x) |= (x) >> 4, (x) |= (x) >> 8, (x) |= (x) >> 16, ++(x))


namespace ggb
{
    /* WRITING */
    void assets_write_bin(std::vector<AssetDataIMG> &imgs,  std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *bin_path_out)
    {
        char header[] = "glnsh";
        std::string bin_path = bin_path_out; 
        std::string path_out = bin_path + ".glnsh";
        std::string path_h_out = bin_path + ".h";

        DEBUG_LOG("Writing asset binaries %s...\n", path_out.c_str());
        std::FILE *file = std::fopen(path_out.c_str(), "wb");

        if (!file) 
        {
            DEBUG_LOG("Failed to compile %s", path_out.c_str());
            return;
        }

        /* header */
        std::fwrite(header, sizeof(char), 5, file);
        
        /* IMG, ATLAS, SHADER, SUBTEX COUNTS */
        AssetCounts dcounts = {};
        dcounts.imgs = imgs.size();
        dcounts.atlases = atlases.size();
        dcounts.shaders = shaders.size();
        dcounts.subtex = 0;
        
        std::vector<int> buffer;
        std::vector<SubTextureDims> sub_tex;

        size_t total_hash_chars = 0;
        size_t img_hash_chars = 0;
        size_t atlas_hash_chars = 0;
        size_t shader_hash_chars = 0;
        size_t total_subtex_chars = 0;
        
        /* PER IMG, ATLAS, SHADER INFOS */

        for (auto &img : imgs) 
        {
            buffer.push_back(img.channels);
            buffer.push_back(img.x);
            buffer.push_back(img.y);

            img_hash_chars += img.hash_key.size() + 1;
        }
        
        for (auto &atlas : atlases)
        {
            int sub_n = (int)atlas.sprite_names.size();
            dcounts.subtex += sub_n;
            
            buffer.push_back(atlas.channels);
            buffer.push_back(atlas.x);
            buffer.push_back(atlas.y);
            buffer.push_back(sub_n);

            atlas_hash_chars += atlas.hash_key.size() + 1;
            
            for (int i =0; i < (int)atlas.sprite_dims.size(); i++)
            {
                sub_tex.push_back(atlas.sprite_dims[i]);
                total_subtex_chars += atlas.sprite_names[i].size() + 1;
            }
        }
        
        for (auto &shader : shaders) 
        {
            shader_hash_chars += shader.hash_key.size() + 1;

            buffer.push_back(shader.count);
        }
        total_hash_chars = img_hash_chars + atlas_hash_chars + shader_hash_chars;

        /* DATA COUNTS */
        std::fwrite(&dcounts, sizeof(int), 4, file);

        /* INFOS */
        std::fwrite(buffer.data(), sizeof(int), buffer.size(), file);
        buffer = {};

        /* SUBTEX */
        std::fwrite(sub_tex.data(), sizeof(SubTextureDims), sub_tex.size(), file);
        
        // REMOVE

        /* PER IMG, ATLAS, SHADER DATA */
        for (auto &img : imgs) 
        {
            std::fwrite(img.data, sizeof(unsigned char), img.channels * img.x * img.y, file);
        }

        for (auto &atlas : atlases) 
        {
            std::fwrite(atlas.data, sizeof(unsigned char), atlas.channels * atlas.x * atlas.y, file);
        }
       
        for (auto &shader : shaders) 
        {
            std::fwrite(shader.data, sizeof(char), shader.count, file);
        }

        DEBUG_LOG("Successfully compiled asset binaries %s\n", path_out.c_str());

        std::fclose(file);

        /* WRITE .h FILE */
        DEBUG_LOG("Creating enums .h file %s\n", path_h_out.c_str())
        file = std::fopen(path_h_out.c_str(), "wb");

        if (!file)
        {
            DEBUG_LOG("Failed to create file %s\n", path_h_out.c_str());
            return;
        }

        char *img_hash_keys = nullptr;
        char *atlas_hash_keys = nullptr;
        char *shader_hash_keys = nullptr;
        (void) img_hash_keys;
        (void) atlas_hash_keys;
        (void) shader_hash_keys;

        std::string img_header = "#pragma once\n\nnamespace IMG {\nenum {";
        std::string atlas_header = "namespace ATLAS {\nenum {";
        std::string shader_header = "namespace SHADER {\nenum {";
        std::string footer = "};\n}\n";

        std::vector<size_t> atlas_subtex_keys(dcounts.atlases);
        std::vector<char> hash_keys(total_hash_chars + img_header.size() + atlas_header.size() + shader_header.size() + 64);
        std::vector<char> subtex_keys(total_subtex_chars + atlas_hash_chars + shader_header.size() * dcounts.atlases + footer.size() * dcounts.atlases + 64);

        size_t hash_offset = 0;
        size_t subtex_offset = 0;
        
        auto copy_str = [&](std::vector<char>& buffer, size_t& offset, const std::string& s) {
            std::memcpy(buffer.data() + offset, s.data(), s.size());
            offset += s.size();
            buffer.data()[offset++] = '\n';
        };
        auto copy_str_comma = [&](std::vector<char>& buffer, size_t& offset, const std::string& s) {
            std::memcpy(buffer.data() + offset, s.data(), s.size());
            offset += s.size();
            buffer.data()[offset++] = ',';
            buffer.data()[offset++] = '\n';
        };
        // TODO finish header output

        /* IMG ENUM CLASS */
        img_hash_keys = hash_keys.data();
        copy_str(hash_keys, hash_offset, img_header); //header
        for (auto &img : imgs) 
        {
            copy_str_comma(hash_keys, hash_offset, img.hash_key);
        }
        copy_str(hash_keys, hash_offset, footer);
        
        /* SHADER ENUM CLASS */
        shader_hash_keys = hash_keys.data() + hash_offset; // header
        copy_str(hash_keys, hash_offset, shader_header);
        for (auto &shader : shaders) 
        {
            copy_str_comma(hash_keys, hash_offset, shader.hash_key);
        }
        copy_str(hash_keys, hash_offset, footer);
        
        /* ATLAS ENUM CLASS */
        atlas_hash_keys = hash_keys.data() + hash_offset;
        copy_str(hash_keys, hash_offset, atlas_header); //header
        for (auto &atlas : atlases)
        {
            copy_str_comma(hash_keys, hash_offset, atlas.hash_key);
            
            /* SUBTEX ENUM CLASS */
            atlas_subtex_keys.push_back(subtex_offset);
            std::string subtex_header = "namespace " + atlas.hash_key + " {\nenum {";
            copy_str(subtex_keys, subtex_offset, subtex_header); // header
            for (auto &s : atlas.sprite_names)
            {
                copy_str_comma(subtex_keys, subtex_offset, s);
            }
            copy_str(subtex_keys, subtex_offset, footer); // header
        }
        copy_str(hash_keys, hash_offset, footer);

        std::fwrite(hash_keys.data(), sizeof(char), hash_offset, file);
        std::fwrite(subtex_keys.data(), sizeof(char), subtex_offset, file);

        std::fclose(file);
        /* 
            enum class IMGS
            enum class SHADER
            enum class ATLAS
            enum class (ATLAS_KEY) ...

        */
    }

    /* READING */
    void assets_read_bin(AssetData &data_out, const char *bin_path)
    {
        char buffer[6] = {};
        // std::string bpath = bin_path;
        std::string header = "glnsh";

        DEBUG_LOG("Reading asset binaries %s...\n", bin_path);
        std::FILE *file = std::fopen(bin_path, "rb");
        if (!file) 
        {
            DEBUG_LOG("File not found %s\n", bin_path);
            return;
        }

        // header
        std::fread(buffer, sizeof(char), 5, file);
        if (buffer != header)
        {
            DEBUG_LOG("Wrong file format: %s\n", bin_path);
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
            // DEBUG_LOG("size: %zu, c: %d, x: %d, y: %d\n", size, img_infos[i].channels, img_infos[i].x, img_infos[i].y);
            
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
