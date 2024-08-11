#include <stdio.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/utils.h>
#include <nucleus/graphics.h>

#define WIDTH  500
#define HEIGHT 500

static nu_allocator_t         alloc;
static nu_logger_t            logger;
static nu_platform_t          platform;
static nu_renderer_t          renderer;
static nu_renderpass_handle_t renderpass;
static nu_texture_handle_t    depth_buffer;
static nu_mesh_handle_t       mesh;
static nu_texture_handle_t    texture;
static nu_material_handle_t   material;
static nu_camera_handle_t     camera;
static nu_input_t             exit_input;

int
main (void)
{
    nu_error_t error;
    NU_UNUSED(error);

    nuext_stdlib_allocator_init(&alloc);

    // Platform
    {
        nu_platform_info_t info;
        info.width     = WIDTH;
        info.height    = HEIGHT;
        info.allocator = alloc;
        error          = nu_platform_init(&info, &platform);
        NU_ERROR_ASSERT(error);
    }

    // Renderer
    {
        nu_renderer_info_t info;
        info.api = NU_RENDERER_GL;
        error    = nu_renderer_init(&platform, &info, &renderer);
        NU_ERROR_ASSERT(error);
    }

    // Renderpass
    {
        nu_renderpass_info_t info;
        info.type = NU_RENDERPASS_FLAT;
        error     = nu_renderpass_create(&renderer, &info, &renderpass);
        NU_ERROR_ASSERT(error);
    }

    // Mesh
    {
        nu_vec3_t positions[NU_CUBE_MESH_VERTEX_COUNT];
        nu_vec2_t uvs[NU_CUBE_MESH_VERTEX_COUNT];
        nu_vec3_t normals[NU_CUBE_MESH_VERTEX_COUNT];
        nu_generate_cube_mesh(1, positions, uvs, normals);
        nu_mesh_info_t info;
        info.positions = positions;
        info.uvs       = uvs;
        info.normals   = normals;
        info.count     = NU_CUBE_MESH_VERTEX_COUNT;
        error          = nu_mesh_create(&renderer, &info, &mesh);
        NU_ERROR_ASSERT(error);
    }

    // Texture
    {
        error = nu_texture_create_color(&renderer, NU_COLOR_WHITE, &texture);
        NU_ERROR_ASSERT(error);
    }

    // Material
    {
        nu_material_info_t info = nu_material_info_default();
        info.texture0           = &texture;
        error = nu_material_create(&renderer, &info, &material);
        NU_ERROR_ASSERT(error);
    }

    // Camera
    {
        nu_camera_info_t info = nu_camera_info_default();
        info.fov              = 60;
        info.eye              = nu_vec3(2, 1, 2);
        info.center           = NU_VEC3_ZERO;
        error                 = nu_camera_create(&renderer, &info, &camera);
        NU_ERROR_ASSERT(error);
    }

    // Depth buffer
    {
        nu_texture_info_t info;
        info.size   = nu_uvec2(WIDTH, HEIGHT);
        info.usage  = NU_TEXTURE_USAGE_TARGET;
        info.format = NU_TEXTURE_FORMAT_DEPTH;
        error       = nu_texture_create(&renderer, &info, &depth_buffer);
        NU_ERROR_ASSERT(error);
    }

    // Exit input
    {
        error = nu_input_create(&platform, &exit_input);
        NU_ERROR_ASSERT(error);
        nuext_input_bind_button(&platform, &exit_input, NUEXT_BUTTON_ESCAPE);
    }

    nu_timer_t timer;
    nu_timer_reset(&timer);
    float time = 0;
    while (!nu_exit_requested(&platform))
    {
        float delta = nu_timer_elapsed(&timer);
        nu_timer_reset(&timer);
        time += delta;
        if (nu_input_just_pressed(&platform, &exit_input))
        {
            break;
        }
        nu_poll_events(&platform);

        nu_mat4_t model = nu_mat4_identity();
        model           = nu_mat4_mul(model, nu_mat4_rotate_y(time / 1000));
        nu_draw(&renderer, renderpass, mesh, material, model);

        nu_renderpass_submit_t submit;
        submit.reset            = NU_TRUE;
        submit.flat.camera      = camera;
        submit.flat.clear_color = NU_COLOR_BLACK;
        nu_texture_handle_t surface_color
            = nu_surface_color_target(&platform, &renderer);
        submit.flat.color_target = &surface_color;
        submit.flat.depth_target = &depth_buffer;
        nu_renderpass_submit(&renderer, renderpass, &submit);

        nu_render(&platform, &renderer);
        nu_swap_buffers(&platform);
    }

    nu_renderer_terminate(&renderer);
    nu_platform_terminate(&platform);

    return 0;
}
