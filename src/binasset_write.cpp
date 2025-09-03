#ifndef _CRT_SECURE_NO_WARNINGS
    #define _CRT_SECURE_NO_WARNINGS
#endif

#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "binassets/binasset_write.h"
#include "binassets/binasset_stl_read.h"

/* STRUCT FUNCTIONS */
namespace binassets
{
    void assets_write_json(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases,std::string path_json_out);

/******************************************************** 
 *                                                      *
 * PUBLIC FUNCTIONS                                     *
 *                                                      *
 ********************************************************/
    
    void assets_create_bin(const char *json_file, const char *bin_path_out)
    {
        if (!json_file || !bin_path_out) return;
        AssetData data;
        
        DEBUG_LOG("Creating asset binaries...\n");
        assets_load_assetlist_json(data, json_file);
        assets_write_bin(data.imgs, data.shaders, data.atlases, bin_path_out);
    }

    

/******************************************************** 
 *                                                      *
 * WRITING FUNCTIONS                                    *
 *                                                      *
 ********************************************************/

    /* WRITING */
    void assets_write_adoboproj(const char *name)
    {
        char header[] = "adoboproj";
        std::FILE *file = std::fopen(name, "wb");

        
        
    }


    void assets_write_bin(std::vector<AssetDataIMG> &imgs,  std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases, const char *bin_path_out)
    {
        char header[] = "glnsh";
        std::string bin_path = bin_path_out; 
        std::string path_out = bin_path + ".glnsh";
        std::string path_h_out = bin_path + ".h";
        std::string path_json_out = bin_path + ".json";

        AssetCounts dcounts = {};
        dcounts.imgs = imgs.size();
        dcounts.atlases = atlases.size();
        dcounts.shaders = shaders.size();
        dcounts.subtex = 0;
        {
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

            /* PER IMG, ATLAS, SHADER INFOS */

            std::vector<SubTextureDims> sub_tex;
            std::vector<int> buffer;
            for (auto &img : imgs)
            {
                buffer.push_back(img.channels);
                buffer.push_back(img.x);
                buffer.push_back(img.y);
            }

            for (auto &atlas : atlases)
            {
                int sub_n = (int)atlas.subtex.size();
                dcounts.subtex += sub_n;

                buffer.push_back(atlas.channels);
                buffer.push_back(atlas.x);
                buffer.push_back(atlas.y);
                buffer.push_back(sub_n);
                DEBUG_LOG("Atlas: %d %d %d %d\n", atlas.channels, atlas.x, atlas.y, sub_n);

                for (size_t i = 0; i < atlas.subtex.size(); i++)
                {
                    auto &subtex = atlas.subtex[i];
                    sub_tex.push_back({subtex.x,
                                       subtex.y,
                                       subtex.width,
                                       subtex.height});
                }
            }

            for (auto &shader : shaders)
            {

                buffer.push_back(shader.count);
                DEBUG_LOG("shader[i].count = %d;\n", shader.count);
            }

            /* DATA COUNTS */
            std::fwrite(&dcounts, sizeof(int), 4, file);

            /* INFOS */
            std::fwrite(buffer.data(), sizeof(int), buffer.size(), file);

            std::fwrite(sub_tex.data(), sizeof(SubTextureDims), sub_tex.size(), file);

            /* SUBTEX */
            
            // REMOVE
    
            /* PER IMG, ATLAS, SHADER DATA */
            for (auto &img : imgs) 
            {
                std::fwrite(img.data, sizeof(unsigned char), img.channels * img.x * img.y, file);
                free(img.data);
            }
    
            for (auto &atlas : atlases) 
            {
                std::fwrite(atlas.data, sizeof(unsigned char), atlas.channels * atlas.x * atlas.y, file);
                free(atlas.data);
            }
           
            for (auto &shader : shaders) 
            {
                std::fwrite(shader.data, sizeof(char), shader.count, file);
                free(shader.data);
            }
    
            DEBUG_LOG("Successfully compiled asset binaries %s\n", path_out.c_str());
    
            std::fclose(file);
        }

        /* WRITE .h FILE */
        {
            DEBUG_LOG("Creating enums .h file %s\n", path_h_out.c_str())
            FILE *file = std::fopen(path_h_out.c_str(), "wb");

            if (!file)
            {
                DEBUG_LOG("Failed to create file %s\n", path_h_out.c_str());
                return;
            }

            std::string h_header = "#pragma once\n";
            std::string img_header = "namespace BSST_IMG {\n    enum {";
            std::string atlas_header = "namespace BSST_ATLAS {\n    enum {";
            std::string shader_header = "namespace BSST_SHADER {\n    enum {";
            std::string bracket = "};\n";
            std::string names_var = "const char *NAMES[] = {";

            const size_t buffer_cap = 4092;
            size_t buffer_size = 0;
            char *out_buffer = (char *)std::malloc(buffer_cap + 1);
            out_buffer[buffer_size] = 0;
            std::string indent1 = "    ";
            std::string indent2 = "        ";

            auto writeout_raw = [&](const std::string &s)
            {
                if (buffer_size + s.size() >= buffer_cap)
                {
                    std::fwrite(out_buffer, 1, buffer_size, file);
                    buffer_size = 0;
                }
                std::memcpy(out_buffer + buffer_size, s.data(), s.size());
                buffer_size += s.size();
            };

            auto writeout = [&](const std::string &s)
            {
                if (buffer_size + s.size() + 1 >= buffer_cap)
                {
                    std::fwrite(out_buffer, 1, buffer_size, file);
                    buffer_size = 0;
                }
                std::memcpy(out_buffer + buffer_size, s.data(), s.size());
                buffer_size += s.size();
                out_buffer[buffer_size++] = '\n';
            };
            auto writeoutln = [&](const std::string &s)
            {
                if (buffer_size + s.size() + 2 >= buffer_cap)
                {
                    std::fwrite(out_buffer, 1, buffer_size, file);
                    buffer_size = 0;
                }
                std::memcpy(out_buffer + buffer_size, s.data(), s.size());
                buffer_size += s.size();
                out_buffer[buffer_size++] = ',';
                out_buffer[buffer_size++] = '\n';
            };

            auto writeout_str = [&](const std::string &s)
            {
                if (buffer_size + s.size() + 4 >= buffer_cap)
                {
                    std::fwrite(out_buffer, 1, buffer_size, file);
                    buffer_size = 0;
                }
                out_buffer[buffer_size++] = '"';
                std::memcpy(out_buffer + buffer_size, s.data(), s.size());
                buffer_size += s.size();
                out_buffer[buffer_size++] = '"';
                out_buffer[buffer_size++] = ',';
                out_buffer[buffer_size++] = '\n';
            };
            // TODO finish header output

            /* IMG ENUM CLASS */
            writeout(h_header);
            if (dcounts.imgs)
            {
                writeout(img_header); // header
                for (auto &img : imgs)
                {
                    writeout_raw(indent2);
                    writeoutln(img.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout_raw(indent1);
                writeout(names_var);
                for (auto &img : imgs)
                {
                    writeout_raw(indent2);
                    writeout_str(img.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout(bracket);
            }

            /* SHADER ENUM CLASS */
            if (dcounts.shaders)
            {
                writeout(shader_header);
                for (auto &shader : shaders)
                {
                    writeout_raw(indent2);
                    writeoutln(shader.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout_raw(indent1);
                writeout(names_var);
                for (auto &shader : shaders)
                {
                    writeout_raw(indent2);
                    writeout_str(shader.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout(bracket);
            }

            /* ATLAS ENUM CLASS */
            if (dcounts.atlases)
            {
                writeout(atlas_header); // header
                for (auto &atlas : atlases)
                {
                    writeout_raw(indent2);
                    writeoutln(atlas.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout_raw(indent1);
                writeout(names_var);
                for (auto &atlas : atlases)
                {
                    writeout_raw(indent2);
                    writeout_str(atlas.hash_key);
                }
                writeout_raw(indent1);
                writeout(bracket);
                writeout(bracket);
            }

            /* WRITE SUB TEX */
            if (dcounts.atlases)
            {
                for (auto &atlas : atlases)
                {
                    /* SUBTEX ENUM CLASS */
                    std::string subtex_header = "namespace BSST_" + atlas.hash_key + " {\n    enum {";
                    writeout(subtex_header); // header
                    for (auto &s : atlas.subtex)
                    {
                        writeout_raw(indent2);
                        writeoutln(s.name);
                    }
                    writeout_raw(indent1);
                    writeout(bracket);
                    writeout_raw(indent1);
                    writeout(names_var);
                    for (auto &s : atlas.subtex)
                    {
                        writeout_raw(indent2);
                        writeout_str(s.name);
                    }

                    writeout_raw(indent1);
                    writeout(bracket);
                    writeout(bracket);
                }
            }

            std::fwrite(out_buffer, sizeof(char), buffer_size, file); // leftovers
        
            std::fclose(file);
        }
        assets_write_json(imgs, shaders, atlases, path_json_out);
    }
    
    void assets_write_json(std::vector<AssetDataIMG> &imgs, std::vector<AssetDataShader> &shaders, std::vector<AssetDataAtlas> &atlases,std::string path_json_out)
    {
        (void) imgs, (void) shaders, (void) atlases;
        const size_t buffer_cap = 4092;
        size_t buffer_size = 0;
        char *out_buffer = (char *)std::malloc(buffer_cap + 1);
        out_buffer[buffer_size] = 0;
        std::string indent1 = "  ";

        std::FILE *file = std::fopen(path_json_out.c_str(), "wb");
        auto writeout_raw = [&](const std::string &s)
        {
            if (buffer_size + s.size() >= buffer_cap)
            {
                std::fwrite(out_buffer, 1, buffer_size, file);
                buffer_size = 0;
            }
            std::memcpy(out_buffer + buffer_size, s.data(), s.size());
            buffer_size += s.size();
        };

        auto writeout_str = [&](const std::string &s)
        {
            if (buffer_size + s.size() + 4 >= buffer_cap)
            {
                std::fwrite(out_buffer, 1, buffer_size, file);
                buffer_size = 0;
            }
            out_buffer[buffer_size++] = '"';
            std::memcpy(out_buffer + buffer_size, s.data(), s.size());
            buffer_size += s.size();
            out_buffer[buffer_size++] = '"';
        };

        writeout_raw("{\n  \"imgs\": [\n");
        for (size_t i = 0; i < imgs.size(); i++)
        {
            writeout_raw("    ");
            writeout_str(imgs[i].hash_key);
            if (i < imgs.size() - 1) writeout_raw(",\n");
            else writeout_raw("\n");
        }

        writeout_raw("  ],\n  \"atlases\": [\n");
        for (size_t i = 0; i < atlases.size(); i++)
        {
            writeout_raw("    {\n      \"name\":\""+atlases[i].hash_key+"\",\n      \"sub_tex\": [\n");
            auto &subtex = atlases[i].subtex;
            for (size_t j = 0; j < atlases[i].subtex.size(); j++)
            {
                writeout_raw("        ");
                writeout_str(subtex[j].name);
                if (j < subtex.size() - 1) writeout_raw(",\n");
                else writeout_raw("\n");
            }
            writeout_raw("      ]\n    }");

            if (i < atlases.size() - 1) writeout_raw(",\n");
            else writeout_raw("\n");
        }
        writeout_raw("  ],\n  \"shaders\": [\n");
        for (size_t i = 0; i < shaders.size(); i++)
        {
            writeout_raw("    ");
            writeout_str(shaders[i].hash_key);
            if (i < shaders.size() - 1) writeout_raw(",\n");
            else writeout_raw("\n");
        }
        writeout_raw("  ]\n}\n");
        std::fwrite(out_buffer, sizeof(char), buffer_size, file); // leftovers
        std::fclose(file);
    }
}
    






/******************************************************** 
 *                                                      *
 * HELPER STATIC FUNCTIONS                              *
 *                                                      *
 ********************************************************/

