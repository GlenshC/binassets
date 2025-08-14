#define DEBUG_ASSET_BIN
#ifdef DEBUG_ASSET_BIN
    #include <cstdio>
    #define DEBUG_LOG(fstr, ...) std::printf(fstr, ##__VA_ARGS__);
#else
    #define DEBUG_LOG(fstr, ...)
#endif