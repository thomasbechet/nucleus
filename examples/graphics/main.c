#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

#define WIDTH  500
#define HEIGHT 500

static nu_renderpass_t renderpass;
static nu_texture_t    depth_buffer;
static nu_mesh_t       mesh;
static nu_texture_t    texture;
static nu_material_t   material;
static nu_camera_t     camera;
static nu_input_t      exit_input;

int
main (void)
{
    nu_config_surface_size(WIDTH, HEIGHT);
    nu_init();

    // Renderpass
    renderpass = nu_renderpass_create(NU_RENDERPASS_TYPE_WIREFRAME, NU_TRUE);

    // Mesh
    {
        nu_geometry_t g = nu_geometry_create(NU_PRIMITIVE_TRIANGLES, 100);
        nu_geometry_cube(g, 1);
        nu_geometry_transform(g, nu_mat4_translate(nu_vec3s(-0.5)));
        mesh = nu_mesh_create_geometry(g);
        nu_geometry_delete(g);
    }

    // Texture
    {
        texture = nu_texture_create_color(NU_COLOR_WHITE);
    }

    // Material
    material = nu_material_create(NU_MATERIAL_TYPE_SURFACE);
    nu_material_texture(material, NU_MATERIAL_TEXTURE0, texture);

    // Camera
    camera = nu_camera_create();
    nu_camera_proj(
        camera, nu_perspective(nu_radian(60), nu_surface_aspect(), 0.01, 100));
    nu_camera_view(camera,
                   nu_lookat(nu_vec3(2, 1, 2), NU_VEC3_ZERO, NU_VEC3_UP));

    // Depth buffer
    depth_buffer = nu_texture_create(nu_vec2u(WIDTH, HEIGHT),
                                     NU_TEXTURE_FORMAT_DEPTH,
                                     NU_TEXTURE_USAGE_TARGET,
                                     NU_NULL);

    // Exit input
    exit_input = nu_input_create();
    nuext_input_bind_button(exit_input, NUEXT_BUTTON_ESCAPE);

    nu_timer_t timer;
    nu_timer_reset(&timer);
    float time = 0;
    while (!nu_exit_requested())
    {
        float delta = nu_timer_elapsed(&timer);
        nu_timer_reset(&timer);
        time += delta;
        if (nu_input_just_pressed(exit_input))
        {
            break;
        }
        nu_poll_events();

        nu_mat4_t model
            = nu_mat4_translate(nu_vec3(0, nu_sin(time / 500) * 0.1, 0));
        model = nu_mat4_mul(model, nu_mat4_rotate_y(time / 1000));
        nu_bind_material(renderpass, material);
        nu_draw_mesh(renderpass, mesh, model);

        nu_color_t   clear_color   = NU_COLOR_BLACK;
        nu_texture_t surface_color = nu_surface_color_target();

        nu_renderpass_camera(renderpass, NU_RENDERPASS_CAMERA, camera);
        nu_renderpass_color(
            renderpass, NU_RENDERPASS_CLEAR_COLOR, &clear_color);
        nu_renderpass_texture(
            renderpass, NU_RENDERPASS_COLOR_TARGET, surface_color);
        nu_renderpass_texture(
            renderpass, NU_RENDERPASS_DEPTH_TARGET, depth_buffer);
        nu_renderpass_submit(renderpass);

        nu_swap_buffers();
    }

    nu_terminate();

    return 0;
}
