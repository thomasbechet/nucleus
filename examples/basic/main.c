#include <stdio.h>
#define NU_STDLIB
#define NU_DEBUG
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

// #define WIDTH  640
// #define HEIGHT 400
#define WIDTH  640 / 2
#define HEIGHT 400 / 2

static nu_allocator_t alloc;
static nu_context_t   ctx;

static nu_input_t draw;
static nu_input_t cursor_x;
static nu_input_t cursor_y;
static nu_input_t quit;

int
main (void)
{
    nu_error_t error;

    // Create allocator
    nuext_init_stdlib_allocator(&alloc);

    // Create context
    nu_context_info_t cinfo;
    nu_context_info_default(&cinfo);
    cinfo.width    = WIDTH;
    cinfo.height   = HEIGHT;
    cinfo.renderer = NU_RENDERER_GL;
    error          = nu_init(&cinfo, &alloc, &ctx);
    NU_ERROR_ASSERT(error);

    // Configure inputs
    error = nu_create_input(&ctx, &draw);
    NU_ERROR_ASSERT(error);
    error = nu_create_input(&ctx, &cursor_x);
    NU_ERROR_ASSERT(error);
    error = nu_create_input(&ctx, &cursor_y);
    NU_ERROR_ASSERT(error);
    error = nu_create_input(&ctx, &quit);
    NU_ERROR_ASSERT(error);

    // Bind inputs
    error = nuext_bind_button(&ctx, &draw, NUEXT_BUTTON_W);
    NU_ERROR_ASSERT(error);
    error = nuext_bind_button(&ctx, &draw, NUEXT_BUTTON_MOUSE_LEFT);
    NU_ERROR_ASSERT(error);
    error = nuext_bind_button(&ctx, &quit, NUEXT_BUTTON_ESCAPE);
    NU_ERROR_ASSERT(error);

    // Create cube
    float cube_positions[NU_CUBE_MESH_POSITION_SIZE];
    float cube_uvs[NU_CUBE_MESH_UV_SIZE];
    nu_generate_cube_mesh(1.0f, cube_positions, cube_uvs);
    nu_mesh_t      cube_mesh;
    nu_mesh_info_t cube_info = { .positions    = cube_positions,
                                 .uvs          = cube_uvs,
                                 .vertex_count = NU_CUBE_MESH_VERTEX_SIZE };
    error                    = nu_create_mesh(&ctx, &cube_info, &cube_mesh);
    NU_ERROR_ASSERT(error);

    // Create camera
    nu_camera_t camera;
    error = nu_create_camera(&ctx, &camera);
    NU_ERROR_ASSERT(error);
    float eye[NU_V3]    = { 1.0f, 0.0f, 1.0f };
    float center[NU_V3] = { 0.0f, 0.0f, 0.0f };
    nu_v3_copy(eye, camera.eye);
    nu_v3_copy(center, camera.center);
    error = nu_update_camera(&ctx, &camera);
    NU_ERROR_ASSERT(error);

    ctx._renderer.backend.gl.mesh = &cube_mesh.gl;    // TODO: remove me
    ctx._renderer.backend.gl.cam  = &camera._data.gl; // TODO: remove me

    // Create renderpasses
    nu_renderpass_t main_pass;
    {
        nu_renderpass_info_t info;
        info.type = NU_RENDERPASS_FLAT;
        error     = nu_create_renderpass(&ctx, &info, &main_pass);
        NU_ERROR_ASSERT(error);
    }

    // Main loop
    nu_bool_t drawing = NU_FALSE;
    nu_bool_t running = NU_TRUE;

    while (!nu_exit_requested(&ctx) && running)
    {
        // Poll events
        nu_poll_events(&ctx);

        // Detect drawing
        if (nu_input_changed(&ctx, &cursor_x)
            || nu_input_changed(&ctx, &cursor_y))
        {
        }

        if (nu_input_pressed(&ctx, &draw))
        {
            printf("pressed\n");
        }
        if (nu_input_just_pressed(&ctx, &draw))
        {
            printf("just pressed\n");
        }
        if (nu_input_just_released(&ctx, &draw))
        {
            printf("just released\n");
        }

        if (nu_input_just_pressed(&ctx, &quit))
        {
            running = NU_FALSE;
        }

        // Render loop
        float model[NU_M4];
        nu_m4_identity(model);
        nu_draw(&ctx, &main_pass, &cube_mesh, model);

        nu_renderpass_submit_t submit;
        nu_submit_renderpass(&ctx, &main_pass, &submit);

        // Refresh surface
        nu_render(&ctx);
    }

    // Free cube
    error = nu_delete_mesh(&ctx, &cube_mesh);
    NU_ERROR_ASSERT(error);
    error = nu_delete_renderpass(&ctx, &main_pass);
    NU_ERROR_ASSERT(error);

    nu_terminate(&ctx, &alloc);

    return 0;
}
