/*
    file.glnsh

    //header
    char "glnsh"

    // counts
    i32 num_imgs
    i32 num_atlas
    i32 num_shaders
    i32 num_subtex

    // per INFO
    i32 channels, x, y...
    i32 []{channels, x, y, n_subtex}
    i32 [] nbytes...

    // name sizes // impl
    u8[]   imgs_name_size
    u8[]   atlas_name_size
    u8[]   shader_name_size
    u8[][] subtex_name_size
    
    // subtex
    i32[][] subtex{x, y, width, height}

    // per DATA
    u8[]   img_data_i ...
    u8[]   atlas_data_i ...
    char[] shader_data_i ...

    // texture names // impl
    char[] img_names;
    char[] atlas_names;
    char[] shader_names;

    char[][] subtex_names

*/

/*
    file.hpp

    img_keys
    shader_keys
    atlas_keys
*/

writing 
we utilize the buggy ahh stl lib and
write out the keys as capitalized ENUMS in a .h  file

reading 
we use dynamically allocated blocks of mem only and
we access the assets using the enum class NAME : type


/*
    // metadata
    i32 num_atlas
    i32 num_shader
    i32 num_scenes

    i32[] atlas_file_path_size
    i32[] shader_file_path_size

    // data
    str[] atlas_file_path
    str[] shader_file_path

    // scene
    [] {
        char name[32];
        [] {
            vec3f    position;
            vec2f    scale;
            vec3f    rotation;
            char[32] name;
            i32      tex_index;
            i32      subtex_index;
            i32      type;
        }
    }
*/