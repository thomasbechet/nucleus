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

    // Create camera controller
    nu_camera_controller_t controller;
    {
        nu_camera_controller_init(&controller);
    }

    // Configure inputs
    {
        error &= nu_input_create(&platform, &draw);
        error &= nu_input_create(&platform, &quit);
        error &= nu_input_create(&platform, &controller.move_forward);
        error &= nu_input_create(&platform, &controller.move_backward);
        error &= nu_input_create(&platform, &controller.move_left);
        error &= nu_input_create(&platform, &controller.move_right);
        error &= nu_input_create(&platform, &controller.move_up);
        error &= nu_input_create(&platform, &controller.move_down);
        error &= nu_input_create(&platform, &controller.view_pitch_neg);
        error &= nu_input_create(&platform, &controller.view_pitch_pos);
        error &= nu_input_create(&platform, &controller.view_yaw_neg);
        error &= nu_input_create(&platform, &controller.view_yaw_pos);
        error &= nu_input_create(&platform, &controller.view_roll_neg);
        error &= nu_input_create(&platform, &controller.view_roll_pos);
        error &= nu_input_create(&platform, &controller.switch_mode);
        NU_ERROR_ASSERT(error);
    }

    // Bind inputs
    {
        error &= nuext_input_bind_button(&platform, quit, NUEXT_BUTTON_ESCAPE);
        error &= nuext_input_bind_button(
            &platform, controller.move_forward, NUEXT_BUTTON_W);
        error &= nuext_input_bind_button(
            &platform, controller.move_backward, NUEXT_BUTTON_S);
        error &= nuext_input_bind_button(
            &platform, controller.move_left, NUEXT_BUTTON_A);
        error &= nuext_input_bind_button(
            &platform, controller.move_right, NUEXT_BUTTON_D);
        error &= nuext_input_bind_button(
            &platform, controller.move_up, NUEXT_BUTTON_X);
        error &= nuext_input_bind_button(
            &platform, controller.move_down, NUEXT_BUTTON_Z);
        error &= nuext_input_bind_axis(&platform,
                                       controller.view_pitch_neg,
                                       NUEXT_AXIS_MOUSE_MOTION_Y_NEG);
        error &= nuext_input_bind_axis(&platform,
                                       controller.view_pitch_pos,
                                       NUEXT_AXIS_MOUSE_MOTION_Y_POS);
        error &= nuext_input_bind_axis(
            &platform, controller.view_yaw_neg, NUEXT_AXIS_MOUSE_MOTION_X_NEG);
        error &= nuext_input_bind_axis(
            &platform, controller.view_yaw_pos, NUEXT_AXIS_MOUSE_MOTION_X_POS);
        error &= nuext_input_bind_button_value(
            &platform, controller.view_pitch_neg, NUEXT_BUTTON_K, 0.08);
        error &= nuext_input_bind_button_value(
            &platform, controller.view_pitch_pos, NUEXT_BUTTON_J, 0.08);
        error &= nuext_input_bind_button_value(
            &platform, controller.view_yaw_neg, NUEXT_BUTTON_H, 0.08);
        error &= nuext_input_bind_button_value(
            &platform, controller.view_yaw_pos, NUEXT_BUTTON_L, 0.08);
        error &= nuext_input_bind_button_value(
            &platform, controller.view_roll_neg, NUEXT_BUTTON_E, 0.08);
        error &= nuext_input_bind_button_value(
            &platform, controller.view_roll_pos, NUEXT_BUTTON_Q, 0.08);
        error &= nuext_input_bind_button(
            &platform, controller.switch_mode, NUEXT_BUTTON_C);
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
    nu_texture_handle_t texture_gui;
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

        error = nuext_load_image("../../../assets/GUI.png", &alloc, &image);
        NU_ERROR_ASSERT(error);
        info.size   = image.size;
        info.usage  = NU_TEXTURE_USAGE_SAMPLE;
        info.format = NU_TEXTURE_FORMAT_COLOR;
        info.colors = image.data;
        error       = nu_texture_create(&renderer, &info, &texture_gui);
        NU_ERROR_ASSERT(error);
        nu_image_free(&image, &alloc);
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
    nu_material_handle_t material_gui;
    nu_material_handle_t material_gui_repeat;
    {
        nu_material_info_t info = nu_material_info_default(NU_MATERIAL_MESH);
        info.mesh.color0        = &texture;
        error = nu_material_create(&renderer, &info, &material);
        NU_ERROR_ASSERT(error);

        info             = nu_material_info_default(NU_MATERIAL_MESH);
        info.mesh.color0 = &texture_white;
        error = nu_material_create(&renderer, &info, &material_white);
        NU_ERROR_ASSERT(error);

        info               = nu_material_info_default(NU_MATERIAL_CANVAS);
        info.canvas.color0 = &texture_gui;
        error = nu_material_create(&renderer, &info, &material_gui);
        NU_ERROR_ASSERT(error);

        info                  = nu_material_info_default(NU_MATERIAL_CANVAS);
        info.canvas.color0    = &texture_gui;
        info.canvas.wrap_mode = NU_TEXTURE_WRAP_REPEAT;
        error = nu_material_create(&renderer, &info, &material_gui_repeat);
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

    // Create font
    nu_font_t font;
    error = nu_font_default(&renderer, &alloc, &font);
    NU_ERROR_ASSERT(error);

    // Create camera
    nu_camera_handle_t camera;
    nu_camera_info_t   camera_info = nu_camera_info_default();
    {
        error = nu_camera_create(&renderer, &camera_info, &camera);
        NU_ERROR_ASSERT(error);
    }

    // Create renderpasses
    nu_renderpass_handle_t main_pass;
    nu_renderpass_handle_t skybox_pass;
    nu_renderpass_handle_t gui_pass;
    {
        nu_renderpass_info_t info;

        info.type               = NU_RENDERPASS_FLAT;
        info.reset_after_submit = NU_TRUE;
        error = nu_renderpass_create(&renderer, &info, &main_pass);
        NU_ERROR_ASSERT(error);

        info.type               = NU_RENDERPASS_SKYBOX;
        info.reset_after_submit = NU_TRUE;
        error = nu_renderpass_create(&renderer, &info, &skybox_pass);
        NU_ERROR_ASSERT(error);

        info.type               = NU_RENDERPASS_CANVAS;
        info.reset_after_submit = NU_TRUE;
        error = nu_renderpass_create(&renderer, &info, &gui_pass);
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
        nu_camera_controller_update(
            &controller, &platform, delta, &camera_info);
        nu_camera_update(&renderer, camera, &camera_info);

        // Render loop
        for (int i = 0; i < 40; ++i)
        {
            nu_mat4_t model = nu_mat4_translate(
                nu_sin(time / 1000 + i) * 30, nu_cos(time / 1000 + i) * 10, i * 5);
            nu_draw(&renderer, main_pass, material, cube_mesh, model);
        }

        // Render custom mesh
        {
            nu_mat4_t model = nu_mat4_scale(0.2, 0.2, 0.2);
            nu_model_draw(&renderer, main_pass, &ariane_model, model);

            model = nu_mat4_translate(10, 0, 0);
            nu_model_draw(&renderer, main_pass, &temple_model, model);
        }

        // Draw GUI
        {
            for (nu_size_t i = 0; i < 30; ++i)
            {
                int x = i % 20;
                int y = i / 20;
                nu_blit(&renderer,
                        gui_pass,
                        material_gui,
                        nu_rect(10 + x * 15, 30 + y * 15, 14, 14),
                        nu_rect(81, 257, 14, 14));
            }

            nu_blit(&renderer,
                    gui_pass,
                    material_gui_repeat,
                    nu_rect(400, 10, 105, 105),
                    nu_rect(81, 257, 14, 14));

            const nu_char_t *s
                = "Lorem Ipsum is simply dummy text of the printing and\n"
                  "typesetting industry. Lorem Ipsum has been the industry's\n"
                  "standard dummy text ever since the 1500s, when an unknown\n"
                  "printer took a galley of type and scrambled it to make a\n"
                  "type specimen book. It has survived not only five\n"
                  "centuries, but also the leap into electronic typesetting,\n"
                  "remaining essentially unchanged. It was popularised in the\n"
                  "1960s with the release of Letraset sheets containing Lorem\n"
                  "Ipsum passages, and more recently with desktop publishing\n"
                  "software like Aldus PageMaker including versions of Lorem\n"
                  "Ipsum.";
            nu_print(&renderer,
                     gui_pass,
                     s,
                     nu_strlen(s),
                     nu_ivec2(10, HEIGHT / 2),
                     &font);
        }

        // Print FPS
        {
#define AVG_SIZE 10
            static nu_u32_t avg[AVG_SIZE] = { 0 };
            static nu_u32_t head          = 0;

            float frame_fps = nu_floor((1.0 / delta) * 1000.0);
            avg[head]       = frame_fps;
            head            = (head + 1) % AVG_SIZE;
            float frame_avg = 0;
            for (nu_size_t i = 0; i < AVG_SIZE; ++i)
            {
                frame_avg += avg[i];
            }
            frame_avg /= AVG_SIZE;

            nu_char_t string[256];
            nu_size_t n = snprintf(string, 256, "FPS: %d", (nu_u32_t)frame_avg);
            nu_print(&renderer, gui_pass, string, n, nu_ivec2(10, 10), &font);
        }

        // Submit renderpass
        nu_renderpass_submit_t submit;

        nu_texture_handle_t surface_tex
            = nu_surface_color_target(&platform, &renderer);
        nu_color_t clear_color = NU_COLOR_BLUE_SKY;

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

        submit.canvas.color_target = &surface_tex;
        nu_renderpass_submit(&renderer, gui_pass, &submit);

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
