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

static nu_allocator_t     alloc;
static nu_platform_t      platform;
static nu_renderer_t      renderer;
static nu_asset_manager_t asset_manager;
static nu_logger_t        logger;

static nu_input_handle_t draw;
static nu_input_handle_t main_button;
static nu_input_handle_t quit;
static nu_input_handle_t cursor_x;
static nu_input_handle_t cursor_y;

static nu_model_t temple_model;
static nu_model_t ariane_model;

#define LOOP_TICK    0
#define LOOP_PHYSICS 1

int
main (void)
{
    nu_error_t error;

    // Create allocator
    nuext_allocator_init_stdlib(&alloc);

    // Create platform
    {
        nu_platform_info_t info = NU_PLATFORM_INFO_DEFAULT;
        info.width              = WIDTH;
        info.height             = HEIGHT;
        error                   = nu_platform_init(&info, &platform);
        NU_ERROR_ASSERT(error);
    }

    // Create renderer
    {
        nu_renderer_info_t info = NU_RENDERER_INFO_DEFAULT;
        info.allocator          = alloc;
        info.api                = NU_RENDERER_GL;
        error                   = nu_renderer_init(&platform, &info, &renderer);
        NU_ERROR_ASSERT(error);
    }

    // Create logger
    error = nu_logger_init(&NU_LOGGER_INFO_DEFAULT, &logger);
    NU_ERROR_ASSERT(error);

    // Create camera controller
    nu_camera_controller_t controller;
    {
        nu_camera_controller_init(&controller);
    }

    // Configure inputs
    {
        error &= nu_input_create(&platform, &draw);
        error &= nu_input_create(&platform, &main_button);
        error &= nu_input_create(&platform, &quit);
        error &= nu_input_create(&platform, &cursor_x);
        error &= nu_input_create(&platform, &cursor_y);
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
        error &= nuext_input_bind_button(
            &platform, main_button, NUEXT_BUTTON_MOUSE_1);
        error &= nuext_input_bind_button(&platform, quit, NUEXT_BUTTON_ESCAPE);
        error &= nuext_input_bind_axis(&platform, cursor_x, NUEXT_AXIS_MOUSE_X);
        error &= nuext_input_bind_axis(&platform, cursor_y, NUEXT_AXIS_MOUSE_Y);
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
        error = nuext_load_image_filename(
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

        error = nuext_load_image_filename(
            "../../../assets/GUI.png", &alloc, &image);
        NU_ERROR_ASSERT(error);
        info.size   = image.size;
        info.usage  = NU_TEXTURE_USAGE_SAMPLE;
        info.format = NU_TEXTURE_FORMAT_COLOR;
        info.colors = image.data;
        error       = nu_texture_create(&renderer, &info, &texture_gui);
        NU_ERROR_ASSERT(error);
        nu_image_free(&image, &alloc);
    }

    // Create material
    nu_material_handle_t material;
    nu_material_handle_t material_white;
    nu_material_handle_t material_gui;
    nu_material_handle_t material_gui_repeat;
    {
        nu_material_info_t info = NU_MATERIAL_INFO_DEFAULT_MESH;
        info.mesh.color0        = &texture;
        error = nu_material_create(&renderer, &info, &material);
        NU_ERROR_ASSERT(error);

        info             = NU_MATERIAL_INFO_DEFAULT_MESH;
        info.mesh.color0 = &texture_white;
        error = nu_material_create(&renderer, &info, &material_white);
        NU_ERROR_ASSERT(error);

        info               = NU_MATERIAL_INFO_DEFAULT_CANVAS;
        info.canvas.color0 = &texture_gui;
        error = nu_material_create(&renderer, &info, &material_gui);
        NU_ERROR_ASSERT(error);

        info                  = NU_MATERIAL_INFO_DEFAULT_CANVAS;
        info.canvas.color0    = &texture_gui;
        info.canvas.wrap_mode = NU_TEXTURE_WRAP_REPEAT;
        error = nu_material_create(&renderer, &info, &material_gui_repeat);
        NU_ERROR_ASSERT(error);
    }

    // Load temple
    {
        nu_gltf_loader_t loader;
        nu_gltf_loader_init(&alloc, &logger, &loader);

        error = nuext_gltf_load_model_filename(
            &loader,
            "../../../assets/temple_of_time.glb",
            &alloc,
            &renderer,
            &temple_model);
        NU_ERROR_ASSERT(error);

        error = nuext_gltf_load_model_filename(&loader,
                                               "../../../assets/ariane6.glb",
                                               &alloc,
                                               &renderer,
                                               &ariane_model);
        NU_ERROR_ASSERT(error);

        nu_gltf_loader_free(&loader);
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
            error = nuext_load_image_filename(filenames[i], &alloc, &images[i]);
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
    error = nu_font_init_default(&renderer, &alloc, &font);
    NU_ERROR_ASSERT(error);

    // Create camera
    nu_camera_handle_t camera;
    nu_camera_info_t   camera_info = NU_CAMERA_INFO_DEFAULT;
    error = nu_camera_create(&renderer, &camera_info, &camera);
    NU_ERROR_ASSERT(error);

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

    // Create UI
    nu_ui_t ui;
    error = nu_ui_init(&renderer, &alloc, &ui);
    NU_ERROR_ASSERT(error);

    nu_ui_style_t button_style;
    button_style.type                     = NU_UI_BUTTON;
    button_style.button.pressed.material  = material_gui_repeat;
    button_style.button.pressed.extent    = nu_rect(113, 97, 30, 14);
    button_style.button.pressed.margin    = (nu_ui_margin_t) { 3, 5, 3, 3 };
    button_style.button.released.material = material_gui_repeat;
    button_style.button.released.extent   = nu_rect(113, 81, 30, 14);
    button_style.button.released.margin   = (nu_ui_margin_t) { 3, 5, 3, 3 };
    button_style.button.hovered.material  = material_gui_repeat;
    button_style.button.hovered.extent    = nu_rect(113, 113, 30, 14);
    button_style.button.hovered.margin    = (nu_ui_margin_t) { 3, 5, 3, 3 };
    nu_ui_push_style(&ui, &button_style);
    nu_ui_style_t checkbox_style;
    checkbox_style.type                      = NU_UI_CHECKBOX;
    checkbox_style.checkbox.checked.material = material_gui_repeat;
    checkbox_style.checkbox.checked.extent   = nu_rect(97, 257, 14, 14);
    checkbox_style.checkbox.checked.margin   = (nu_ui_margin_t) { 3, 3, 2, 2 };
    checkbox_style.checkbox.unchecked.material = material_gui_repeat;
    checkbox_style.checkbox.unchecked.extent   = nu_rect(81, 257, 14, 14);
    checkbox_style.checkbox.unchecked.margin = (nu_ui_margin_t) { 2, 4, 2, 2 };
    nu_ui_push_style(&ui, &checkbox_style);
    nu_ui_style_t cursor_style;
    cursor_style.type                  = NU_UI_CURSOR;
    cursor_style.cursor.image.material = material_gui_repeat;
    cursor_style.cursor.image.extent   = nu_rect(98, 38, 3, 3);
    cursor_style.cursor.image.margin   = (nu_ui_margin_t) { 0, 0, 0, 0 };
    nu_ui_push_style(&ui, &cursor_style);

    // Main loop
    nu_bool_t drawing = NU_FALSE;
    nu_bool_t running = NU_TRUE;

    nu_fixed_loop_t loops[2];
    loops[0] = nu_fixed_loop(LOOP_TICK, 1.0 / 60.0 * 1000.0);
    loops[1] = nu_fixed_loop(LOOP_PHYSICS, 1.0 / 20.0 * 1000.0);

    nu_timer_t timer;
    nu_timer_reset(&timer);
    float     delta      = 0.0f;
    float     time       = 0;
    nu_bool_t bool_state = NU_TRUE;

    while (!nu_platform_exit_requested(&platform) && running)
    {
        nu_fixed_loop_update(loops, 2, delta);
        nu_u32_t id;
        while (nu_fixed_loop_next(loops, 2, &id))
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
        nu_platform_poll_events(&platform);

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
                nu_sin(time / 100 + i) * 4, nu_cos(time / 100 + i) * 10, i * 5);
            nu_draw_mesh(&renderer, main_pass, material, cube_mesh, model);
        }

        // Render custom mesh
        {
            nu_mat4_t transform = nu_mat4_scale(0.3, 0.3, 0.3);
            nu_draw_model(&renderer, main_pass, &ariane_model, transform);

            transform = nu_mat4_translate(10, 0, 0);
            nu_draw_model(&renderer, main_pass, &temple_model, transform);
            transform = nu_mat4_translate(20, 0, 0);
            nu_draw_model(&renderer, main_pass, &temple_model, transform);
            transform = nu_mat4_translate(30, 0, 0);
            nu_draw_model(&renderer, main_pass, &temple_model, transform);
            transform = nu_mat4_translate(40, 0, 0);
            nu_draw_model(&renderer, main_pass, &temple_model, transform);
        }

        // Draw GUI
        {
            for (nu_size_t i = 0; i < 30; ++i)
            {
                int x = i % 20;
                int y = i / 20;
                nu_draw_blit(&renderer,
                             gui_pass,
                             material_gui,
                             nu_rect(10 + x * 15, 30 + y * 15, 14, 14),
                             nu_rect(81, 257, 14, 14));
            }

            nu_draw_blit(&renderer,
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
            nu_draw_text(&renderer,
                         gui_pass,
                         s,
                         nu_strlen(s),
                         nu_ivec2(10, HEIGHT / 2),
                         &font);
        }

        // GUI
        {
            ui.controllers[0].active = NU_TRUE;
            ui.controllers[0].cursor
                = nuext_platform_cursor(&platform, cursor_x, cursor_y);
            ui.controllers[0].main_pressed
                = nu_input_pressed(&platform, main_button);
            nu_ui_begin(&ui, &renderer);
            if (nu_ui_button(&ui, nu_rect(300, 100, 60, 20)))
            {
                NU_INFO(&logger, "button pressed !");
            }
            if (nu_ui_checkbox(&ui, nu_rect(300, 300, 14, 14), &bool_state))
            {
                NU_INFO(&logger, "checkbox changed %d", bool_state);
            }
            nu_ui_end(&ui);
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
            nu_draw_text(
                &renderer, gui_pass, string, n, nu_ivec2(10, 10), &font);
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

        submit.canvas.color_target = &surface_tex;
        nu_ui_submit_renderpasses(&ui, &renderer, &submit);

        // Render
        nu_renderer_render(&platform, &renderer);

        // Refresh surface
        nu_platform_swap_buffers(&platform);
    }

    // Free cube
    error = nu_mesh_delete(&renderer, cube_mesh);
    NU_ERROR_ASSERT(error);
    error = nu_renderpass_delete(&renderer, main_pass);
    NU_ERROR_ASSERT(error);

    nu_renderer_free(&renderer);
    nu_platform_free(&platform);

    return 0;
}
