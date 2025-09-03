#pragma once

#include <string>
#include <vector>

#ifdef ADOBO_GAME_ENGINE
// #include "types.h"
#include "util/debug.h"
#else
#include "util_debug.h"
#endif

#include "binassets/binasset_common.h"
#include <string>
namespace binassets
{


    /* WRITING */
    void assets_create_bin(const char *json_file, const char *bin_path_out);
    void assets_write_adoboproj(const char *name);
}

namespace bsst = binassets;