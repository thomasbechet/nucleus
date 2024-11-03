#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

#define WIDTH  500
#define HEIGHT 500

static nu_renderpass_t renderpass;
static nu_texture_t    depth_buffer;
static nu_texture_t    texture;
static nu_material_t   material;
static nu_camera_t     camera;
static nu_input_t      exit_input;
static float           global_time;
static nu_renderpass_t guipass;
static nu_font_t       font;
static nu_texture_t    surface_color;

void
init (void)
{
    // Material
    texture  = nu_texture_create_color(NU_COLOR_WHITE);
    material = nu_material_create(NU_MATERIAL_SURFACE);
    nu_material_set_texture(material, texture);

    // Camera
    camera = nu_camera_create();
    nu_camera_set_proj(
        camera, nu_perspective(nu_radian(60), nu_surface_aspect(), 0.01, 100));
    nu_camera_set_view(camera,
                       nu_lookat(nu_v3(2, 1, 2), NU_V3_ZEROS, NU_V3_UP));

    // Renderpass
    depth_buffer
        = nu_texture_create(NU_TEXTURE_DEPTH_TARGET, nu_v3u(WIDTH, HEIGHT, 0));
    nu_color_t clear_color = NU_COLOR_BLACK;
    surface_color          = nu_surface_color_target();

    renderpass = nu_renderpass_create(NU_RENDERPASS_FORWARD);
    nu_renderpass_set_shade(renderpass, NU_SHADE_WIREFRAME);
    nu_renderpass_set_camera(renderpass, camera);
    nu_renderpass_set_clear_color(renderpass, &clear_color);
    nu_renderpass_set_color_target(renderpass, surface_color);
    nu_renderpass_set_depth_target(renderpass, depth_buffer);

    guipass = nu_renderpass_create(NU_RENDERPASS_CANVAS);
    nu_renderpass_set_color_target(guipass, surface_color);

    font = nu_font_create_default();

    // Exit input
    exit_input = nu_input_create();
    nuext_input_bind_button(exit_input, NUEXT_BUTTON_ESCAPE);
}

void
update (void)
{
    float delta = nu_deltatime();
    global_time += delta;
    if (nu_input_just_pressed(exit_input))
    {
        nu_request_stop();
    }

    nu_m4_t model
        = nu_m4_translate(nu_v3(0, nu_sin(global_time / 500) * 0.1, 0));
    model = nu_m4_mul(model, nu_m4_rotate_y(global_time / 1000));

    nu_draw_box(renderpass, nu_b3(nu_v3s(-0.5), nu_v3s(0.5)), material, model);
    const nu_v3_t points[]
        = { nu_v3s(0.0), nu_v3s(0.1), nu_v3s(0.2), nu_v3s(0.3) };
    nu_draw_points(renderpass, points, 4, material, model);

    nu_draw_stats(guipass, font, nu_v2i(10, 10));

    nu_renderpass_submit(renderpass);
    nu_renderpass_submit(guipass);
}

void
nu_app (void)
{
    nu_app_surface_size(WIDTH, HEIGHT);
    nu_app_init_callback(init);
    nu_app_update_callback(update);
}
