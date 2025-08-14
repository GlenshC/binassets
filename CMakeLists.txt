cmake_minimum_required(VERSION 3.14)
project(testapp CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /WX /fsanitize=address  /Oy-)
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /fsanitize=address /Oy-")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")

    # Adjust this path to your LLVM install and version
    
    else()
    add_compile_options(-Wall -Wextra -Werror)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
    endif()
    
    add_executable(testapp
    src/imgbin.cpp
    src/imgbinio.cpp
    src/main.cpp
    src/imgbin_structs.cpp
    vendor/nlohmann/tinyxml2.cpp
    )
    
    target_include_directories(testapp PRIVATE
    src
    vendor
    )
    
if(MSVC)
    target_link_libraries(testapp
        "C:/Program Files/LLVM/lib/clang/20/lib/windows/clang_rt.asan_dynamic-x86_64.lib"
        "C:/Program Files/LLVM/lib/clang/20/lib/windows/clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
    )
endif()
