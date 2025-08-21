cmake_minimum_required(VERSION 3.14)
project(binassets CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===============================
# Compiler Flags
# ===============================
if(MSVC)
    add_compile_options(/W4 /WX /fsanitize=address /Oy-)
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /fsanitize=address /Oy-")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
else()
    add_compile_options(-Wall -Wextra -Werror)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
endif()

# ===============================
# binasset_read (standalone, no deps)
# ===============================
add_library(binasset_read STATIC
    src/binasset_read.cpp
)
target_include_directories(binasset_read
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ===============================
# tinyxml2 (internal to binasset_write)
# ===============================
add_library(tinyxml2 STATIC
    vendor/nlohmann/tinyxml2.cpp
)
target_include_directories(tinyxml2
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

# ===============================
# binasset_write (depends on tinyxml2)
# ===============================
add_library(binasset_write STATIC
    src/binasset_write.cpp
)
target_include_directories(binasset_write
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/vendor
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(binasset_write
    PRIVATE
        tinyxml2
)

# Uncomment if binasset_write also needs binasset_read:
# target_link_libraries(binasset_write PRIVATE binasset_read)

# ===============================
# MSVC ASan runtime (for clang-cl)
# ===============================
if(MSVC)
    foreach(lib_target IN ITEMS binasset_read binasset_write)
        target_link_libraries(${lib_target}
            PRIVATE
                "C:/Program Files/LLVM/lib/clang/20/lib/windows/clang_rt.asan_dynamic-x86_64.lib"
                "C:/Program Files/LLVM/lib/clang/20/lib/windows/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
        )
    endforeach()
endif()

# ===============================
# Optional test executable
# ===============================
option(BINASSETS_WRITER_MAIN "Build a standalone test executable" ON)
if (BINASSETS_WRITER_MAIN)
    set_source_files_properties(
        src/main.cpp
        PROPERTIES COMPILE_DEFINITIONS "ASSET_CONVERT_MAIN"
    )
endif()
option(BINASSETS_BUILD_EXE "Build a standalone test executable" OFF)
if(BINASSETS_BUILD_EXE)
    add_executable(binassets_exe
        src/main.cpp
    )
    target_link_libraries(binassets_exe
        PRIVATE
            binasset_read
            binasset_write
    )
endif()
