#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

#define WIDTH  500
#define HEIGHT 500

static nu_renderpass_t g_renderpass;
static nu_texture_t    g_depth_buffer;
static nu_texture_t    g_texture;
static nu_material_t   g_material;
static nu_camera_t     g_camera;
static nu_input_t      g_exit_input;
static float           g_global_time;
static nu_renderpass_t g_guipass;
static nu_font_t       g_font;
static nu_texture_t    g_surface_color;

static nu_scope_t g_scope;

void
init (void)
{
    g_scope = nu_scope_new(NU_STR("main"), NU_MEM_1M);
    nu_scope_set_active(g_scope);

    // Material
    g_texture  = nu_texture_new_from_color(NU_COLOR_WHITE);
    g_material = nu_material_new(NU_MATERIAL_SURFACE);
    nu_material_set_texture(g_material, g_texture);

    // Camera
    g_camera = nu_camera_new();
    nu_camera_set_proj(
        g_camera,
        nu_perspective(nu_radian(60), nu_surface_aspect(), 0.01, 100));
    nu_camera_set_view(g_camera,
                       nu_lookat(nu_v3(2, 1, 2), NU_V3_ZEROS, NU_V3_UP));

    // Renderpass
    g_depth_buffer = nu_texture_new(
        NU_TEXTURE_DEPTHBUFFER_TARGET, nu_v3u(WIDTH, HEIGHT, 1), 1);
    nu_color_t clear_color = NU_COLOR_BLACK;
    g_surface_color        = nu_surface_color_target();

    g_renderpass = nu_renderpass_new(NU_RENDERPASS_FORWARD);
    nu_renderpass_set_shade(g_renderpass, NU_SHADE_WIREFRAME);
    nu_renderpass_set_camera(g_renderpass, g_camera);
    nu_renderpass_set_clear_color(g_renderpass, &clear_color);
    nu_renderpass_set_color_target(g_renderpass, g_surface_color);
    nu_renderpass_set_depth_target(g_renderpass, g_depth_buffer);

    g_guipass = nu_renderpass_new(NU_RENDERPASS_CANVAS);
    nu_renderpass_set_color_target(g_guipass, g_surface_color);

    g_font = nu_font_new_default();

    // Exit input
    g_exit_input = nu_input_new();
    nuext_input_bind_button(g_exit_input, NUEXT_BUTTON_ESCAPE);
}

void
update (void)
{
    float delta = nu_deltatime();
    g_global_time += delta;
    if (nu_input_just_pressed(g_exit_input))
    {
        nu_request_stop();
    }

    nu_m4_t model
        = nu_m4_translate(nu_v3(0, nu_sin(g_global_time / 500) * 0.1, 0));
    model = nu_m4_mul(model, nu_m4_rotate_y(g_global_time / 1000));

    nu_draw_box(
        g_renderpass, nu_b3(nu_v3s(-0.5), nu_v3s(0.5)), g_material, model);
    const nu_v3_t points[]
        = { nu_v3s(0.0), nu_v3s(0.1), nu_v3s(0.2), nu_v3s(0.3) };
    nu_draw_points(g_renderpass, points, 4, g_material, model);

    nu_draw_stats(g_guipass, g_font, nu_v2i(10, 10));

    nu_renderpass_submit(g_renderpass);
    nu_renderpass_submit(g_guipass);
}

void
nu_app (void)
{
    nu_app_surface_size(WIDTH, HEIGHT);
    nu_app_init_callback(init);
    nu_app_update_callback(update);
}
