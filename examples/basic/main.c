#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

// #define WIDTH  640
// #define HEIGHT 400
// #define WIDTH  720
// #define HEIGHT 480
// #define WIDTH  1920
// #define HEIGHT 1080
// #define HEIGHT 640
// #define WIDTH  400
// #define WIDTH  640 / 2
// #define HEIGHT 400 / 2
#define WIDTH  512
#define HEIGHT 288

#define SHADOW_WIDTH  1000
#define SHADOW_HEIGHT 1000

static nu_scope_t g_scope;

static nu_input_t g_draw;
static nu_input_t g_main_button;
static nu_input_t g_quit;
static nu_input_t g_cursor_x;
static nu_input_t g_cursor_y;

static nu_input_t g_move_x;
static nu_input_t g_move_y;
static nu_input_t g_move_z;
static nu_input_t g_view_yaw;
static nu_input_t g_view_pitch;
static nu_input_t g_view_roll;
static nu_input_t g_switch_mode;

static nu_model_t g_temple_model;
static nu_model_t g_ariane_model;

static nu_controller_t g_controller;
static nu_texture_t    g_depth_buffer;
static nu_texture_t    g_shadow_map;
static nu_mesh_t       g_custom_mesh;
static nu_mesh_t       g_custom_mesh_normals;
static nu_b3_t         g_bounds;
static nu_texture_t    g_texture;
static nu_texture_t    g_texture_gui;
static nu_material_t   g_material;
static nu_material_t   g_material_gui_repeat;
static nu_material_t   g_material_gui;
static nu_model_t      g_castle;
static nu_texture_t    g_skybox;
static nu_font_t       g_font;
static nu_camera_t     g_camera;
static nu_texture_t    g_surface_tex;
static nu_color_t      g_clear_color;
static nu_renderpass_t g_main_pass;
static nu_renderpass_t g_gui_pass;
static nu_renderpass_t g_wireframe_pass;
static nu_renderpass_t g_shadow_pass;
static nu_ui_t         g_ui;
static nu_ui_style_t   g_style;
static nu_fixedloop_t  g_physics_loop_handle;
static nu_lightenv_t   g_lightenv;
static nu_camera_t     g_shadow_camera;
static nu_ecs_t        g_ecs;

void physics_loop(nu_f32_t timestep);

void
draw_scene (nu_renderpass_t pass)
{
    nu_draw_mesh(pass, g_custom_mesh, g_material, nu_m4_identity());
    nu_draw_model(pass, g_castle, nu_m4_identity());
    nu_m4_t transform = nu_m4_scale(nu_v3(4, 4, 4));
    transform = nu_m4_mul(nu_m4_translate(nu_v3(10, -50, 0)), transform);
    nu_draw_model(pass, g_temple_model, transform);

    transform = nu_m4_translate(nu_v3(0, 10, 0));
    nu_draw_box(pass, nu_b3(nu_v3s(-0.5), nu_v3s(0.5)), g_material, transform);
}

typedef enum
{
    COMP_TRANSFORM,
    COMP_PLAYER
} component_t;

typedef struct
{
    int         hello;
    nu_v3_t     vector;
    nu_q4_t     quat;
    component_t component;
} subtype_t;

typedef struct
{
    nu_v3_t   position;
    nu_q4_t   rotation;
    nu_v3_t   scale;
    subtype_t subtype;
} transform_t;

typedef struct
{
    nu_u32_t     stat;
    nu_v3_t      v;
    nu_texture_t texture;
} player_t;

void
init (void)
{
    g_scope = nu_scope_new(NU_STR("main"), NU_MEM_128M);

    nu_scope_set_active(g_scope);
    nuext_import_package(NU_STR("../../../assets/pkg.json"), NU_UID("import"));
    // nu_seria_t seria = nu_seria_new();
    // nu_seria_open_file(seria, NU_SERIA_WRITE, NU_STR("pkg.bin"), NU_MEM_64M);
    // nu_resource_save_group(NU_UID("import"), seria);
    // nu_seria_close(seria);

    // Configure inputs
    g_draw        = nu_input_new();
    g_main_button = nu_input_new();
    g_quit        = nu_input_new();
    g_cursor_x    = nu_input_new();
    g_cursor_y    = nu_input_new();
    g_move_x      = nu_input_new();
    g_move_y      = nu_input_new();
    g_move_z      = nu_input_new();
    g_view_yaw    = nu_input_new();
    g_view_pitch  = nu_input_new();
    g_view_roll   = nu_input_new();
    g_switch_mode = nu_input_new();

    nu_object_set_uid(g_draw, NU_UID("draw"));
    NU_INFO(NU_UID_FMT, nu_object_get_uid(g_draw));

    // Create camera controller
    g_controller = nu_controller_new(g_view_pitch,
                                     g_view_yaw,
                                     g_view_roll,
                                     g_move_x,
                                     g_move_y,
                                     g_move_z,
                                     g_switch_mode);

    // Bind inputs
    nuext_input_bind_button(g_main_button, NUEXT_BUTTON_MOUSE_LEFT);
    nuext_input_bind_button(g_quit, NUEXT_BUTTON_ESCAPE);
    nuext_input_bind_axis(g_cursor_x, NUEXT_AXIS_MOUSE_X);
    nuext_input_bind_axis(g_cursor_y, NUEXT_AXIS_MOUSE_Y);
    nuext_input_bind_button_value(g_move_z, NUEXT_BUTTON_W, 1);
    nuext_input_bind_button_value(g_move_z, NUEXT_BUTTON_S, -1);
    nuext_input_bind_button_value(g_move_x, NUEXT_BUTTON_D, 1);
    nuext_input_bind_button_value(g_move_x, NUEXT_BUTTON_A, -1);
    nuext_input_bind_button_value(g_move_y, NUEXT_BUTTON_X, 1);
    nuext_input_bind_button_value(g_move_y, NUEXT_BUTTON_Z, -1);
    nuext_input_bind_axis(g_view_pitch, NUEXT_AXIS_MOUSE_MOTION_Y);
    nuext_input_bind_axis(g_view_yaw, NUEXT_AXIS_MOUSE_MOTION_X);
    nuext_input_bind_button_value(g_view_pitch, NUEXT_BUTTON_J, 0.12);
    nuext_input_bind_button_value(g_view_pitch, NUEXT_BUTTON_K, -0.12);
    nuext_input_bind_button_value(g_view_yaw, NUEXT_BUTTON_L, 0.12);
    nuext_input_bind_button_value(g_view_yaw, NUEXT_BUTTON_H, -0.12);
    nuext_input_bind_button_value(g_view_roll, NUEXT_BUTTON_Q, 0.12);
    nuext_input_bind_button_value(g_view_roll, NUEXT_BUTTON_E, -0.12);
    nuext_input_bind_button(g_switch_mode, NUEXT_BUTTON_C);

    // Create depth buffer
    g_depth_buffer = nu_texture_new(
        NU_TEXTURE_DEPTHBUFFER_TARGET, nu_v3u(WIDTH, HEIGHT, 1), 1);

    // Create meshes
    {
        g_custom_mesh = nu_mesh_new(NU_PRIMITIVE_TRIANGLES, 10000, NU_MESH_ALL);
        nu_mesh_t sub = nu_mesh_new(NU_PRIMITIVE_TRIANGLES, 1000, NU_MESH_ALL);
        for (nu_size_t i = 0; i < 10; ++i)
        {
            nu_mesh_cube(sub, 1);
            nu_m4_t transform = nu_m4_mul(nu_m4_translate(nu_v3s(i * 5)),
                                          nu_m4_scale(nu_v3s(2 * i)));
            nu_mesh_transform(sub, transform);
            nu_mesh_merge(g_custom_mesh, sub);
        }

        // nu_geometry_grid(sub, 200, 200, 1, 75);
        // nu_geometry_transform(sub, nu_mat4_translate(nu_vec3(-100, 0,
        // -100))); nu_geometry_append(final, sub);

        // nu_geometry_cube(g_custom_mesh, 1);
        nu_mesh_grid(sub, 5, 10, 1, 75);
        nu_mesh_merge(g_custom_mesh, sub);
        // nu_mesh_plane(g_custom_mesh, 10, 10);
        g_custom_mesh_normals = nu_mesh_new_normals(g_custom_mesh);
        g_bounds              = nu_mesh_bounds(g_custom_mesh);
    }

    // Load resources
    g_texture     = nu_object_find(nu_texture(), NU_UID("brick"));
    g_texture_gui = nu_object_find(nu_texture(), NU_UID("GUI"));

    // Create material
    g_material = nu_material_new(NU_MATERIAL_SURFACE);
    nu_material_set_texture(g_material, g_texture);
    g_material_gui_repeat = nu_material_new(NU_MATERIAL_CANVAS);
    nu_material_set_texture(g_material_gui_repeat, g_texture_gui);
    nu_material_set_wrap_mode(g_material_gui_repeat, NU_TEXTURE_WRAP_REPEAT);
    g_material_gui = nu_material_new(NU_MATERIAL_CANVAS);
    nu_material_set_texture(g_material_gui, g_texture_gui);
    nu_material_set_wrap_mode(g_material_gui, NU_TEXTURE_WRAP_CLAMP);

    // Load temple
    g_temple_model = nu_object_find(nu_model(), NU_UID("temple"));
    g_ariane_model = nu_object_find(nu_model(), NU_UID("ariane"));
    g_castle       = nu_object_find(nu_model(), NU_UID("castle"));

    // Load cubemap
    g_skybox = nu_object_find(nu_texture(), NU_UID("skybox"));

    // Create lightenv

    // Create font
    g_font = nu_font_new_default();

    // Create camera
    g_camera = nu_camera_new();
    nu_camera_set_proj(
        g_camera,
        nu_perspective(nu_radian(60), nu_surface_aspect(), 0.01, 500));

    // Create renderpasses
    g_surface_tex = nu_surface_color_target();
    g_clear_color = NU_COLOR_BLUE_SKY;

    g_main_pass = nu_renderpass_new(NU_RENDERPASS_FORWARD);
    nu_renderpass_set_camera(g_main_pass, g_camera);
    nu_renderpass_set_color_target(g_main_pass, g_surface_tex);
    nu_renderpass_set_depth_target(g_main_pass, g_depth_buffer);
    nu_renderpass_set_clear_color(g_main_pass, &g_clear_color);
    nu_renderpass_set_shade(g_main_pass, NU_SHADE_UNLIT);

    g_gui_pass = nu_renderpass_new(NU_RENDERPASS_CANVAS);

    nu_renderpass_set_color_target(g_gui_pass, g_surface_tex);

    g_wireframe_pass = nu_renderpass_new(NU_RENDERPASS_FORWARD);
    nu_renderpass_set_camera(g_wireframe_pass, g_camera);
    nu_renderpass_set_color_target(g_wireframe_pass, g_surface_tex);
    nu_renderpass_set_depth_target(g_wireframe_pass, g_depth_buffer);
    nu_renderpass_set_shade(g_wireframe_pass, NU_SHADE_WIREFRAME);

    g_shadow_map = nu_texture_new(
        NU_TEXTURE_SHADOWMAP_TARGET, nu_v3u(SHADOW_WIDTH, SHADOW_HEIGHT, 1), 1);
    g_shadow_camera = nu_camera_new();
    nu_camera_set_proj(g_shadow_camera, nu_ortho(-50, 50, -50, 50, 1, 500));
    nu_camera_set_view(g_shadow_camera,
                       nu_lookat(nu_v3s(100.0), nu_v3(0, 0, 10), NU_V3_UP));
    g_shadow_pass = nu_renderpass_new(NU_RENDERPASS_SHADOW);
    nu_renderpass_set_depth_target(g_shadow_pass, g_shadow_map);
    nu_renderpass_set_camera(g_shadow_pass, g_shadow_camera);

    nu_light_t light = nu_light_new(NU_LIGHT_DIRECTIONAL);
    g_lightenv       = nu_lightenv_new();
    nu_lightenv_add_shadowmap(g_lightenv, g_shadow_map, g_shadow_camera);
    nu_lightenv_set_skybox(g_lightenv, g_skybox, nu_q4_identity());
    nu_renderpass_set_lightenv(g_main_pass, g_lightenv);

    // Create UI
    g_ui    = nu_ui_new();
    g_style = nu_ui_style_new();

    nu_ui_style(g_style,
                NU_UI_STYLE_BUTTON_PRESSED,
                g_material_gui_repeat,
                nu_b2i_xywh(113, 97, 30, 14),
                nu_b2i_xywh(115, 100, 26, 8));
    nu_ui_style(g_style,
                NU_UI_STYLE_BUTTON_RELEASED,
                g_material_gui_repeat,
                nu_b2i_xywh(113, 81, 30, 14),
                nu_b2i_xywh(115, 83, 26, 8));
    nu_ui_style(g_style,
                NU_UI_STYLE_BUTTON_HOVERED,
                g_material_gui_repeat,
                nu_b2i_xywh(113, 113, 30, 14),
                nu_b2i_xywh(115, 115, 26, 8));
    nu_ui_style(g_style,
                NU_UI_STYLE_CHECKBOX_CHECKED,
                g_material_gui_repeat,
                nu_b2i_xywh(97, 257, 14, 14),
                nu_b2i_xywh(99, 260, 10, 8));
    nu_ui_style(g_style,
                NU_UI_STYLE_CHECKBOX_UNCHECKED,
                g_material_gui_repeat,
                nu_b2i_xywh(81, 257, 14, 14),
                nu_b2i_xywh(99, 261, 10, 8));
    nu_ui_style(g_style,
                NU_UI_STYLE_CURSOR,
                g_material_gui_repeat,
                nu_b2i_xywh(52, 83, 8, 7),
                nu_b2i_xywh(52, 83, 8, 7));

    nu_ui_push_style(g_ui, g_style);

    // Main loop
    nu_bool_t drawing = NU_FALSE;
    nu_bool_t running = NU_TRUE;
}

void
update (void)
{
    nu_fixedloop_update(nu_deltatime());

    // Check exit
    if (nu_input_just_pressed(g_quit))
    {
        nu_request_stop();
    }

    // Update camera controller
    nu_controller_update(g_controller, nu_deltatime(), g_camera);

    // Render loop
    nu_draw_mesh(
        g_wireframe_pass, g_custom_mesh_normals, g_material, nu_m4_identity());
    nu_draw_box(g_wireframe_pass, g_bounds, g_material, nu_m4_identity());
    nu_m4_t transform = nu_m4_identity();
    nu_draw_model(g_wireframe_pass, g_ariane_model, transform);

    draw_scene(g_main_pass);
    draw_scene(g_shadow_pass);

    const nu_v3_t points[] = { NU_V3_ZEROS, NU_V3_UP,    NU_V3_ZEROS,
                               NU_V3_RIGHT, NU_V3_ZEROS, NU_V3_BACKWARD };
    nu_draw_lines(g_wireframe_pass,
                  points,
                  3,
                  g_material,
                  nu_m4_translate(nu_v3(-5, 5, -5)));
    nu_draw_box(g_wireframe_pass,
                nu_b3(NU_V3_ZEROS, NU_V3_ONES),
                g_material,
                nu_m4_translate(nu_v3(0, 5, -5)));

    // GUI
    nu_ui_set_cursor(g_ui, 0, nuext_platform_cursor(g_cursor_x, g_cursor_y));
    nu_ui_set_pressed(g_ui, 0, nu_input_pressed(g_main_button));
    nu_ui_begin(g_ui, g_gui_pass);

    // Print FPS
    nu_draw_stats(g_gui_pass, g_font, nu_v2i(10, 10));

    // Submit renderpass
    nu_renderpass_submit(g_shadow_pass);
    nu_renderpass_submit(g_main_pass);
    nu_renderpass_submit(g_wireframe_pass);
    nu_renderpass_submit(g_gui_pass);
}

void
nu_app (void)
{
    nu_app_surface_size(WIDTH, HEIGHT);
    nu_app_init_callback(init);
    nu_app_update_callback(update);
    // nu_app_log_level(NU_LOG_DEBUG);
}
