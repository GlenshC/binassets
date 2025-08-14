#include "./imgbin.h"
#include "stb_image/stb_image.h"

namespace ggb
{

    static char* string_readf(const char* path, int &size);
    static std::string get_dir_fpath(const std::string &filepath);


    AssetDataIMG::AssetDataIMG(const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key)
    {
        data = stbi_load((file).c_str(), &x, &y, &channels, 0);
        if (data)
        {
            DEBUG_LOG("AssetDataIMG: read success %s.\n", file.c_str());
            return;
        }
        DEBUG_LOG("AssetDataIMG: failed to initialize %s.\n", file.c_str());
    }
    AssetDataIMG::AssetDataIMG(unsigned char *fdata, const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key), data(fdata)
    {
    }

    AssetDataShader::AssetDataShader(const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key)
    {
        data = string_readf((file).c_str(), count);
        if (data)
        {
            DEBUG_LOG("AssetDataShader: read success %s.\n", file.c_str());
            return;
        }
        DEBUG_LOG("AssetDataShader: failed to read %s.\n", file.c_str());
    }

    AssetDataShader::AssetDataShader(char *fdata, const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key), data(fdata)
    {
    }

    AssetDataAtlas::AssetDataAtlas(const std::string &fxml_path, const std::string &fhash_key)
        : xml_path(fxml_path), hash_key(fhash_key)
    {
        dir = get_dir_fpath(xml_path);
        assets_load_xml(*this, fxml_path.c_str());

        data = stbi_load((dir + "/" + file).c_str(), &x, &y, &channels, 0);
        if (data)
        {
            DEBUG_LOG("AssetDataAtlas: read success %s.\n", (dir + "/" + file).c_str());
            return;
        }
        DEBUG_LOG("AssetDataAtlas: loaded xml");
    }

    
    AssetDataAtlas::AssetDataAtlas(unsigned char *fdata, const std::string &fpath, const std::string &fhash_key)
        : file(fpath), hash_key(fhash_key), data(fdata)
    {

    }

    static char* string_readf(const char* path, int &size)
    {
        std::FILE *file = std::fopen(path, "r");
        if (!file)
        {
            return NULL;
        }
    
        std::fseek(file, 0, SEEK_END);
        size = std::ftell(file);
        std::fseek(file, 0, SEEK_SET);
        
        char *str = (char *)std::malloc(size + 1);
        str[std::fread(str, sizeof(char), size, file)] = '\0';
    
        return str;
    }

    static std::string get_dir_fpath(const std::string &filepath)
    {
        size_t pos = filepath.find_last_of("/\\");
        if (pos == std::string::npos)
            return ""; // no directory part
        return filepath.substr(0, pos);
    }
}