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
    nu_u32_t index0;
    nu_u32_t index1;
    float    distance;
} constraint_t;

typedef NU_VEC(constraint_t) constraint_vec_t;

typedef struct
{
    point_mass_vec_t pms;
    collision_vec_t  collisions;
    constraint_vec_t constraints;
    nu_vec3_t        global_force;
} context_t;

static context_t ctx;

static nu_u32_t
add_pm (nu_vec3_t pos, nu_vec3_t vel)
{
    point_mass_t *pm = NU_VEC_PUSH(&ctx.pms);
    pm->pos          = pos;
    pm->vel          = vel;
    pm->acc          = NU_VEC3_ZEROS;
    return ctx.pms.size - 1;
}
static void
add_constraint (nu_u32_t a, nu_u32_t b, float distance)
{
    constraint_t *c = NU_VEC_PUSH(&ctx.constraints);
    c->index0       = a;
    c->index1       = b;
    c->distance     = distance;
}
static void
shoot_context (nu_vec3_t pos, nu_vec3_t dir)
{
    ctx.global_force = nu_vec3(0, -9.81, 0);
    nu_vec3_t   vel  = nu_vec3_muls(dir, 10);
    const float s    = 1.0;
    const float h    = s / 2;
    const float s2   = nu_sqrt(s + s);
    const float s3   = nu_sqrt(s + s + s);

    nu_u32_t p0 = add_pm(nu_vec3_add(pos, nu_vec3(-h, h, h)), vel);
    nu_u32_t p1 = add_pm(nu_vec3_add(pos, nu_vec3(-h, h, -h)), vel);
    nu_u32_t p2 = add_pm(nu_vec3_add(pos, nu_vec3(h, h, -h)), vel);
    nu_u32_t p3 = add_pm(nu_vec3_add(pos, nu_vec3(h, h, h)), vel);
    nu_u32_t p4 = add_pm(nu_vec3_add(pos, nu_vec3(-h, -h, h)), vel);
    nu_u32_t p5 = add_pm(nu_vec3_add(pos, nu_vec3(-h, -h, -h)), vel);
    nu_u32_t p6 = add_pm(nu_vec3_add(pos, nu_vec3(h, -h, -h)), vel);
    nu_u32_t p7 = add_pm(nu_vec3_add(pos, nu_vec3(h, -h, h)), vel);

    add_constraint(p0, p1, s);
    add_constraint(p1, p2, s);
    add_constraint(p2, p3, s);
    add_constraint(p3, p0, s);

    add_constraint(p4, p5, s);
    add_constraint(p5, p6, s);
    add_constraint(p6, p7, s);
    add_constraint(p7, p4, s);

    add_constraint(p0, p4, s);
    add_constraint(p1, p5, s);
    add_constraint(p2, p6, s);
    add_constraint(p3, p7, s);

    add_constraint(p0, p2, s2);
    add_constraint(p1, p3, s2);

    add_constraint(p4, p6, s2);
    add_constraint(p5, p7, s2);

    add_constraint(p0, p7, s2);
    add_constraint(p4, p3, s2);

    add_constraint(p5, p2, s2);
    add_constraint(p1, p6, s2);

    add_constraint(p4, p1, s2);
    add_constraint(p0, p5, s2);

    add_constraint(p3, p6, s2);
    add_constraint(p7, p2, s2);

    add_constraint(p4, p2, s3);
    add_constraint(p0, p6, s3);
    add_constraint(p7, p1, s3);
    add_constraint(p3, p5, s3);
}
static void
init_context (void)
{
    NU_VEC_INIT(100, &ctx.pms);
    NU_VEC_INIT(100, &ctx.collisions);
    NU_VEC_INIT(100, &ctx.constraints);
}
static void
free_context (void)
{
    NU_VEC_FREE(&ctx.constraints);
    NU_VEC_FREE(&ctx.collisions);
    NU_VEC_FREE(&ctx.pms);
}
static nu_vec3_t
compute_force (point_mass_t *pm)
{
    return ctx.global_force;
}

static collision_t
find_collision (nu_vec3_t position)
{
    collision_t c;
    c.depth  = -position.y;
    c.normal = NU_VEC3_UP;
    return c;
}
static void
update_context (float dt)
{
    dt *= 0.001;
    // Verlet integration
    for (nu_size_t i = 0; i < ctx.pms.size; ++i)
    {
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

        // pm->vel = nu_vec3_add(pm->vel, nu_vec3_muls(compute_force(pm), dt));
        // pm->pos = nu_vec3_add(pm->pos, nu_vec3_muls(pm->vel, dt));
    }

    // Collision resolution
    for (nu_size_t i = 0; i < ctx.pms.size; ++i)
    {
        point_mass_t *pm         = ctx.pms.data + i;
        const float   elasticity = 0;
        const float   friction   = 100;

        collision_t c = find_collision(pm->pos);
        if (c.depth < 0)
        {
            continue;
        }
        pm->pos      = nu_vec3_add(pm->pos, nu_vec3_muls(c.normal, c.depth));
        nu_vec3_t vn = nu_vec3_muls(c.normal, nu_vec3_dot(c.normal, pm->vel));
        nu_vec3_t vt = nu_vec3_sub(pm->vel, vn);
        vn           = nu_vec3_muls(vn, -elasticity);
        vt           = nu_vec3_muls(vt, nu_exp(-friction * dt));
        pm->vel      = nu_vec3_add(vn, vt);
    }

    // Resolve constraints
    for (nu_size_t n = 0; n < 10; ++n)
    {
        for (nu_size_t i = 0; i < ctx.constraints.size; ++i)
        {
            constraint_t *c  = ctx.constraints.data + i;
            nu_vec3_t     p0 = ctx.pms.data[c->index0].pos;
            nu_vec3_t     p1 = ctx.pms.data[c->index1].pos;

            nu_vec3_t delta    = nu_vec3_sub(p1, p0);
            float     distance = nu_vec3_norm(delta);

            nu_vec3_t required_delta
                = nu_vec3_muls(delta, c->distance / distance);
            // nu_vec3_t force = nu_vec3_muls(nu_vec3_sub(required_delta,
            // delta), 50);

            // ctx.pms.data[c->index0].vel
            //     = nu_vec3_sub(ctx.pms.data[c->index0].vel,
            //     nu_vec3_muls(force, dt));
            // ctx.pms.data[c->index1].vel
            //     = nu_vec3_add(ctx.pms.data[c->index1].vel,
            //     nu_vec3_muls(force, dt));

            // float     damping = 1.0 - nu_exp(-500 * dt);
            // nu_vec3_t offset
            //     = nu_vec3_muls(nu_vec3_sub(required_delta, delta), damping);
            nu_vec3_t offset = nu_vec3_sub(required_delta, delta);

            ctx.pms.data[c->index0].pos
                = nu_vec3_sub(p0, nu_vec3_muls(offset, 0.5));
            ctx.pms.data[c->index1].pos
                = nu_vec3_add(p1, nu_vec3_muls(offset, 0.5));
        }
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
    nu_input_t shoot       = nu_input_create();

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
    nuext_input_bind_button(shoot, NUEXT_BUTTON_R);

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

    nu_material_t red_mat = nu_material_create(NU_MATERIAL_TYPE_SURFACE);
    nu_material_color(red_mat, NU_MATERIAL_COLOR, NU_COLOR_RED);

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

    nu_fixed_loop_t loop = nu_fixed_loop(0, 1.0 / 60.0 * 1000.0);

    while (!nu_exit_requested())
    {
        // Poll events
        nu_poll_events();

        if (nu_input_just_pressed(shoot))
        {
            nu_mat4_t transform = nu_controller_transform(controller);
            nu_vec3_t pos       = nu_mat4_mulv3(transform, NU_VEC3_ZEROS);
            nu_vec4_t dir0      = nu_mat4_mulv(transform, nu_vec4(0, 0, -1, 0));
            nu_vec3_t dir       = nu_vec3(dir0.x, dir0.y, dir0.z);
            shoot_context(pos, dir);
        }

        update_context(delta);
        nu_fixed_loop_update(&loop, 1, delta);
        nu_u32_t id;
        while (nu_fixed_loop_next(&loop, 1, &id))
        {
            if (id == 0)
            {
                update_context(0.5);
            }
        }

        delta = nu_timer_elapsed(&timer);
        nu_timer_reset(&timer);

        nu_bind_material(wireframe_pass, red_mat);
        for (nu_size_t i = 0; i < ctx.pms.size; ++i)
        {
            point_mass_t *pm = ctx.pms.data + i;
            nu_draw_mesh(wireframe_pass, cube, nu_mat4_translate(pm->pos));
        }
        nu_bind_material(wireframe_pass, NU_NULL);
        for (nu_size_t i = 0; i < ctx.constraints.size; ++i)
        {
            nu_vec3_t p0 = ctx.pms.data[ctx.constraints.data[i].index0].pos;
            nu_vec3_t p1 = ctx.pms.data[ctx.constraints.data[i].index1].pos;
            const nu_vec3_t points[] = { p0, p1 };
            nu_draw_lines(wireframe_pass, points, 1, nu_mat4_identity());
        }

        // Update camera controller
        nu_controller_update(controller, delta, camera);

        nu_bind_material(wireframe_pass, NU_NULL);
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
