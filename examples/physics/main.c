#include "nucleus/graphics/api.h"
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

#define WIDTH  720
#define HEIGHT 480
// #define WIDTH  640
// #define HEIGHT 400

typedef struct
{
    nu_vec3_t pos;
    nu_vec3_t vel;
    nu_vec3_t acc;
} point_mass_t;

typedef NU_VEC(point_mass_t) point_mass_vec_t;

typedef struct
{
    nu_vec3_t normal;
    float     depth;
} collision_t;

typedef NU_VEC(collision_t) collision_vec_t;

typedef struct
{
    point_mass_vec_t pms;
    collision_vec_t  collisions;
    nu_vec3_t        global_force;
} context_t;

static context_t ctx;

static void
reset_context (void)
{
    NU_VEC_CLEAR(&ctx.pms);
    ctx.global_force = nu_vec3(0, -9.81 / 2, 0);
    point_mass_t *pm = NU_VEC_PUSH(&ctx.pms);
    pm->pos          = nu_vec3(0, 5, 0);
    pm->vel          = NU_VEC3_ZEROS;
    pm->acc          = NU_VEC3_ZEROS;
}
static void
init_context (void)
{
    NU_VEC_INIT(100, &ctx.pms);
    NU_VEC_INIT(100, &ctx.collisions);
    reset_context();
}
static void
free_context (void)
{
    NU_VEC_FREE(&ctx.collisions);
    NU_VEC_FREE(&ctx.pms);
}
static nu_vec3_t
compute_force (point_mass_t *pm)
{
    return ctx.global_force;
}

static void
update_context (float dt)
{
    dt *= 0.001;
    for (nu_size_t i = 0; i < ctx.pms.size; ++i)
    {
        // Verlet integration
        point_mass_t *pm = ctx.pms.data + i;
        pm->pos
            = nu_vec3_add(pm->pos,
                          nu_vec3_add(nu_vec3_muls(pm->vel, dt),
                                      nu_vec3_muls(pm->acc, 0.5 * dt * dt)));
        nu_vec3_t new_acc = compute_force(pm);
        nu_vec3_t new_vel = nu_vec3_add(
            pm->vel, nu_vec3_muls(nu_vec3_add(pm->acc, new_acc), 0.5 * dt));
        pm->acc = new_acc;
        pm->vel = new_vel;
    }
}

int
main (void)
{
    nu_config_surface_size(WIDTH, HEIGHT);
    nu_config_renderer_api(NU_RENDERER_GL);
    nu_init();

    // Configure inputs
    nu_input_t quit        = nu_input_create();
    nu_input_t move_x      = nu_input_create();
    nu_input_t move_y      = nu_input_create();
    nu_input_t move_z      = nu_input_create();
    nu_input_t view_yaw    = nu_input_create();
    nu_input_t view_pitch  = nu_input_create();
    nu_input_t view_roll   = nu_input_create();
    nu_input_t switch_mode = nu_input_create();
    nu_input_t reset       = nu_input_create();

    // Create camera controller
    nu_controller_t controller = nu_controller_create(
        view_pitch, view_yaw, view_roll, move_x, move_y, move_z, switch_mode);

    // Bind inputs
    nuext_input_bind_button(quit, NUEXT_BUTTON_ESCAPE);
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
    nuext_input_bind_button(reset, NUEXT_BUTTON_R);

    // Create depth buffer
    nu_texture_t depth_buffer = nu_texture_create(nu_vec2u(WIDTH, HEIGHT),
                                                  NU_TEXTURE_FORMAT_DEPTH,
                                                  NU_TEXTURE_USAGE_TARGET,
                                                  NU_NULL);
    // Grid mesh
    nu_mesh_t grid;
    {
        nu_geometry_t g = nu_geometry_create();
        nu_geometry_grid(g, 30, 30, 1, 1);
        nu_geometry_transform(g, nu_mat4_translate(nu_vec3(-15, 0, -15)));
        grid = nu_geometry_create_mesh(g, NU_PRIMITIVE_LINES);
        nu_geometry_delete(g);
    }

    // Cube mesh
    nu_mesh_t cube;
    {
        nu_geometry_t g = nu_geometry_create();
        nu_geometry_cube(g, 0.1);
        nu_geometry_transform(g, nu_mat4_translate(nu_vec3s(-0.05)));
        cube = nu_geometry_create_mesh(g, NU_PRIMITIVE_LINES);
        nu_geometry_delete(g);
    }

    // Create font
    nu_font_t font = nu_font_create_default();

    // Create camera
    nu_camera_t camera = nu_camera_create();
    nu_camera_proj(
        camera, nu_perspective(nu_radian(60), nu_surface_aspect(), 0.01, 500));

    // Create renderpasses
    nu_texture_t surface_tex = nu_surface_color_target();
    nu_color_t   clear_color = NU_COLOR_BLACK;

    nu_renderpass_t main_pass
        = nu_renderpass_create(NU_RENDERPASS_TYPE_FLAT, NU_TRUE);
    nu_renderpass_camera(main_pass, NU_RENDERPASS_CAMERA, camera);
    nu_renderpass_texture(main_pass, NU_RENDERPASS_COLOR_TARGET, surface_tex);
    nu_renderpass_texture(main_pass, NU_RENDERPASS_DEPTH_TARGET, depth_buffer);
    nu_renderpass_color(main_pass, NU_RENDERPASS_CLEAR_COLOR, &clear_color);

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

    // Main loop
    nu_timer_t timer;
    nu_timer_reset(&timer);
    float delta = 0.0f;

    init_context();

    while (!nu_exit_requested())
    {
        delta = nu_timer_elapsed(&timer);
        nu_timer_reset(&timer);

        // Poll events
        nu_poll_events();

        if (nu_input_just_pressed(reset))
        {
            reset_context();
        }

        update_context(delta);

        for (nu_size_t i = 0; i < ctx.pms.size; ++i)
        {
            point_mass_t *pm = ctx.pms.data + i;
            nu_draw_mesh(wireframe_pass, cube, nu_mat4_translate(pm->pos));
        }

        // Update camera controller
        nu_controller_update(controller, delta, camera);

        nu_draw_mesh(wireframe_pass, grid, nu_mat4_identity());
        nu_draw_stats(gui_pass, font, nu_vec2i(10, 10));

        // Submit renderpass
        nu_renderpass_submit(main_pass);
        nu_renderpass_submit(wireframe_pass);
        nu_renderpass_submit(gui_pass);

        // Refresh surface
        nu_swap_buffers();
    }

    free_context();
    nu_terminate();

    return 0;
}
