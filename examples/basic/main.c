#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

#define WIDTH  640
#define HEIGHT 400
// #define WIDTH  720
// #define HEIGHT 480
// #define WIDTH  1920
// #define HEIGHT 1080
// #define HEIGHT 640
// #define WIDTH  400
// #define WIDTH  640 / 2
// #define HEIGHT 400 / 2
// #define WIDTH  512
// #define HEIGHT 288

#define SHADOW_WIDTH  1000
#define SHADOW_HEIGHT 1000

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

static nu_controller_t controller;
static nu_texture_t    depth_buffer;
static nu_texture_t    shadow_map;
static nu_mesh_t       custom_mesh;
static nu_mesh_t       custom_mesh_normals;
static nu_b3_t         bounds;
static nu_texture_t    texture;
static nu_texture_t    texture_gui;
static nu_material_t   material;
static nu_material_t   material_gui_repeat;
static nu_material_t   material_gui;
static nu_model_t      castle;
static nu_texture_t    skybox;
static nu_font_t       font;
static nu_camera_t     camera;
static nu_texture_t    surface_tex;
static nu_color_t      clear_color;
static nu_renderpass_t main_pass;
static nu_renderpass_t gui_pass;
static nu_renderpass_t wireframe_pass;
static nu_renderpass_t shadow_pass;
static nu_ui_t         ui;
static nu_ui_style_t   style;
static nu_fixedloop_t  physics_loop_handle;
static nu_lightenv_t   lightenv;
static nu_camera_t     shadow_camera;
static nu_ecs_t        ecs;

void physics_loop(nu_f32_t timestep);

void
draw_scene (nu_renderpass_t pass)
{
    nu_draw_mesh(pass, custom_mesh, material, nu_m4_identity());
    nu_draw_model(pass, castle, nu_m4_identity());
    nu_m4_t transform = nu_m4_scale(nu_v3(4, 4, 4));
    transform = nu_m4_mul(nu_m4_translate(nu_v3(10, -50, 0)), transform);
    nu_draw_model(pass, temple_model, transform);

    transform = nu_m4_translate(nu_v3(0, 10, 0));
    nu_draw_box(pass, nu_b3(nu_v3s(-0.5), nu_v3s(0.5)), material, transform);
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
    nu_u32_t stat;
} player_t;

void
init (void)
{
    nuext_import_package("../../../assets/pkg.json");

    NU_SERIA_ENUM("component",
                  component_t,
                  NU_SERIA_ENUM_VALUE("transform", COMP_TRANSFORM);
                  NU_SERIA_ENUM_VALUE("player", COMP_PLAYER););
    NU_SERIA_STRUCT(
        "subtype",
        subtype_t,
        NU_SERIA_STRUCT_FIELD(
            "hello", NU_SERIA_U32, 1, NU_SERIA_REQUIRED, hello);
        NU_SERIA_STRUCT_FIELD(
            "vector", NU_SERIA_V3, 1, NU_SERIA_REQUIRED, vector);
        NU_SERIA_STRUCT_FIELD("quat", NU_SERIA_Q4, 1, NU_SERIA_REQUIRED, quat);
        NU_SERIA_STRUCT_FIELD("component",
                              nu_seria_type("component"),
                              1,
                              NU_SERIA_REQUIRED,
                              component););
    NU_SERIA_STRUCT(
        "transform",
        transform_t,
        NU_SERIA_STRUCT_FIELD(
            "position", NU_SERIA_V3, 1, NU_SERIA_REQUIRED, position);
        NU_SERIA_STRUCT_FIELD(
            "rotation", NU_SERIA_Q4, 1, NU_SERIA_REQUIRED, rotation);
        NU_SERIA_STRUCT_FIELD(
            "scale", NU_SERIA_V3, 1, NU_SERIA_REQUIRED, scale);
        NU_SERIA_STRUCT_FIELD("subtype",
                              nu_seria_type("subtype"),
                              1,
                              NU_SERIA_REQUIRED,
                              subtype););
    NU_SERIA_STRUCT("player",
                    player_t,
                    NU_SERIA_STRUCT_FIELD(
                        "stat", NU_SERIA_U32, 1, NU_SERIA_REQUIRED, stat));

    nu_seria_dump_types();

    transform_t trans;
    trans.scale             = NU_V3_UP;
    trans.position          = NU_V3_LEFT;
    trans.rotation          = nu_q4_identity();
    trans.subtype.quat      = nu_q4_identity();
    trans.subtype.hello     = 123;
    trans.subtype.vector    = nu_v3(1, 2, 3);
    trans.subtype.component = COMP_PLAYER;

    transform_t t[3]       = { trans, trans, trans };
    t[2].subtype.hello     = 666;
    t[2].subtype.component = COMP_TRANSFORM;
    nu_seria_dump(nu_seria_type("transform"), 3, t);

    // Configure inputs
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
    controller = nu_controller_create(
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
    depth_buffer
        = nu_texture_create(NU_TEXTURE_DEPTH_TARGET, nu_v3u(WIDTH, HEIGHT, 0));

    // Create meshes
    {
        nu_geometry_t final = nu_geometry_create();
        nu_geometry_t sub   = nu_geometry_create();
        for (nu_size_t i = 0; i < 10; ++i)
        {
            nu_geometry_cube(sub, 1);
            nu_m4_t transform = nu_m4_mul(nu_m4_translate(nu_v3s(i * 5)),
                                          nu_m4_scale(nu_v3s(2 * i)));
            nu_geometry_transform(sub, transform);
            nu_geometry_merge(final, sub);
        }

        // nu_geometry_grid(sub, 200, 200, 1, 75);
        // nu_geometry_transform(sub, nu_mat4_translate(nu_vec3(-100, 0,
        // -100))); nu_geometry_append(final, sub);

        // nu_geometry_cube(final, 1);
        nu_geometry_grid(sub, 5, 10, 1, 75);
        nu_geometry_merge(final, sub);
        // nu_geometry_plane(final, 10, 10);
        custom_mesh = nu_geometry_create_mesh(final, NU_PRIMITIVE_TRIANGLES);
        custom_mesh_normals = nu_geometry_create_mesh_normals(final);
        bounds              = nu_geometry_bounds(final);
        nu_geometry_delete(final);
    }

    // Load resources
    texture     = NU_ASSET_TEXTURE("brick");
    texture_gui = NU_ASSET_TEXTURE("GUI");

    // Create material
    material = nu_material_create(NU_MATERIAL_SURFACE);
    nu_material_set_texture(material, texture);
    material_gui_repeat = nu_material_create(NU_MATERIAL_CANVAS);
    nu_material_set_texture(material_gui_repeat, texture_gui);
    nu_material_set_wrap_mode(material_gui_repeat, NU_TEXTURE_WRAP_REPEAT);
    material_gui = nu_material_create(NU_MATERIAL_CANVAS);
    nu_material_set_texture(material_gui, texture_gui);
    nu_material_set_wrap_mode(material_gui, NU_TEXTURE_WRAP_CLAMP);

    // Load temple
    temple_model = NU_ASSET_MODEL("temple");
    ariane_model = NU_ASSET_MODEL("ariane");
    castle       = NU_ASSET_MODEL("castle");

    // Load cubemap
    skybox = NU_ASSET_TEXTURE("skybox");

    // Create lightenv

    // Create font
    font = nu_font_create_default();

    // Create camera
    camera = nu_camera_create();
    nu_camera_set_proj(
        camera, nu_perspective(nu_radian(60), nu_surface_aspect(), 0.01, 500));

    // Create renderpasses
    surface_tex = nu_surface_color_target();
    clear_color = NU_COLOR_BLUE_SKY;

    main_pass = nu_renderpass_create(NU_RENDERPASS_FORWARD);
    nu_renderpass_set_camera(main_pass, camera);
    nu_renderpass_set_color_target(main_pass, surface_tex);
    nu_renderpass_set_depth_target(main_pass, depth_buffer);
    nu_renderpass_set_clear_color(main_pass, &clear_color);
    nu_renderpass_set_shade(main_pass, NU_SHADE_UNLIT);

    gui_pass = nu_renderpass_create(NU_RENDERPASS_CANVAS);
    nu_renderpass_set_color_target(gui_pass, surface_tex);

    wireframe_pass = nu_renderpass_create(NU_RENDERPASS_FORWARD);
    nu_renderpass_set_camera(wireframe_pass, camera);
    nu_renderpass_set_color_target(wireframe_pass, surface_tex);
    nu_renderpass_set_depth_target(wireframe_pass, depth_buffer);
    nu_renderpass_set_shade(wireframe_pass, NU_SHADE_WIREFRAME);

    shadow_map    = nu_texture_create(NU_TEXTURE_SHADOW_TARGET,
                                   nu_v3u(SHADOW_WIDTH, SHADOW_HEIGHT, 0));
    shadow_camera = nu_camera_create();
    nu_camera_set_proj(shadow_camera, nu_ortho(-50, 50, -50, 50, 1, 500));
    nu_camera_set_view(shadow_camera,
                       nu_lookat(nu_v3s(100.0), nu_v3(0, 0, 10), NU_V3_UP));
    shadow_pass = nu_renderpass_create(NU_RENDERPASS_SHADOW);
    nu_renderpass_set_depth_target(shadow_pass, shadow_map);
    nu_renderpass_set_camera(shadow_pass, shadow_camera);

    nu_light_t light = nu_light_create(NU_LIGHT_DIRECTIONAL);
    lightenv         = nu_lightenv_create();
    nu_lightenv_add_shadowmap(lightenv, shadow_map, shadow_camera);
    nu_lightenv_set_skybox(lightenv, skybox, nu_q4_identity());
    nu_renderpass_set_lightenv(main_pass, lightenv);

    // Create UI
    ui    = nu_ui_create();
    style = nu_ui_style_create();

    nu_ui_style(style,
                NU_UI_STYLE_BUTTON_PRESSED,
                material_gui_repeat,
                nu_b2i_xywh(113, 97, 30, 14),
                nu_b2i_xywh(115, 100, 26, 8));
    nu_ui_style(style,
                NU_UI_STYLE_BUTTON_RELEASED,
                material_gui_repeat,
                nu_b2i_xywh(113, 81, 30, 14),
                nu_b2i_xywh(115, 83, 26, 8));
    nu_ui_style(style,
                NU_UI_STYLE_BUTTON_HOVERED,
                material_gui_repeat,
                nu_b2i_xywh(113, 113, 30, 14),
                nu_b2i_xywh(115, 115, 26, 8));
    nu_ui_style(style,
                NU_UI_STYLE_CHECKBOX_CHECKED,
                material_gui_repeat,
                nu_b2i_xywh(97, 257, 14, 14),
                nu_b2i_xywh(99, 260, 10, 8));
    nu_ui_style(style,
                NU_UI_STYLE_CHECKBOX_UNCHECKED,
                material_gui_repeat,
                nu_b2i_xywh(81, 257, 14, 14),
                nu_b2i_xywh(99, 261, 10, 8));
    nu_ui_style(style,
                NU_UI_STYLE_CURSOR,
                material_gui_repeat,
                nu_b2i_xywh(52, 83, 8, 7),
                nu_b2i_xywh(52, 83, 8, 7));

    nu_ui_push_style(ui, style);

    // Main loop
    nu_bool_t drawing = NU_FALSE;
    nu_bool_t running = NU_TRUE;

    physics_loop_handle
        = nu_fixedloop_create(physics_loop, 1.0 / 60.0 * 1000.0);

    nu_seria_t seria = nu_seria_create();
    nu_seria_open_reflect(seria);
    // nu_seria_subtype(seria, NU_NULL);
    nu_seria_delete(seria);
}

void
update (void)
{
    nu_fixedloop_update(nu_deltatime());

    // Check exit
    if (nu_input_just_pressed(quit))
    {
        nu_request_stop();
    }

    // Update camera controller
    nu_controller_update(controller, nu_deltatime(), camera);

    // Render loop
    nu_draw_mesh(
        wireframe_pass, custom_mesh_normals, material, nu_m4_identity());
    nu_draw_box(wireframe_pass, bounds, material, nu_m4_identity());
    nu_m4_t transform = nu_m4_identity();
    nu_draw_model(wireframe_pass, ariane_model, transform);

    draw_scene(main_pass);
    draw_scene(shadow_pass);

    const nu_v3_t points[] = { NU_V3_ZEROS, NU_V3_UP,    NU_V3_ZEROS,
                               NU_V3_RIGHT, NU_V3_ZEROS, NU_V3_BACKWARD };
    nu_draw_lines(
        wireframe_pass, points, 3, material, nu_m4_translate(nu_v3(-5, 5, -5)));
    nu_draw_box(wireframe_pass,
                nu_b3(NU_V3_ZEROS, NU_V3_ONES),
                material,
                nu_m4_translate(nu_v3(0, 5, -5)));

    // GUI
    nu_ui_set_cursor(ui, 0, nuext_platform_cursor(cursor_x, cursor_y));
    nu_ui_set_pressed(ui, 0, nu_input_pressed(main_button));
    nu_ui_begin(ui, gui_pass);

    // Print FPS
    nu_draw_stats(gui_pass, font, nu_v2i(10, 10));

    // Submit renderpass
    nu_renderpass_submit(shadow_pass);
    nu_renderpass_submit(main_pass);
    nu_renderpass_submit(wireframe_pass);
    nu_renderpass_submit(gui_pass);
}

void
physics_loop (nu_f32_t timestep)
{
}

void
nu_main (void)
{
    nu_app_surface_size(WIDTH, HEIGHT);
    nu_app_init_callback(init);
    nu_app_update_callback(update);
}
