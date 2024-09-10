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
    nu_init();

    // Renderpass
    {
        nu_renderpass_info_t info;
        info.type               = NU_RENDERPASS_FLAT;
        info.reset_after_submit = NU_TRUE;
        renderpass              = nu_renderpass_create(&info);
    }

    // Mesh
    {
        nu_geometry_t g = nu_geometry_create(NU_CUBE_VERTEX_COUNT);
        nu_geometry_cube(g, 1);
        mesh = nu_mesh_create_geometry(g);
        nu_geometry_delete(g);
    }

    // Texture
    {
        texture = nu_texture_create_color(NU_COLOR_WHITE);
    }

    // Material
    {
        nu_material_info_t info = nu_material_info_default(NU_MATERIAL_MESH);
        info.mesh.color0        = &texture;
        material                = nu_material_create(&info);
    }

    // Camera
    {
        nu_camera_info_t info = nu_camera_info();
        info.fov              = 60;
        info.eye              = nu_vec3(2, 1, 2);
        info.center           = NU_VEC3_ZERO;
        camera                = nu_camera_create(&info);
    }

    // Depth buffer
    {
        nu_texture_info_t info;
        info.size    = nu_uvec2(WIDTH, HEIGHT);
        info.usage   = NU_TEXTURE_USAGE_TARGET;
        info.format  = NU_TEXTURE_FORMAT_DEPTH;
        depth_buffer = nu_texture_create(&info);
    }

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

        nu_mat4_t model = nu_mat4_translate(0, nu_sin(time / 500) * 0.1, 0);
        model           = nu_mat4_mul(model, nu_mat4_rotate_y(time / 1000));
        nu_draw_mesh(renderpass, material, mesh, model);

        nu_color_t   clear_color   = NU_COLOR_BLACK;
        nu_texture_t surface_color = nu_surface_color_target();

        nu_renderpass_submit_t submit;
        submit.flat.camera       = camera;
        submit.flat.clear_color  = &clear_color;
        submit.flat.color_target = &surface_color;
        submit.flat.depth_target = &depth_buffer;
        nu_renderpass_submit(renderpass, &submit);

        nu_swap_buffers();
    }

    nu_terminate();

    return 0;
}
