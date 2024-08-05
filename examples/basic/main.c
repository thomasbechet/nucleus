#include <stdio.h>
#define NU_STDLIB
#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>
#define STB_IMAGE_IMPLEMENTATION
#include <nucleus/external/stb/stb_image.h>

#define WIDTH  640
#define HEIGHT 400
// #define HEIGHT 640
// #define WIDTH  400
// #define WIDTH  640 / 2
// #define HEIGHT 400 / 2
// #define HEIGHT 640 / 2
// #define WIDTH  400 / 2

// #define WIDTH  512
// #define HEIGHT 288

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
    nuext_stdlib_allocator_init(&alloc);

    // Create context
    nu_context_info_t cinfo = nu_context_info_default();
    cinfo.width             = WIDTH;
    cinfo.height            = HEIGHT;
    cinfo.renderer          = NU_RENDERER_GL;
    cinfo.allocator         = alloc;
    error                   = nu_context_init(&cinfo, &ctx);
    NU_ERROR_ASSERT(error);

    // Create logger
    error = nu_create_logger(&ctx, &logger);
    NU_ERROR_ASSERT(error);

    // Configure inputs
    error &= nu_input_create(&ctx, &draw);
    error &= nu_input_create(&ctx, &quit);
    error &= nu_input_create(&ctx, &move_forward);
    error &= nu_input_create(&ctx, &move_backward);
    error &= nu_input_create(&ctx, &move_left);
    error &= nu_input_create(&ctx, &move_right);
    error &= nu_input_create(&ctx, &move_up);
    error &= nu_input_create(&ctx, &move_down);
    error &= nu_input_create(&ctx, &view_pitch_neg);
    error &= nu_input_create(&ctx, &view_pitch_pos);
    error &= nu_input_create(&ctx, &view_yaw_neg);
    error &= nu_input_create(&ctx, &view_yaw_pos);
    error &= nu_input_create(&ctx, &view_roll_neg);
    error &= nu_input_create(&ctx, &view_roll_pos);
    NU_ERROR_ASSERT(error);

    // Bind inputs
    error &= nuext_input_bind_button(&ctx, &quit, NUEXT_BUTTON_ESCAPE);
    error &= nuext_input_bind_button(&ctx, &move_forward, NUEXT_BUTTON_W);
    error &= nuext_input_bind_button(&ctx, &move_backward, NUEXT_BUTTON_S);
    error &= nuext_input_bind_button(&ctx, &move_left, NUEXT_BUTTON_A);
    error &= nuext_input_bind_button(&ctx, &move_right, NUEXT_BUTTON_D);
    error &= nuext_input_bind_button(&ctx, &move_up, NUEXT_BUTTON_X);
    error &= nuext_input_bind_button(&ctx, &move_down, NUEXT_BUTTON_Z);
    error &= nuext_input_bind_axis(
        &ctx, &view_pitch_neg, NUEXT_AXIS_MOUSE_MOTION_Y_NEG);
    error &= nuext_input_bind_axis(
        &ctx, &view_pitch_pos, NUEXT_AXIS_MOUSE_MOTION_Y_POS);
    error &= nuext_input_bind_axis(
        &ctx, &view_yaw_neg, NUEXT_AXIS_MOUSE_MOTION_X_NEG);
    error &= nuext_input_bind_axis(
        &ctx, &view_yaw_pos, NUEXT_AXIS_MOUSE_MOTION_X_POS);
    error &= nuext_input_bind_button_value(
        &ctx, &view_pitch_neg, NUEXT_BUTTON_K, 0.08);
    error &= nuext_input_bind_button_value(
        &ctx, &view_pitch_pos, NUEXT_BUTTON_J, 0.08);
    error &= nuext_input_bind_button_value(
        &ctx, &view_yaw_neg, NUEXT_BUTTON_H, 0.08);
    error &= nuext_input_bind_button_value(
        &ctx, &view_yaw_pos, NUEXT_BUTTON_L, 0.08);
    error &= nuext_input_bind_button_value(
        &ctx, &view_roll_neg, NUEXT_BUTTON_E, 0.08);
    error &= nuext_input_bind_button_value(
        &ctx, &view_roll_pos, NUEXT_BUTTON_Q, 0.08);
    NU_ERROR_ASSERT(error);

    // Create depth texture
    nu_texture_t depth_target;
    {
        nu_texture_info_t info;
        info.usage  = NU_TEXTURE_USAGE_TARGET;
        info.format = NU_TEXTURE_FORMAT_DEPTH;
        info.size   = nu_uvec2(WIDTH, HEIGHT);
        error       = nu_texture_create(&ctx, &info, &depth_target);
        NU_ERROR_ASSERT(error);
    }

    // Create cube
    nu_vec3_t cube_positions[NU_CUBE_MESH_VERTEX_COUNT];
    nu_vec2_t cube_uvs[NU_CUBE_MESH_VERTEX_COUNT];
    nu_generate_cube_mesh(1.0f, cube_positions, cube_uvs);
    nu_mesh_t      cube_mesh;
    nu_mesh_info_t cube_info = { .positions = cube_positions,
                                 .uvs       = cube_uvs,
                                 .count     = NU_CUBE_MESH_VERTEX_COUNT };
    error                    = nu_mesh_create(&ctx, &cube_info, &cube_mesh);
    NU_ERROR_ASSERT(error);

    // Create material
    nu_texture_t  texture;
    nu_material_t material;
    {
        int            width, height, channels;
        unsigned char *img
            = stbi_load("../../../assets/brick_building_front_lowres.png",
                        &width,
                        &height,
                        &channels,
                        3);
        if (img == NULL)
        {
            NU_ERROR(&logger, "error in loading the image");
            exit(1);
        }

        nu_color_t *colors
            = nu_alloc(alloc, sizeof(nu_color_t) * width * height);
        for (int i = 0; i < (width * height); ++i)
        {
            colors[i] = NU_COLOR_RED;

            colors[i].r = img[i * 3 + 0];
            colors[i].g = img[i * 3 + 1];
            colors[i].b = img[i * 3 + 2];
            colors[i].a = 0;
        }

        NU_INFO(&logger, "%d %d %d", channels, width, height);

        nu_texture_info_t tinfo;
        tinfo.size.x = width;
        tinfo.size.y = height;
        tinfo.usage  = NU_TEXTURE_USAGE_SAMPLE;
        tinfo.format = NU_TEXTURE_FORMAT_COLOR;
        error        = nu_texture_create(&ctx, &tinfo, &texture);
        NU_ERROR_ASSERT(error);
        error = nu_texture_write(
            &ctx, &texture, nu_rect(0, 0, width, height), colors);
        NU_ERROR_ASSERT(error);

        stbi_image_free(img);
        nu_free(alloc, colors, sizeof(nu_color_t) * width * height);
    }
    {
        nu_material_info_t info = nu_material_info_default();
        info.texture0           = &texture;
        info.uv_transform       = nu_mat3_scale(0.5, 0.5);
        error                   = nu_material_create(&ctx, &info, &material);
        NU_ERROR_ASSERT(error);
    }

    // Create camera
    nu_camera_t      camera;
    nu_camera_info_t cam_info = nu_camera_info_default();
    error                     = nu_camera_create(&ctx, &cam_info, &camera);
    NU_ERROR_ASSERT(error);

    nu_camera_controller_t controller;
    nu_camera_controller_init(&controller);

    // Create renderpasses
    nu_renderpass_t main_pass;
    {
        nu_renderpass_info_t info;
        info.type      = NU_RENDERPASS_FLAT;
        info.flat.todo = 0;
        error          = nu_renderpass_create(&ctx, &info, &main_pass);
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
        nu_camera_controller_update(&controller, delta, &cam_info);
        nu_camera_update(&ctx, &camera, &cam_info);

        if (nu_input_just_pressed(&ctx, &quit))
        {
            running = NU_FALSE;
        }

        // Render loop
        for (int i = 0; i < 20; ++i)
        {
            nu_mat4_t model = nu_mat4_translate(i, i, i);
            nu_draw(&ctx, &main_pass, &cube_mesh, &material, &model);
        }

        nu_renderpass_submit_t submit;
        submit.reset             = NU_TRUE;
        submit.flat.camera       = &camera;
        submit.flat.color_target = nu_surface_color_target(&ctx);
        submit.flat.depth_target = &depth_target;
        submit.flat.clear_color  = NU_COLOR_WHITE;
        nu_renderpass_submit(&ctx, &main_pass, &submit);

        // Refresh surface
        nu_render(&ctx);
    }

    // Free cube
    error = nu_mesh_delete(&ctx, &cube_mesh);
    NU_ERROR_ASSERT(error);
    error = nu_renderpass_delete(&ctx, &main_pass);
    NU_ERROR_ASSERT(error);

    nu_context_free(&ctx);

    return 0;
}
