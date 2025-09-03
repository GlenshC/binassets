#pragma once

#define DEBUG_ASSET_BIN
#ifdef DEBUG_ASSET_BIN
    #include <cstdio>
    #define DEBUG_LOG(fstr, ...) std::printf(fstr, ##__VA_ARGS__);
    #define DEBUG_ERR(fstr, ...) std::fprintf(stderr, fstr, ##__VA_ARGS__);

    template <typename T>
    inline void debug_mem(T *arr, size_t offset_end)
    {
        DEBUG_LOG("\n");
        for (int i = 0; i < 16; i++)
        {
            DEBUG_LOG("%5x ", i);
        }
        DEBUG_LOG("\n");
        for (size_t i = 0, aligncount = 0; i < offset_end; i++)
        {
            DEBUG_LOG("%5d ", arr[i]);
            aligncount++;
            if (aligncount >= 16)
            {
                DEBUG_LOG("\n");
                aligncount = 0;
            }
        }
        DEBUG_LOG("\n")
    }
#else
    #define DEBUG_LOG(fstr, ...)
    #define DEBUG_ERR(fstr, ...)
#endif

