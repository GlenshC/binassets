#include "imgbin.h"
#include "bin.h"
// #define ASSET_CONVERT_MAIN

#ifdef ASSET_CONVERT_MAIN
int main(int argc, char *argv[])
{
    DEBUG_LOG("Binary Asset Compiler.\n\n");
    if (argc != 3) 
    {
        DEBUG_LOG("USAGE: %s [json_file_path] [out_glnsh_file].\n", argv[0]);
        return 1;
    }
    ggb::assets_create_bin(argv[1], argv[2]);

    return 0;
}
#else //defined(ASSET_LOADER_MAIN)
int main(int argc, char *argv[])
{
    DEBUG_LOG("Binary Asset Loader.\n\n");
    if (argc != 2)
    {
        DEBUG_LOG("USAGE: %s [glnsh_file_path]", argv[0]);
        return 1;
    }
    
    ggb::AssetData data;
    ggb::assets_load_bin(data, argv[1]);

    DEBUG_LOG("shoot-duck_back.h: %d", data.atlases[ATLAS::shoot][shoot::DUCK_BACK_PNG].height);

    ggb::assets_data_free(data);
}
#endif