#include <stdio.h>
#define NU_STDLIB
#define NU_DEBUG
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

#define WIDTH  640
#define HEIGHT 400
// #define WIDTH  640 / 2
// #define HEIGHT 400 / 2

static nu_allocator_t alloc;
static nu_context_t   ctx;

static nu_input_t draw;
static nu_input_t cursor_x;
static nu_input_t cursor_y;
static nu_input_t quit;

#define LOOP_TICK    0
#define LOOP_PHYSICS 1

int
main (void)
{
    nu_error_t error;

    // Create allocator
    nuext_init_stdlib_allocator(&alloc);

    // Create context
    nu_context_info_t cinfo;
    nu_context_info_default(&cinfo);
    cinfo.width     = WIDTH;
    cinfo.height    = HEIGHT;
    cinfo.renderer  = NU_RENDERER_GL;
    cinfo.allocator = alloc;
    error           = nu_init(&cinfo, &ctx);
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
    nu_vec3_t cube_positions[NU_CUBE_MESH_VERTEX_COUNT];
    nu_vec2_t cube_uvs[NU_CUBE_MESH_VERTEX_COUNT];
    nu_generate_cube_mesh(1.0f, cube_positions, cube_uvs);
    nu_mesh_t      cube_mesh;
    nu_mesh_info_t cube_info = { .positions = cube_positions,
                                 .uvs       = cube_uvs,
                                 .count     = NU_CUBE_MESH_VERTEX_COUNT };
    error                    = nu_create_mesh(&ctx, &cube_info, &cube_mesh);
    NU_ERROR_ASSERT(error);

    // Create camera
    nu_camera_t camera;
    error = nu_create_camera(&ctx, &camera);
    NU_ERROR_ASSERT(error);
    camera.eye    = nu_vec3(1, 0, 1);
    camera.center = NU_VEC3_ZERO;
    error         = nu_update_camera(&ctx, &camera);
    NU_ERROR_ASSERT(error);

    // Create renderpasses
    nu_renderpass_t main_pass;
    {
        nu_renderpass_info_t info;
        info.type      = NU_RENDERPASS_FLAT;
        info.flat.todo = 0;
        error          = nu_create_renderpass(&ctx, &info, &main_pass);
        NU_ERROR_ASSERT(error);
    }

    // Main loop
    nu_bool_t drawing = NU_FALSE;
    nu_bool_t running = NU_TRUE;

    nu_fixed_loop_t loops[2];
    loops[0] = nu_fixed_loop(LOOP_TICK, 1.0 / 60.0 * 1000.0);
    loops[1] = nu_fixed_loop(LOOP_PHYSICS, 1.0 / 20.0 * 1000.0);

    nu_timer_t timer;
    nu_timer_reset(&timer);
    float delta = 0.0f;

    while (!nu_exit_requested(&ctx) && running)
    {
        nu_update_fixed_loops(loops, 2, delta);
        nu_u32_t id;
        while (nu_next_fixed_loop(loops, 2, &id))
        {
            switch (id)
            {
                case LOOP_TICK:
                    printf("tick\n");
                    break;
                case LOOP_PHYSICS:
                    printf("physics\n");
                    break;
                default:
                    break;
            }
        }

        delta = nu_timer_elapsed(&timer);
        nu_timer_reset(&timer);

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
        nu_mat4_t model = nu_mat4_identity();
        nu_draw(&ctx, &main_pass, &cube_mesh, &model);
        model.x1 = 0.5;
        nu_draw(&ctx, &main_pass, &cube_mesh, &model);

        nu_renderpass_submit_t submit;
        submit.reset       = NU_TRUE;
        submit.flat.camera = &camera;
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
