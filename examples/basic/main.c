#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

// #define WIDTH  640
// #define HEIGHT 400
#define WIDTH  720
#define HEIGHT 480
// #define WIDTH  1920
// #define HEIGHT 1080
// #define HEIGHT 640
// #define WIDTH  400
// #define WIDTH  640 / 2
// #define HEIGHT 400 / 2
// #define WIDTH  512
// #define HEIGHT 288

static nu_input_t draw;
static nu_input_t main_button;
static nu_input_t quit;
static nu_input_t cursor_x;
static nu_input_t cursor_y;

static nu_input_t move_x;
static nu_input_t move_y;
static nu_input_t move_z;
static nu_input_t view_yaw;
static nu_input_t view_pitch;
static nu_input_t view_roll;
static nu_input_t switch_mode;

static nu_model_t temple_model;
static nu_model_t ariane_model;

#define LOOP_TICK    0
#define LOOP_PHYSICS 1

int
main (void)
{
    nu_config_surface_size(WIDTH, HEIGHT);
    nu_config_renderer_api(NU_RENDERER_GL);
    nu_init();

    nuext_import_package("../../../assets/pkg.json");

    // Configure inputs
    nu_controller_info_t cinfo;
    draw        = nu_input_create();
    main_button = nu_input_create();
    quit        = nu_input_create();
    cursor_x    = nu_input_create();
    cursor_y    = nu_input_create();
    move_x      = nu_input_create();
    move_y      = nu_input_create();
    move_z      = nu_input_create();
    view_yaw    = nu_input_create();
    view_pitch  = nu_input_create();
    view_roll   = nu_input_create();
    switch_mode = nu_input_create();

    // Create camera controller
    nu_controller_t controller = nu_controller_create(
        view_pitch, view_yaw, view_roll, move_x, move_y, move_z, switch_mode);

    // Bind inputs
    nuext_input_bind_button(main_button, NUEXT_BUTTON_MOUSE_LEFT);
    nuext_input_bind_button(quit, NUEXT_BUTTON_ESCAPE);
    nuext_input_bind_axis(cursor_x, NUEXT_AXIS_MOUSE_X);
    nuext_input_bind_axis(cursor_y, NUEXT_AXIS_MOUSE_Y);
    nuext_input_bind_button_value(move_z, NUEXT_BUTTON_W, 1);
    nuext_input_bind_button_value(move_z, NUEXT_BUTTON_S, -1);
    nuext_input_bind_button_value(move_x, NUEXT_BUTTON_D, 1);
    nuext_input_bind_button_value(move_x, NUEXT_BUTTON_A, -1);
    nuext_input_bind_button_value(move_y, NUEXT_BUTTON_X, 1);
    nuext_input_bind_button_value(move_y, NUEXT_BUTTON_Z, -1);
    nuext_input_bind_axis(view_pitch, NUEXT_AXIS_MOUSE_MOTION_Y);
    nuext_input_bind_axis(view_yaw, NUEXT_AXIS_MOUSE_MOTION_X);
    nuext_input_bind_button_value(view_pitch, NUEXT_BUTTON_J, 0.12);
    nuext_input_bind_button_value(view_pitch, NUEXT_BUTTON_K, -0.12);
    nuext_input_bind_button_value(view_yaw, NUEXT_BUTTON_L, 0.12);
    nuext_input_bind_button_value(view_yaw, NUEXT_BUTTON_H, -0.12);
    nuext_input_bind_button_value(view_roll, NUEXT_BUTTON_Q, 0.12);
    nuext_input_bind_button_value(view_roll, NUEXT_BUTTON_E, -0.12);
    nuext_input_bind_button(switch_mode, NUEXT_BUTTON_C);

    // Create depth buffer
    nu_texture_t depth_buffer = nu_texture_create(nu_vec2u(WIDTH, HEIGHT),
                                                  NU_TEXTURE_FORMAT_DEPTH,
                                                  NU_TEXTURE_USAGE_TARGET,
                                                  NU_NULL);

    // Create meshes
    nu_mesh_t custom_mesh;
    {
        nu_geometry_t final = nu_geometry_create(NU_PRIMITIVE_TRIANGLES, 1000);
        nu_geometry_t sub   = nu_geometry_create(NU_PRIMITIVE_TRIANGLES, 2 * 6);
        for (nu_size_t i = 0; i < 10; ++i)
        {
            nu_geometry_cube(sub, 1);
            nu_mat4_t transform
                = nu_mat4_mul(nu_mat4_translate(nu_vec3s(i * 5)),
                              nu_mat4_scale(nu_vec3s(2 * i)));
            nu_geometry_transform(sub, transform);
            nu_geometry_append(final, sub);
        }

        nu_geometry_grid(sub, 200, 200, 1, 75);
        nu_geometry_transform(sub, nu_mat4_translate(nu_vec3(-100, 0, -100)));
        nu_geometry_append(final, sub);

        custom_mesh = nu_geometry_create_mesh(final);
        nu_geometry_delete(sub);
    }

    // Load resources
    nu_texture_t texture     = NU_ASSET_TEXTURE("brick");
    nu_texture_t texture_gui = NU_ASSET_TEXTURE("GUI");

    // Create material
    nu_material_t material = nu_material_create(NU_MATERIAL_TYPE_SURFACE);
    nu_material_texture(material, NU_MATERIAL_TEXTURE0, texture);
    nu_material_t material_gui_repeat
        = nu_material_create(NU_MATERIAL_TYPE_CANVAS);
    nu_material_texture(material_gui_repeat, NU_MATERIAL_TEXTURE0, texture_gui);
    nu_material_wrap_mode(
        material_gui_repeat, NU_MATERIAL_WRAP_MODE, NU_TEXTURE_WRAP_REPEAT);

    // Load temple
    temple_model = NU_ASSET_MODEL("temple");
    ariane_model = NU_ASSET_MODEL("ariane");

    // Load cubemap
    nu_cubemap_t skybox = NU_ASSET_CUBEMAP("skybox");

    // Create font
    nu_font_t font = nu_font_create_default();

    // Create camera
    nu_camera_t camera = nu_camera_create();

    // Create renderpasses
    nu_texture_t surface_tex = nu_surface_color_target();
    nu_color_t   clear_color = NU_COLOR_BLUE_SKY;

    nu_renderpass_t main_pass
        = nu_renderpass_create(NU_RENDERPASS_TYPE_FLAT, NU_TRUE);
    nu_renderpass_camera(main_pass, NU_RENDERPASS_CAMERA, camera);
    nu_renderpass_texture(main_pass, NU_RENDERPASS_COLOR_TARGET, surface_tex);
    nu_renderpass_texture(main_pass, NU_RENDERPASS_DEPTH_TARGET, depth_buffer);
    nu_renderpass_color(main_pass, NU_RENDERPASS_CLEAR_COLOR, &clear_color);

    nu_renderpass_t skybox_pass
        = nu_renderpass_create(NU_RENDERPASS_TYPE_SKYBOX, NU_TRUE);
    nu_renderpass_texture(skybox_pass, NU_RENDERPASS_COLOR_TARGET, surface_tex);
    nu_renderpass_texture(
        skybox_pass, NU_RENDERPASS_DEPTH_TARGET, depth_buffer);
    nu_renderpass_cubemap(skybox_pass, NU_RENDERPASS_SKYBOX, skybox);
    nu_renderpass_quat(
        skybox_pass, NU_RENDERPASS_SKYBOX_ROTATION, nu_quat_identity());
    nu_renderpass_camera(skybox_pass, NU_RENDERPASS_CAMERA, camera);

    nu_renderpass_t gui_pass
        = nu_renderpass_create(NU_RENDERPASS_TYPE_CANVAS, NU_TRUE);
    nu_renderpass_texture(gui_pass, NU_RENDERPASS_COLOR_TARGET, surface_tex);

    nu_renderpass_t wireframe_pass
        = nu_renderpass_create(NU_RENDERPASS_TYPE_WIREFRAME, NU_TRUE);
    nu_renderpass_camera(wireframe_pass, NU_RENDERPASS_CAMERA, camera);
    nu_renderpass_texture(
        wireframe_pass, NU_RENDERPASS_COLOR_TARGET, surface_tex);
    nu_renderpass_texture(
        wireframe_pass, NU_RENDERPASS_DEPTH_TARGET, depth_buffer);

    // Create UI
    nu_ui_t       ui    = nu_ui_create();
    nu_ui_style_t style = nu_ui_style_create();

    nu_ui_style(style,
                NU_UI_STYLE_BUTTON_PRESSED,
                material_gui_repeat,
                nu_box2i(113, 97, 30, 14),
                nu_box2i(115, 100, 26, 8));
    nu_ui_style(style,
                NU_UI_STYLE_BUTTON_RELEASED,
                material_gui_repeat,
                nu_box2i(113, 81, 30, 14),
                nu_box2i(115, 83, 26, 8));
    nu_ui_style(style,
                NU_UI_STYLE_BUTTON_HOVERED,
                material_gui_repeat,
                nu_box2i(113, 113, 30, 14),
                nu_box2i(115, 116, 26, 8));
    nu_ui_style(style,
                NU_UI_STYLE_CHECKBOX_CHECKED,
                material_gui_repeat,
                nu_box2i(97, 257, 14, 14),
                nu_box2i(99, 260, 10, 8));
    nu_ui_style(style,
                NU_UI_STYLE_CHECKBOX_UNCHECKED,
                material_gui_repeat,
                nu_box2i(81, 257, 14, 14),
                nu_box2i(99, 261, 10, 8));
    nu_ui_style(style,
                NU_UI_STYLE_CURSOR,
                material_gui_repeat,
                nu_box2i(52, 83, 8, 7),
                nu_box2i(52, 83, 8, 7));

    nu_ui_push_style(ui, style);

    // Main loop
    nu_bool_t drawing = NU_FALSE;
    nu_bool_t running = NU_TRUE;

    nu_fixed_loop_t loops[2];
    loops[0] = nu_fixed_loop(LOOP_TICK, 1.0 / 60.0 * 1000.0);
    loops[1] = nu_fixed_loop(LOOP_PHYSICS, 1.0 / 20.0 * 1000.0);

    nu_timer_t timer;
    nu_timer_reset(&timer);
    float     delta      = 0.0f;
    nu_bool_t bool_state = NU_TRUE;

    while (!nu_exit_requested() && running)
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

        // Poll events
        nu_poll_events();

        // Check exit
        if (nu_input_just_pressed(quit))
        {
            running = NU_FALSE;
        }

        // Update camera controller
        nu_controller_update(controller, delta, camera);

        // Render loop
        nu_bind_material(main_pass, material);
        nu_draw_mesh(main_pass, custom_mesh, nu_mat4_identity());

        // Render custom mesh
        nu_mat4_t transform = nu_mat4_identity();
        nu_draw_model(wireframe_pass, ariane_model, transform);

        transform = nu_mat4_scale(nu_vec3(4, 4, 4));
        transform
            = nu_mat4_mul(nu_mat4_translate(nu_vec3(10, -3.97, 0)), transform);
        nu_draw_model(main_pass, temple_model, transform);

        const nu_vec3_t points[]
            = { NU_VEC3_ZERO,  NU_VEC3_UP,   NU_VEC3_ZERO,
                NU_VEC3_RIGHT, NU_VEC3_ZERO, NU_VEC3_BACKWARD };
        nu_draw_lines(
            wireframe_pass, points, 3, nu_mat4_translate(nu_vec3(-5, 5, -5)));
        nu_draw_box(wireframe_pass,
                    nu_box3(NU_VEC3_ZERO, NU_VEC3_ONE),
                    nu_mat4_translate(nu_vec3(0, 5, -5)));

        // GUI
        nu_ui_set_cursor(ui, 0, nuext_platform_cursor(cursor_x, cursor_y));
        nu_ui_set_pressed(ui, 0, nu_input_pressed(main_button));
        nu_ui_begin(ui);
        if (nu_ui_button(ui, nu_box2i(300, 100, 60, 20)))
        {
            NU_INFO("button pressed !");
        }
        if (nu_ui_checkbox(ui, nu_box2i(300, 300, 14, 14), &bool_state))
        {
            NU_INFO("checkbox changed %d", bool_state);
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
        nu_size_t n = nu_snprintf(string, 256, "FPS:%d", (nu_u32_t)frame_avg);
        nu_draw_text(gui_pass, font, string, n, nu_vec2i(10, 10));
        n = nu_snprintf(string, 256, "RES:%dx%d", WIDTH, HEIGHT);
        nu_draw_text(gui_pass, font, string, n, nu_vec2i(10, 20));

        // Submit renderpass
        nu_renderpass_submit(main_pass);
        nu_renderpass_submit(skybox_pass);
        nu_renderpass_submit(wireframe_pass);
        nu_renderpass_submit(gui_pass);
        nu_ui_submit_renderpasses(ui, surface_tex);

        // Refresh surface
        nu_swap_buffers();
    }

    nu_terminate();

    return 0;
}
