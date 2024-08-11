#include <stdio.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

#define WIDTH  640
#define HEIGHT 400
// #define HEIGHT 640
// #define WIDTH  400
// #define WIDTH  640 / 2
// #define HEIGHT 400 / 2
// #define HEIGHT 640 / 2
// #define WIDTH  400 / 2

// #define WIDTH  512
// #define HEIGHT 288

static nu_allocator_t alloc;
static nu_platform_t  platform;
static nu_renderer_t  renderer;
static nu_logger_t    logger;

static nu_input_handle_t draw;
static nu_input_handle_t move_forward;
static nu_input_handle_t move_backward;
static nu_input_handle_t move_left;
static nu_input_handle_t move_right;
static nu_input_handle_t move_up;
static nu_input_handle_t move_down;
static nu_input_handle_t view_pitch_neg;
static nu_input_handle_t view_pitch_pos;
static nu_input_handle_t view_yaw_neg;
static nu_input_handle_t view_yaw_pos;
static nu_input_handle_t view_roll_neg;
static nu_input_handle_t view_roll_pos;
static nu_input_handle_t quit;

static nu_mesh_handle_t booster_mesh;
static nu_mat4_t        booster_transform;
static nu_mesh_handle_t llpm_mesh;
static nu_mat4_t        llpm_transform;
static nu_mesh_handle_t vulcain_mesh;
static nu_mat4_t        vulcain_transform;
static nu_model_t       temple_model;
static nu_model_t       ariane_model;

#define LOOP_TICK    0
#define LOOP_PHYSICS 1

static nu_error_t
load_mesh (const nuext_gltf_mesh_t *mesh)
{
    nu_mesh_info_t info;
    info.positions = mesh->positions;
    info.uvs       = mesh->uvs;
    info.normals   = mesh->normals;
    info.count     = mesh->count;

    if (NU_MATCH(mesh->name, "Cylinder.003"))
    {
        NU_INFO(
            &logger, "load mesh %s with %d vertices", mesh->name, mesh->count);
        nu_error_t error = nu_mesh_create(&renderer, &info, &vulcain_mesh);
        NU_ERROR_ASSERT(error);
    }
    else if (NU_MATCH(mesh->name, "Cylinder.002"))
    {
        NU_INFO(
            &logger, "load mesh %s with %d vertices", mesh->name, mesh->count);
        nu_error_t error = nu_mesh_create(&renderer, &info, &booster_mesh);
        NU_ERROR_ASSERT(error);
    }
    else if (NU_MATCH(mesh->name, "Cylinder"))
    {
        NU_INFO(
            &logger, "load mesh %s with %d vertices", mesh->name, mesh->count);
        nu_error_t error = nu_mesh_create(&renderer, &info, &llpm_mesh);
        NU_ERROR_ASSERT(error);
    }

    return NU_ERROR_NONE;
}
static nu_error_t
load_node (const nuext_gltf_node_t *node)
{
    if (NU_MATCH(node->name, "LLPM"))
    {
        llpm_transform = node->transform;
    }
    else if (NU_MATCH(node->name, "Booster"))
    {
        booster_transform = node->transform;
    }
    else if (NU_MATCH(node->name, "Vulcain"))
    {
        vulcain_transform = node->transform;
    }
    return NU_ERROR_NONE;
}

static nu_error_t
load_asset (const nuext_gltf_asset_t *asset, void *userdata)
{
    switch (asset->type)
    {
        case NUEXT_GLTF_ASSET_MESH:
            return load_mesh(&asset->mesh);
        case NUEXT_GLTF_ASSET_TEXTURE:
            break;
        case NUEXT_GLTF_ASSET_MATERIAL:
            break;
        case NUEXT_GLTF_ASSET_NODE:
            return load_node(&asset->node);
    }
    return NU_ERROR_NONE;
}

int
main (void)
{
    nu_error_t error;

    // Create allocator
    nuext_stdlib_allocator_init(&alloc);

    // Create platform
    {
        nu_platform_info_t info = nu_platform_info_default();
        info.width              = WIDTH;
        info.height             = HEIGHT;
        info.allocator          = alloc;
        error                   = nu_platform_init(&info, &platform);
        NU_ERROR_ASSERT(error);
    }

    // Create renderer
    {
        nu_renderer_info_t info = nu_renderer_info_default();
        info.allocator          = alloc;
        info.api                = NU_RENDERER_GL;
        error                   = nu_renderer_init(&platform, &info, &renderer);
        NU_ERROR_ASSERT(error);
    }

    // Create logger
    {
        nu_logger_info_t info = nu_logger_info_default();
        error                 = nu_logger_init(&info, &logger);
        NU_ERROR_ASSERT(error);
    }

    // Configure inputs
    {
        error &= nu_input_create(&platform, &draw);
        error &= nu_input_create(&platform, &quit);
        error &= nu_input_create(&platform, &move_forward);
        error &= nu_input_create(&platform, &move_backward);
        error &= nu_input_create(&platform, &move_left);
        error &= nu_input_create(&platform, &move_right);
        error &= nu_input_create(&platform, &move_up);
        error &= nu_input_create(&platform, &move_down);
        error &= nu_input_create(&platform, &view_pitch_neg);
        error &= nu_input_create(&platform, &view_pitch_pos);
        error &= nu_input_create(&platform, &view_yaw_neg);
        error &= nu_input_create(&platform, &view_yaw_pos);
        error &= nu_input_create(&platform, &view_roll_neg);
        error &= nu_input_create(&platform, &view_roll_pos);
        NU_ERROR_ASSERT(error);
    }

    // Bind inputs
    {
        error &= nuext_input_bind_button(&platform, quit, NUEXT_BUTTON_ESCAPE);
        error
            &= nuext_input_bind_button(&platform, move_forward, NUEXT_BUTTON_W);
        error &= nuext_input_bind_button(
            &platform, move_backward, NUEXT_BUTTON_S);
        error &= nuext_input_bind_button(&platform, move_left, NUEXT_BUTTON_A);
        error &= nuext_input_bind_button(&platform, move_right, NUEXT_BUTTON_D);
        error &= nuext_input_bind_button(&platform, move_up, NUEXT_BUTTON_X);
        error &= nuext_input_bind_button(&platform, move_down, NUEXT_BUTTON_Z);
        error &= nuext_input_bind_axis(
            &platform, view_pitch_neg, NUEXT_AXIS_MOUSE_MOTION_Y_NEG);
        error &= nuext_input_bind_axis(
            &platform, view_pitch_pos, NUEXT_AXIS_MOUSE_MOTION_Y_POS);
        error &= nuext_input_bind_axis(
            &platform, view_yaw_neg, NUEXT_AXIS_MOUSE_MOTION_X_NEG);
        error &= nuext_input_bind_axis(
            &platform, view_yaw_pos, NUEXT_AXIS_MOUSE_MOTION_X_POS);
        error &= nuext_input_bind_button_value(
            &platform, view_pitch_neg, NUEXT_BUTTON_K, 0.08);
        error &= nuext_input_bind_button_value(
            &platform, view_pitch_pos, NUEXT_BUTTON_J, 0.08);
        error &= nuext_input_bind_button_value(
            &platform, view_yaw_neg, NUEXT_BUTTON_H, 0.08);
        error &= nuext_input_bind_button_value(
            &platform, view_yaw_pos, NUEXT_BUTTON_L, 0.08);
        error &= nuext_input_bind_button_value(
            &platform, view_roll_neg, NUEXT_BUTTON_E, 0.08);
        error &= nuext_input_bind_button_value(
            &platform, view_roll_pos, NUEXT_BUTTON_Q, 0.08);
        NU_ERROR_ASSERT(error);
    }

    // Create depth buffer
    nu_texture_handle_t depth_buffer;
    {
        nu_texture_info_t info;
        info.usage  = NU_TEXTURE_USAGE_TARGET;
        info.format = NU_TEXTURE_FORMAT_DEPTH;
        info.size   = nu_uvec2(WIDTH, HEIGHT);
        error       = nu_texture_create(&renderer, &info, &depth_buffer);
        NU_ERROR_ASSERT(error);
    }

    // Create cube
    nu_mesh_handle_t cube_mesh;
    {
        nu_vec3_t cube_positions[NU_CUBE_MESH_VERTEX_COUNT];
        nu_vec2_t cube_uvs[NU_CUBE_MESH_VERTEX_COUNT];
        nu_vec3_t cube_normals[NU_CUBE_MESH_VERTEX_COUNT];
        nu_generate_cube_mesh(1.0f, cube_positions, cube_uvs, cube_normals);
        nu_mesh_info_t info = { .positions = cube_positions,
                                .uvs       = cube_uvs,
                                .normals   = cube_normals,
                                .count     = NU_CUBE_MESH_VERTEX_COUNT };
        error               = nu_mesh_create(&renderer, &info, &cube_mesh);
        NU_ERROR_ASSERT(error);
    }

    // Load resources
    nu_texture_handle_t texture;
    nu_texture_handle_t texture_white;
    {
        nu_image_t image;
        error = nuext_load_image(
            "../../../assets/brick_building_front_lowres.png", &alloc, &image);

        nu_texture_info_t info;
        info.size   = image.size;
        info.usage  = NU_TEXTURE_USAGE_SAMPLE;
        info.format = NU_TEXTURE_FORMAT_COLOR;
        info.colors = image.data;
        error       = nu_texture_create(&renderer, &info, &texture);
        NU_ERROR_ASSERT(error);
        nu_image_free(&image, &alloc);

        nu_texture_create_color(&renderer, NU_COLOR_WHITE, &texture_white);
    }

    // Load models
    {
        error = nuext_load_gltf("../../../assets/ariane6.glb",
                                &logger,
                                &alloc,
                                load_asset,
                                NU_NULL);
        NU_ERROR_ASSERT(error);
    }

    // Create material
    nu_material_handle_t material;
    nu_material_handle_t material_white;
    {
        nu_material_info_t info = nu_material_info_default();
        info.texture0           = &texture;
        error = nu_material_create(&renderer, &info, &material);
        NU_ERROR_ASSERT(error);

        info          = nu_material_info_default();
        info.texture0 = &texture_white;
        error         = nu_material_create(&renderer, &info, &material_white);
        NU_ERROR_ASSERT(error);
    }

    // Load temple
    {
        error = nuext_model_from_gltf("../../../assets/temple_of_time.glb",
                                      &renderer,
                                      &logger,
                                      &alloc,
                                      &temple_model);
        NU_ERROR_ASSERT(error);
    }

    // Load ariane
    {
        error = nuext_model_from_gltf("../../../assets/ariane6.glb",
                                      &renderer,
                                      &logger,
                                      &alloc,
                                      &ariane_model);
        NU_ERROR_ASSERT(error);
    }

    // Load cubemap
    nu_cubemap_handle_t skybox;
    {
        const nu_char_t *filenames[] = {
            "../../../assets/skyboxes/vz_clear_ocean_up.png",
            "../../../assets/skyboxes/vz_clear_ocean_down.png",
            "../../../assets/skyboxes/vz_clear_ocean_left.png",
            "../../../assets/skyboxes/vz_clear_ocean_right.png",
            "../../../assets/skyboxes/vz_clear_ocean_front.png",
            "../../../assets/skyboxes/vz_clear_ocean_back.png",
        };
        nu_image_t images[6];
        for (nu_size_t i = 0; i < 6; ++i)
        {
            error = nuext_load_image(filenames[i], &alloc, &images[i]);
            NU_ERROR_ASSERT(error);
        }
        nu_cubemap_info_t info;
        info.size        = images->size.x;
        info.usage       = NU_TEXTURE_USAGE_SAMPLE;
        info.colors_posx = images[3].data;
        info.colors_negx = images[2].data;
        info.colors_posy = images[0].data;
        info.colors_negy = images[1].data;
        info.colors_posz = images[4].data;
        info.colors_negz = images[5].data;
        error            = nu_cubemap_create(&renderer, &info, &skybox);
        NU_ERROR_ASSERT(error);
        for (nu_size_t i = 0; i < 6; ++i)
        {
            nu_image_free(&images[i], &alloc);
        }
    }

    // Create camera
    nu_camera_handle_t camera;
    nu_camera_info_t   camera_info = nu_camera_info_default();
    {
        error = nu_camera_create(&renderer, &camera_info, &camera);
        NU_ERROR_ASSERT(error);
    }

    // Create camera controller
    nu_camera_controller_t controller;
    {
        nu_camera_controller_init(&controller);
    }

    // Create renderpasses
    nu_renderpass_handle_t main_pass;
    nu_renderpass_handle_t skybox_pass;
    {
        nu_renderpass_info_t info;

        info.type = NU_RENDERPASS_FLAT;
        error     = nu_renderpass_create(&renderer, &info, &main_pass);
        NU_ERROR_ASSERT(error);

        info.type = NU_RENDERPASS_SKYBOX;
        error     = nu_renderpass_create(&renderer, &info, &skybox_pass);
        NU_ERROR_ASSERT(error);
    }

    // Main loop
    nu_bool_t drawing = NU_FALSE;
    nu_bool_t running = NU_TRUE;

    nu_fixed_loop_t loops[2];
    loops[0] = nu_fixed_loop(LOOP_TICK, 1.0 / 60.0 * 1000.0);
    loops[1] = nu_fixed_loop(LOOP_PHYSICS, 1.0 / 20.0 * 1000.0);

    nu_timer_t timer;
    nu_timer_reset(&timer);
    float delta = 0.0f;
    float time  = 0;

    while (!nu_exit_requested(&platform) && running)
    {
        nu_update_fixed_loops(loops, 2, delta);
        nu_u32_t id;
        while (nu_next_fixed_loop(loops, 2, &id))
        {
            switch (id)
            {
                case LOOP_TICK:
                    break;
                case LOOP_PHYSICS:
                    break;
                default:
                    break;
            }
        }

        delta = nu_timer_elapsed(&timer);
        nu_timer_reset(&timer);
        time += delta;

        // Poll events
        nu_poll_events(&platform);

        // Check exit
        if (nu_input_just_pressed(&platform, quit))
        {
            running = NU_FALSE;
        }

        // Update camera controller
        controller.view_pitch_neg = nu_input_value(&platform, view_pitch_neg);
        controller.view_pitch_pos = nu_input_value(&platform, view_pitch_pos);
        controller.view_yaw_neg   = nu_input_value(&platform, view_yaw_neg);
        controller.view_yaw_pos   = nu_input_value(&platform, view_yaw_pos);
        controller.view_roll_neg  = nu_input_value(&platform, view_roll_neg);
        controller.view_roll_pos  = nu_input_value(&platform, view_roll_pos);
        controller.move_forward   = nu_input_value(&platform, move_forward);
        controller.move_backward  = nu_input_value(&platform, move_backward);
        controller.move_left      = nu_input_value(&platform, move_left);
        controller.move_right     = nu_input_value(&platform, move_right);
        controller.move_up        = nu_input_value(&platform, move_up);
        controller.move_down      = nu_input_value(&platform, move_down);
        nu_camera_controller_update(&controller, delta, &camera_info);
        nu_camera_update(&renderer, camera, &camera_info);

        // Update material
        // nu_material_info_t minfo;
        // minfo.texture0     = &texture;
        // minfo.texture1     = NU_NULL;
        // minfo.uv_transform = nu_mat3_scale(nu_sin(time / 1000), 1);
        // nu_material_update(&renderer, &material, &minfo);

        // Render loop
        for (int i = 0; i < 40; ++i)
        {
            nu_mat4_t model = nu_mat4_translate(
                nu_sin(time / 1000 + i), nu_cos(time / 1000 + i), i * 1.05);
            nu_draw(&renderer, main_pass, cube_mesh, material, model);
        }

        // Render custom mesh
        {
            nu_mat4_t model = nu_mat4_scale(0.2, 0.2, 0.2);
            nu_model_draw(&renderer, main_pass, &ariane_model, model);

            model = nu_mat4_translate(10, 0, 0);
            nu_model_draw(&renderer, main_pass, &temple_model, model);
        }

        // Submit renderpass
        nu_renderpass_submit_t submit;

        nu_texture_handle_t surface_tex
            = nu_surface_color_target(&platform, &renderer);
        nu_color_t clear_color = NU_COLOR_BLUE_SKY;

        submit.reset = NU_TRUE;

        submit.flat.camera       = camera;
        submit.flat.color_target = &surface_tex;
        submit.flat.depth_target = &depth_buffer;
        submit.flat.clear_color  = &clear_color;
        nu_renderpass_submit(&renderer, main_pass, &submit);

        submit.skybox.camera       = camera;
        submit.skybox.color_target = &surface_tex;
        submit.skybox.depth_target = &depth_buffer;
        submit.skybox.cubemap      = skybox;
        submit.skybox.rotation     = nu_quat_identity();
        nu_renderpass_submit(&renderer, skybox_pass, &submit);

        // Render
        nu_render(&platform, &renderer);

        // Refresh surface
        nu_swap_buffers(&platform);
    }

    // Free cube
    error = nu_mesh_delete(&renderer, cube_mesh);
    NU_ERROR_ASSERT(error);
    error = nu_renderpass_delete(&renderer, main_pass);
    NU_ERROR_ASSERT(error);

    nu_renderer_terminate(&renderer);
    nu_platform_terminate(&platform);

    return 0;
}
