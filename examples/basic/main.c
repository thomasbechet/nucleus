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

static nu_allocator_t     allocator;
static nu_platform_t      platform;
static nu_renderer_t      renderer;
static nu_asset_manager_t assets;
static nu_logger_t        logger;

static nu_input_t draw;
static nu_input_t main_button;
static nu_input_t quit;
static nu_input_t cursor_x;
static nu_input_t cursor_y;

static nu_model_t temple_model;
static nu_model_t ariane_model;

#define LOOP_TICK    0
#define LOOP_PHYSICS 1

int
main (void)
{
    // Create allocator
    nuext_allocator_create_stdlib(&allocator);

    // Create logger
    nu_logger_info_t logger_info;
    logger_info.allocator = allocator;
    logger_info.level     = NU_LOG_DEBUG;
    nu_logger_create(&logger_info, &logger);

    // Create platform
    nu_platform_info_t platform_info;
    platform_info.width     = WIDTH;
    platform_info.height    = HEIGHT;
    platform_info.allocator = allocator;
    platform_info.logger    = logger;
    nu_platform_create(&platform_info, &platform);

    // Create renderer
    nu_renderer_info_t renderer_info;
    renderer_info.allocator = allocator;
    renderer_info.api       = NU_RENDERER_GL;
    renderer_info.platform  = platform;
    renderer_info.logger    = logger;
    nu_renderer_create(&renderer_info, &renderer);

    // Create asset manager
    nu_asset_manager_create(allocator, &assets);

    // Create camera controller
    nu_camera_controller_t controller;
    nu_camera_controller_init(&controller);

    // Configure inputs
    nu_input_create(platform, &draw);
    nu_input_create(platform, &main_button);
    nu_input_create(platform, &quit);
    nu_input_create(platform, &cursor_x);
    nu_input_create(platform, &cursor_y);
    nu_input_create(platform, &controller.move_forward);
    nu_input_create(platform, &controller.move_backward);
    nu_input_create(platform, &controller.move_left);
    nu_input_create(platform, &controller.move_right);
    nu_input_create(platform, &controller.move_up);
    nu_input_create(platform, &controller.move_down);
    nu_input_create(platform, &controller.view_pitch_neg);
    nu_input_create(platform, &controller.view_pitch_pos);
    nu_input_create(platform, &controller.view_yaw_neg);
    nu_input_create(platform, &controller.view_yaw_pos);
    nu_input_create(platform, &controller.view_roll_neg);
    nu_input_create(platform, &controller.view_roll_pos);
    nu_input_create(platform, &controller.switch_mode);

    // Bind inputs

    nuext_input_bind_button(platform, main_button, NUEXT_BUTTON_MOUSE_1);
    nuext_input_bind_button(platform, quit, NUEXT_BUTTON_ESCAPE);
    nuext_input_bind_axis(platform, cursor_x, NUEXT_AXIS_MOUSE_X);
    nuext_input_bind_axis(platform, cursor_y, NUEXT_AXIS_MOUSE_Y);
    nuext_input_bind_button(platform, controller.move_forward, NUEXT_BUTTON_W);
    nuext_input_bind_button(platform, controller.move_backward, NUEXT_BUTTON_S);
    nuext_input_bind_button(platform, controller.move_left, NUEXT_BUTTON_A);
    nuext_input_bind_button(platform, controller.move_right, NUEXT_BUTTON_D);
    nuext_input_bind_button(platform, controller.move_up, NUEXT_BUTTON_X);
    nuext_input_bind_button(platform, controller.move_down, NUEXT_BUTTON_Z);
    nuext_input_bind_axis(
        platform, controller.view_pitch_neg, NUEXT_AXIS_MOUSE_MOTION_Y_NEG);
    nuext_input_bind_axis(
        platform, controller.view_pitch_pos, NUEXT_AXIS_MOUSE_MOTION_Y_POS);
    nuext_input_bind_axis(
        platform, controller.view_yaw_neg, NUEXT_AXIS_MOUSE_MOTION_X_NEG);
    nuext_input_bind_axis(
        platform, controller.view_yaw_pos, NUEXT_AXIS_MOUSE_MOTION_X_POS);
    nuext_input_bind_button_value(
        platform, controller.view_pitch_neg, NUEXT_BUTTON_K, 0.08);
    nuext_input_bind_button_value(
        platform, controller.view_pitch_pos, NUEXT_BUTTON_J, 0.08);
    nuext_input_bind_button_value(
        platform, controller.view_yaw_neg, NUEXT_BUTTON_H, 0.08);
    nuext_input_bind_button_value(
        platform, controller.view_yaw_pos, NUEXT_BUTTON_L, 0.08);
    nuext_input_bind_button_value(
        platform, controller.view_roll_neg, NUEXT_BUTTON_E, 0.08);
    nuext_input_bind_button_value(
        platform, controller.view_roll_pos, NUEXT_BUTTON_Q, 0.08);
    nuext_input_bind_button(platform, controller.switch_mode, NUEXT_BUTTON_C);

    // Create depth buffer
    nu_texture_t      depth_buffer;
    nu_texture_info_t depth_buffer_info;
    depth_buffer_info.usage  = NU_TEXTURE_USAGE_TARGET,
    depth_buffer_info.format = NU_TEXTURE_FORMAT_DEPTH,
    depth_buffer_info.size   = nu_uvec2(WIDTH, HEIGHT),
    nu_texture_create(renderer, &depth_buffer_info, &depth_buffer);

    // Create cube
    nu_mesh_t cube_mesh;
    nu_vec3_t cube_positions[NU_CUBE_MESH_VERTEX_COUNT];
    nu_vec2_t cube_uvs[NU_CUBE_MESH_VERTEX_COUNT];
    nu_vec3_t cube_normals[NU_CUBE_MESH_VERTEX_COUNT];
    nu_generate_cube_mesh(1.0f, cube_positions, cube_uvs, cube_normals);
    nu_mesh_info_t cube_mesh_info;
    cube_mesh_info.positions = cube_positions, cube_mesh_info.uvs = cube_uvs,
    cube_mesh_info.normals = cube_normals,
    cube_mesh_info.count   = NU_CUBE_MESH_VERTEX_COUNT;
    nu_mesh_create(renderer, &cube_mesh_info, &cube_mesh);

    // Load resources
    nu_texture_t texture;
    nu_texture_t texture_white;
    nu_texture_t texture_gui;
    {
        nu_image_t image;
        nuext_image_load_filename(
            "../../../assets/brick_building_front_lowres.png",
            allocator,
            &image);
        nu_texture_create_image(renderer, image, &texture);
        nu_image_delete(image);

        nu_texture_create_color(renderer, NU_COLOR_WHITE, &texture_white);

        nuext_image_load_filename("../../../assets/GUI.png", allocator, &image);
        nu_texture_create_image(renderer, image, &texture_gui);
        nu_image_delete(image);
    }

    // Create material
    nu_material_t material;
    nu_material_t material_white;
    nu_material_t material_gui;
    nu_material_t material_gui_repeat;
    {
        nu_material_info_t info = nu_material_info_default(NU_MATERIAL_MESH);
        info.mesh.color0        = &texture;
        nu_material_create(renderer, &info, &material);

        info             = nu_material_info_default(NU_MATERIAL_MESH);
        info.mesh.color0 = &texture_white;
        nu_material_create(renderer, &info, &material_white);

        info               = nu_material_info_default(NU_MATERIAL_CANVAS);
        info.canvas.color0 = &texture_gui;
        nu_material_create(renderer, &info, &material_gui);

        info                  = nu_material_info_default(NU_MATERIAL_CANVAS);
        info.canvas.color0    = &texture_gui;
        info.canvas.wrap_mode = NU_TEXTURE_WRAP_REPEAT;
        nu_material_create(renderer, &info, &material_gui_repeat);
    }

    // Load temple
    {
        nu_gltf_loader_t loader;
        nu_gltf_loader_init(allocator, logger, &loader);

        nuext_model_load_gltf_filename(&loader,
                                       "../../../assets/temple_of_time.glb",
                                       allocator,
                                       renderer,
                                       &temple_model);

        nuext_model_load_gltf_filename(&loader,
                                       "../../../assets/ariane6.glb",
                                       allocator,
                                       renderer,
                                       &ariane_model);

        nu_gltf_loader_free(&loader);
    }

    // Load cubemap
    nu_cubemap_t skybox;
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
            nuext_image_load_filename(filenames[i], allocator, &images[i]);
        }
        nu_cubemap_info_t info;
        info.size        = nu_image_size(images[0]).x;
        info.usage       = NU_TEXTURE_USAGE_SAMPLE;
        info.colors_posx = nu_image_colors(images[3]);
        info.colors_negx = nu_image_colors(images[2]);
        info.colors_posy = nu_image_colors(images[0]);
        info.colors_negy = nu_image_colors(images[1]);
        info.colors_posz = nu_image_colors(images[4]);
        info.colors_negz = nu_image_colors(images[5]);
        nu_cubemap_create(renderer, &info, &skybox);
        for (nu_size_t i = 0; i < 6; ++i)
        {
            nu_image_delete(images[i]);
        }
    }

    // Create font
    nu_font_t font;
    nu_font_create_default(renderer, allocator, &font);

    // Create camera
    nu_camera_t      camera;
    nu_camera_info_t camera_info = nu_camera_info();
    nu_camera_create(renderer, &camera_info, &camera);

    // Create renderpasses
    nu_renderpass_t      main_pass;
    nu_renderpass_info_t main_pass_info;
    main_pass_info.type               = NU_RENDERPASS_FLAT;
    main_pass_info.reset_after_submit = NU_TRUE;
    nu_renderpass_create(renderer, &main_pass_info, &main_pass);

    nu_renderpass_t      skybox_pass;
    nu_renderpass_info_t skybox_pass_info;
    skybox_pass_info.type               = NU_RENDERPASS_SKYBOX;
    skybox_pass_info.reset_after_submit = NU_TRUE;
    nu_renderpass_create(renderer, &skybox_pass_info, &skybox_pass);

    nu_renderpass_t      gui_pass;
    nu_renderpass_info_t gui_pass_info;
    gui_pass_info.type               = NU_RENDERPASS_CANVAS;
    gui_pass_info.reset_after_submit = NU_TRUE;
    nu_renderpass_create(renderer, &gui_pass_info, &gui_pass);

    // Create UI
    nu_ui_t      ui;
    nu_ui_info_t ui_info;
    ui_info.allocator = allocator;
    ui_info.renderer  = renderer;
    nu_ui_create(&ui_info, &ui);

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
    nu_ui_push_style(ui, &button_style);
    nu_ui_style_t checkbox_style;
    checkbox_style.type                      = NU_UI_CHECKBOX;
    checkbox_style.checkbox.checked.material = material_gui_repeat;
    checkbox_style.checkbox.checked.extent   = nu_rect(97, 257, 14, 14);
    checkbox_style.checkbox.checked.margin   = (nu_ui_margin_t) { 3, 3, 2, 2 };
    checkbox_style.checkbox.unchecked.material = material_gui_repeat;
    checkbox_style.checkbox.unchecked.extent   = nu_rect(81, 257, 14, 14);
    checkbox_style.checkbox.unchecked.margin = (nu_ui_margin_t) { 2, 4, 2, 2 };
    nu_ui_push_style(ui, &checkbox_style);
    nu_ui_style_t cursor_style;
    cursor_style.type                  = NU_UI_CURSOR;
    cursor_style.cursor.image.material = material_gui_repeat;
    cursor_style.cursor.image.extent   = nu_rect(98, 38, 3, 3);
    cursor_style.cursor.image.margin   = (nu_ui_margin_t) { 0, 0, 0, 0 };
    nu_ui_push_style(ui, &cursor_style);

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

    while (!nu_platform_exit_requested(platform) && running)
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
        nu_platform_poll_events(platform);

        // Check exit
        if (nu_input_just_pressed(quit))
        {
            running = NU_FALSE;
        }

        // Update camera controller
        nu_camera_controller_update(&controller, delta, &camera_info);
        nu_camera_update(renderer, camera, &camera_info);

        // Render loop
        for (int i = 0; i < 40; ++i)
        {
            nu_mat4_t model = nu_mat4_translate(
                nu_sin(time / 100 + i) * 4, nu_cos(time / 100 + i) * 10, i * 5);
            nu_draw_mesh(renderer, main_pass, material, cube_mesh, model);
        }

        // Render custom mesh
        nu_mat4_t transform = nu_mat4_scale(0.3, 0.3, 0.3);
        nu_draw_model(renderer, main_pass, ariane_model, transform);

        transform = nu_mat4_translate(10, 0, 0);
        nu_draw_model(renderer, main_pass, temple_model, transform);
        transform = nu_mat4_translate(20, 0, 0);
        nu_draw_model(renderer, main_pass, temple_model, transform);
        transform = nu_mat4_translate(30, 0, 0);
        nu_draw_model(renderer, main_pass, temple_model, transform);
        transform = nu_mat4_translate(40, 0, 0);
        nu_draw_model(renderer, main_pass, temple_model, transform);

        // Draw GUI
        for (nu_size_t i = 0; i < 30; ++i)
        {
            int x = i % 20;
            int y = i / 20;
            nu_draw_blit(renderer,
                         gui_pass,
                         material_gui,
                         nu_rect(10 + x * 15, 30 + y * 15, 14, 14),
                         nu_rect(81, 257, 14, 14));
        }

        nu_draw_blit(renderer,
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
        nu_draw_text(renderer,
                     gui_pass,
                     font,
                     s,
                     nu_strlen(s),
                     nu_ivec2(10, HEIGHT / 2));

        // GUI
        nu_ui_set_cursor(
            ui, 0, nuext_platform_cursor(platform, cursor_x, cursor_y));
        nu_ui_set_pressed(ui, 0, nu_input_pressed(main_button));
        nu_ui_begin(ui);
        if (nu_ui_button(ui, nu_rect(300, 100, 60, 20)))
        {
            NU_INFO(logger, "button pressed !");
        }
        if (nu_ui_checkbox(ui, nu_rect(300, 300, 14, 14), &bool_state))
        {
            NU_INFO(logger, "checkbox changed %d", bool_state);
        }
        nu_ui_end(ui);

        // Print FPS
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
        nu_draw_text(renderer, gui_pass, font, string, n, nu_ivec2(10, 10));

        // Submit renderpass
        nu_renderpass_submit_t submit;

        nu_texture_t surface_tex = nu_surface_color_target(renderer);
        nu_color_t   clear_color = NU_COLOR_BLUE_SKY;

        submit.flat.camera       = camera;
        submit.flat.color_target = &surface_tex;
        submit.flat.depth_target = &depth_buffer;
        submit.flat.clear_color  = &clear_color;
        nu_renderpass_submit(renderer, main_pass, &submit);

        submit.skybox.camera       = camera;
        submit.skybox.color_target = &surface_tex;
        submit.skybox.depth_target = &depth_buffer;
        submit.skybox.cubemap      = skybox;
        submit.skybox.rotation     = nu_quat_identity();
        nu_renderpass_submit(renderer, skybox_pass, &submit);

        submit.canvas.color_target = &surface_tex;
        nu_renderpass_submit(renderer, gui_pass, &submit);

        submit.canvas.color_target = &surface_tex;
        nu_ui_submit_renderpasses(ui, &submit);

        // Render
        nu_renderer_render(renderer);

        // Refresh surface
        nu_platform_swap_buffers(platform);
    }

    // Free cube
    nu_mesh_delete(renderer, cube_mesh);
    nu_renderpass_delete(renderer, main_pass);

    nu_renderer_delete(renderer);
    nu_platform_delete(platform);

    return 0;
}
