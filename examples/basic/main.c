#include <stdio.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

#define WIDTH  640
#define HEIGHT 400
// #define WIDTH  640 / 2
// #define HEIGHT 400 / 2

static nu_allocator_t alloc;
static nu_context_t   ctx;
static nu_logger_t    logger;

static nu_input_t draw;
static nu_input_t move_forward;
static nu_input_t move_backward;
static nu_input_t move_left;
static nu_input_t move_right;
static nu_input_t move_up;
static nu_input_t move_down;
static nu_input_t view_pitch_neg;
static nu_input_t view_pitch_pos;
static nu_input_t view_yaw_neg;
static nu_input_t view_yaw_pos;
static nu_input_t view_roll_neg;
static nu_input_t view_roll_pos;
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
    error           = nu_init_context(&cinfo, &ctx);
    NU_ERROR_ASSERT(error);

    // Create logger
    error = nu_create_logger(&ctx, &logger);
    NU_ERROR_ASSERT(error);

    // Configure inputs
    error &= nu_create_input(&ctx, &draw);
    error &= nu_create_input(&ctx, &quit);
    error &= nu_create_input(&ctx, &move_forward);
    error &= nu_create_input(&ctx, &move_backward);
    error &= nu_create_input(&ctx, &move_left);
    error &= nu_create_input(&ctx, &move_right);
    error &= nu_create_input(&ctx, &move_up);
    error &= nu_create_input(&ctx, &move_down);
    error &= nu_create_input(&ctx, &view_pitch_neg);
    error &= nu_create_input(&ctx, &view_pitch_pos);
    error &= nu_create_input(&ctx, &view_yaw_neg);
    error &= nu_create_input(&ctx, &view_yaw_pos);
    error &= nu_create_input(&ctx, &view_roll_neg);
    error &= nu_create_input(&ctx, &view_roll_pos);
    NU_ERROR_ASSERT(error);

    // Bind inputs
    error &= nuext_bind_button(&ctx, &quit, NUEXT_BUTTON_ESCAPE);
    error &= nuext_bind_button(&ctx, &move_forward, NUEXT_BUTTON_W);
    error &= nuext_bind_button(&ctx, &move_backward, NUEXT_BUTTON_S);
    error &= nuext_bind_button(&ctx, &move_left, NUEXT_BUTTON_A);
    error &= nuext_bind_button(&ctx, &move_right, NUEXT_BUTTON_D);
    error &= nuext_bind_button(&ctx, &move_up, NUEXT_BUTTON_X);
    error &= nuext_bind_button(&ctx, &move_down, NUEXT_BUTTON_Z);
    // error &= nuext_bind_axis(&ctx, &view_x, NUEXT_AXIS_MOUSE_MOTION_X);
    // error &= nuext_bind_axis(&ctx, &view_y, NUEXT_AXIS_MOUSE_MOTION_Y);
    error &= nuext_bind_axis(
        &ctx, &view_pitch_neg, NUEXT_AXIS_MOUSE_MOTION_Y_NEG);
    error &= nuext_bind_axis(
        &ctx, &view_pitch_pos, NUEXT_AXIS_MOUSE_MOTION_Y_POS);
    error
        &= nuext_bind_axis(&ctx, &view_yaw_neg, NUEXT_AXIS_MOUSE_MOTION_X_NEG);
    error
        &= nuext_bind_axis(&ctx, &view_yaw_pos, NUEXT_AXIS_MOUSE_MOTION_X_POS);
    error
        &= nuext_bind_button_value(&ctx, &view_pitch_neg, NUEXT_BUTTON_K, 0.08);
    error
        &= nuext_bind_button_value(&ctx, &view_pitch_pos, NUEXT_BUTTON_J, 0.08);
    error &= nuext_bind_button_value(&ctx, &view_yaw_neg, NUEXT_BUTTON_H, 0.08);
    error &= nuext_bind_button_value(&ctx, &view_yaw_pos, NUEXT_BUTTON_L, 0.08);
    error
        &= nuext_bind_button_value(&ctx, &view_roll_neg, NUEXT_BUTTON_E, 0.08);
    error
        &= nuext_bind_button_value(&ctx, &view_roll_pos, NUEXT_BUTTON_Q, 0.08);
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

    nu_camera_controller_t controller;
    nu_init_camera_controller(&controller);

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
                    break;
                case LOOP_PHYSICS:
                    break;
                default:
                    break;
            }
        }

        delta = nu_timer_elapsed(&timer);
        nu_timer_reset(&timer);

        // Poll events
        nu_poll_events(&ctx);

        controller.view_pitch_neg = nu_input_value(&ctx, &view_pitch_neg);
        controller.view_pitch_pos = nu_input_value(&ctx, &view_pitch_pos);
        controller.view_yaw_neg   = nu_input_value(&ctx, &view_yaw_neg);
        controller.view_yaw_pos   = nu_input_value(&ctx, &view_yaw_pos);
        controller.view_roll_neg  = nu_input_value(&ctx, &view_roll_neg);
        controller.view_roll_pos  = nu_input_value(&ctx, &view_roll_pos);
        controller.move_forward   = nu_input_value(&ctx, &move_forward);
        controller.move_backward  = nu_input_value(&ctx, &move_backward);
        controller.move_left      = nu_input_value(&ctx, &move_left);
        controller.move_right     = nu_input_value(&ctx, &move_right);
        controller.move_up        = nu_input_value(&ctx, &move_up);
        controller.move_down      = nu_input_value(&ctx, &move_down);
        nu_update_camera_controller(&controller, delta, &camera);
        nu_update_camera(&ctx, &camera);

        if (nu_input_just_pressed(&ctx, &quit))
        {
            running = NU_FALSE;
        }

        // Render loop
        nu_mat4_t model = nu_mat4_identity();
        nu_draw(&ctx, &main_pass, &cube_mesh, &model);
        model = nu_mat4_translate(4, 0, 0);
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

    nu_free_context(&ctx, &alloc);

    return 0;
}
