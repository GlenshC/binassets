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
    i32 channels, x, y, n_subtex ...
    i32 nbytes...
    
    // subtex
    i32[][] subtex{x, y, width, height}

    // per DATA
    u8[] img_data_i ...
    u8[] atlas_data_i ...
    char[] shader_data_i ...


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