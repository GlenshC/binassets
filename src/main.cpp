// #define ASSET_CONVERT_MAIN

#include "util_debug.h"
#include "stb_image/stb_image.h"
#ifdef ASSET_CONVERT_MAIN
    #include "binassets/binasset_write.h"
    int main(int argc, char *argv[])
    {
        DEBUG_LOG("Binary Asset Compiler.\n\n");
        if (argc != 3) 
        {
            DEBUG_LOG("USAGE: %s [json_file_path] [out_glnsh_file].\n", argv[0]);
            return 1;
        }
        binassets::assets_create_bin(argv[1], argv[2]);

        return 0;
    }

#else
    #include "binassets/binasset_read.h"
    #include "binassets/binasset_json.h"
    #include "../bin.h"
    int main(int argc, char *argv[])
    {
        DEBUG_LOG("Binary Asset Loader.\n\n");
        if (argc != 2)
        {
            DEBUG_LOG("USAGE: %s [glnsh_file_path]", argv[0]);
            return 1;
        }
        
        // binassets::assets_load_bin(argv[1]);
        bsst::AssetNames names;
        binassets::assets_load_json(names, argv[1]);
        binassets::assets_test(names);
        bsst::assets_free(names);

    }

#endif