#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

// #define WIDTH  720
// #define HEIGHT 480
#define WIDTH  640
#define HEIGHT 400

typedef struct
{
    nu_v3_t x; // position
    nu_v3_t v; // velocity
    nu_v3_t f; // force
    float   m; // mass
    float   w; // inv mass
    nu_v3_t p; // previous position
} point_mass_t;

typedef struct
{
    nu_u32_t a; // index of point mass
    nu_v3_t  q; // position of collision
    nu_v3_t  n; // normal
} collision_constraint_t;

typedef struct
{
    nu_u32_t a; // index of first point mass
    nu_u32_t b; // index of second point mass
    float    d; // distance
} distance_constraint_t;

typedef NU_VEC(point_mass_t) point_mass_vec_t;
typedef NU_VEC(collision_constraint_t) collision_constraint_vec_t;
typedef NU_VEC(distance_constraint_t) distance_constraint_vec_t;

typedef struct
{
    point_mass_vec_t           point_masses;
    collision_constraint_vec_t collision_constraints;
    distance_constraint_vec_t  distance_constraints;
} context_t;

#define BOX0 nu_b3(nu_v3(0, 0, 0), nu_v3(5, 2, 5))
#define BOX1 nu_b3(nu_v3(-5, 0, -5), nu_v3(-1, 6, -1))
static nu_b3_t boxes[2];

static context_t ctx;

static nu_u32_t
add_pm (nu_v3_t pos, nu_v3_t vel)
{
    point_mass_t *pm = NU_VEC_PUSH(&ctx.point_masses);
    pm->x            = pos;
    pm->v            = vel;
    pm->f            = NU_VEC3_ZEROS;
    pm->m            = 1;
    pm->w            = 1.0 / pm->m;
    pm->p            = NU_VEC3_ZEROS;
    return ctx.point_masses.size - 1;
}
static void
add_distance_constraint (nu_u32_t a, nu_u32_t b, float distance)
{
    distance_constraint_t *c = NU_VEC_PUSH(&ctx.distance_constraints);
    c->a                     = a;
    c->b                     = b;
    c->d                     = distance;
}
static void
shoot_context (nu_v3_t pos, nu_v3_t dir)
{
    nu_v3_t     vel = nu_v3_muls(dir, 10);
    const float s   = 1.0;
    const float h   = s / 2;
    const float s2  = nu_sqrt(s + s);
    const float s3  = nu_sqrt(s + s + s);

    nu_u32_t p0 = add_pm(nu_v3_add(pos, nu_v3(-h, h, h)), vel);
    nu_u32_t p1 = add_pm(nu_v3_add(pos, nu_v3(-h, h, -h)), vel);
    nu_u32_t p2 = add_pm(nu_v3_add(pos, nu_v3(h, h, -h)), vel);
    nu_u32_t p3 = add_pm(nu_v3_add(pos, nu_v3(h, h, h)), vel);
    nu_u32_t p4 = add_pm(nu_v3_add(pos, nu_v3(-h, -h, h)), vel);
    nu_u32_t p5 = add_pm(nu_v3_add(pos, nu_v3(-h, -h, -h)), vel);
    nu_u32_t p6 = add_pm(nu_v3_add(pos, nu_v3(h, -h, -h)), vel);
    nu_u32_t p7 = add_pm(nu_v3_add(pos, nu_v3(h, -h, h)), vel);

    add_distance_constraint(p0, p1, s);
    add_distance_constraint(p1, p2, s);
    add_distance_constraint(p2, p3, s);
    add_distance_constraint(p3, p0, s);

    add_distance_constraint(p4, p5, s);
    add_distance_constraint(p5, p6, s);
    add_distance_constraint(p6, p7, s);
    add_distance_constraint(p7, p4, s);

    add_distance_constraint(p0, p4, s);
    add_distance_constraint(p1, p5, s);
    add_distance_constraint(p2, p6, s);
    add_distance_constraint(p3, p7, s);

    add_distance_constraint(p0, p2, s2);
    // add_distance_constraint(p1, p3, s2);

    add_distance_constraint(p4, p6, s2);
    // add_distance_constraint(p5, p7, s2);

    add_distance_constraint(p0, p7, s2);
    // add_distance_constraint(p4, p3, s2);

    add_distance_constraint(p5, p2, s2);
    // add_distance_constraint(p1, p6, s2);

    add_distance_constraint(p4, p1, s2);
    // add_distance_constraint(p0, p5, s2);

    add_distance_constraint(p3, p6, s2);
    // add_distance_constraint(p7, p2, s2);

    add_distance_constraint(p4, p2, s3);
    add_distance_constraint(p0, p6, s3);
    add_distance_constraint(p7, p1, s3);
    add_distance_constraint(p3, p5, s3);
}
static void
init_context (void)
{
    NU_VEC_INIT(100, &ctx.point_masses);
    NU_VEC_INIT(100, &ctx.collision_constraints);
    NU_VEC_INIT(100, &ctx.distance_constraints);
}
static void
free_context (void)
{
    NU_VEC_FREE(&ctx.distance_constraints);
    NU_VEC_FREE(&ctx.collision_constraints);
    NU_VEC_FREE(&ctx.point_masses);
}
static nu_v3_t
compute_sum_forces (point_mass_t *pm)
{
    return nu_v3(0, -9.81, 0);
}

static void
update_context (float dt)
{
    // Convert to seconds
    dt *= 0.001;

    const nu_size_t substep = 10;
    float           subdt   = dt / substep;
    for (nu_size_t n = 0; n < substep; ++n)
    {
        // (7) integrate position (TODO: integrate rotation)
        for (nu_size_t i = 0; i < ctx.point_masses.size; ++i)
        {
            point_mass_t *pm        = ctx.point_masses.data + i;
            nu_v3_t       sum_force = compute_sum_forces(pm);

            // keep previous position
            pm->p = pm->x;

            // integrate position
            pm->x = nu_v3_add(pm->x, nu_v3_muls(pm->v, subdt));
            pm->x = nu_v3_add(pm->x,
                              nu_v3_muls(sum_force, subdt * subdt * pm->w));
        }

        // (8) generate collision constraints
        NU_VEC_CLEAR(&ctx.collision_constraints);
        for (nu_size_t i = 0; i < ctx.point_masses.size; ++i)
        {
            point_mass_t *pm = ctx.point_masses.data + i;

            // ground collision
            const float ground = 0;
            if (pm->x.y < ground)
            {
                collision_constraint_t *c
                    = NU_VEC_PUSH(&ctx.collision_constraints);
                c->q = nu_v3(pm->x.x, ground, pm->x.z);
                c->n = NU_VEC3_UP;
                c->a = i;
            }

            // box collision
            for (nu_size_t b = 0; b < NU_ARRAY_SIZE(boxes); ++b)
            {
                nu_b3_t box = boxes[b];
                if (nu_b3_contains(box, pm->x))
                {
                    nu_v3_t rel = pm->x;

                    // Left
                    float   d, maxd = NU_FLT_MAX;
                    nu_v3_t q, n;
                    q = n = NU_VEC3_ZEROS;

                    d = box.max.x - rel.x;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(box.max.x, rel.y, rel.z);
                        n    = NU_VEC3_RIGHT;
                    }
                    d = rel.x - box.min.x;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(box.min.x, rel.y, rel.z);
                        n    = NU_VEC3_LEFT;
                    }

                    d = box.max.y - rel.y;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(rel.x, box.max.y, rel.z);
                        n    = NU_VEC3_UP;
                    }
                    d = rel.y - box.min.y;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(rel.x, box.min.y, rel.z);
                        n    = NU_VEC3_DOWN;
                    }

                    d = box.max.z - rel.z;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(rel.x, rel.y, box.max.z);
                        n    = NU_VEC3_BACKWARD;
                    }
                    d = rel.z - box.min.z;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(rel.x, rel.y, box.min.z);
                        n    = NU_VEC3_FORWARD;
                    }

                    collision_constraint_t *c
                        = NU_VEC_PUSH(&ctx.collision_constraints);
                    c->q = q;
                    c->n = n;
                    c->a = i;
                }
            }
        }

        // (9) solve constraints
        // solve collision constraints
        for (nu_size_t i = 0; i < ctx.collision_constraints.size; ++i)
        {
            collision_constraint_t *c     = ctx.collision_constraints.data + i;
            point_mass_t           *a     = ctx.point_masses.data + c->a;
            nu_v3_t                 v     = nu_v3_sub(a->x, c->q);
            float                   depth = -nu_v3_dot(v, c->n);
            if (depth > 0)
            {
                a->x = nu_v3_add(a->x, nu_v3_muls(c->n, depth));
            }
        }
        // solve distance constraints
        for (nu_size_t i = 0; i < ctx.distance_constraints.size; ++i)
        {
            distance_constraint_t *c = ctx.distance_constraints.data + i;
            point_mass_t          *a = ctx.point_masses.data + c->a;
            point_mass_t          *b = ctx.point_masses.data + c->b;

            nu_v3_t delta          = nu_v3_sub(b->x, a->x);
            float   distance       = nu_v3_norm(delta);
            nu_v3_t required_delta = nu_v3_muls(delta, c->d / distance);

            nu_v3_t offset = nu_v3_sub(required_delta, delta);

            nu_v3_t ca = nu_v3_muls(offset, -0.5);
            nu_v3_t cb = nu_v3_muls(offset, 0.5);

            a->x = nu_v3_add(a->x, ca);
            b->x = nu_v3_add(b->x, cb);
        }
        // (12) compute new velocity
        for (nu_size_t i = 0; i < ctx.point_masses.size; ++i)
        {
            point_mass_t *pm = ctx.point_masses.data + i;
            pm->v            = nu_v3_divs(nu_v3_sub(pm->x, pm->p), subdt);
        }
        // (16) solve velocities
        for (nu_size_t i = 0; i < ctx.collision_constraints.size; ++i)
        {
            const float elasticity = 0.1;
            const float friction   = 30;

            collision_constraint_t *c  = ctx.collision_constraints.data + i;
            point_mass_t           *pm = ctx.point_masses.data + c->a;

            nu_v3_t vn = nu_v3_muls(c->n, nu_v3_dot(c->n, pm->v));
            nu_v3_t vt = nu_v3_sub(pm->v, vn);
            vn         = nu_v3_muls(vn, -elasticity);
            vt         = nu_v3_muls(vt, nu_exp(-friction * subdt));
            pm->v      = nu_v3_add(vn, vt);
        }
    }
}

int
main (void)
{
    nu_config_surface_size(WIDTH, HEIGHT);
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

    // Setup boxes
    boxes[0] = BOX0;
    boxes[1] = BOX1;

    // Create depth buffer
    nu_texture_t depth_buffer
        = nu_texture_create(NU_TEXTURE_DEPTH_TARGET, nu_v3u(WIDTH, HEIGHT, 0));
    // Grid mesh
    nu_mesh_t grid;
    {
        nu_geometry_t g = nu_geometry_create();
        nu_geometry_grid(g, 30, 30, 1, 1);
        nu_geometry_transform(g, nu_m4_translate(nu_v3(-15, 0, -15)));
        grid = nu_geometry_create_mesh(g, NU_PRIMITIVE_LINES);
        nu_geometry_delete(g);
    }

    // Cube mesh
    nu_mesh_t cube;
    {
        nu_geometry_t g = nu_geometry_create();
        nu_geometry_cube(g, 0.1);
        nu_geometry_transform(g, nu_m4_translate(nu_v3s(-0.05)));
        cube = nu_geometry_create_mesh(g, NU_PRIMITIVE_LINES);
        nu_geometry_delete(g);
    }

    nu_material_t redmat
        = nu_material_create_color(NU_MATERIAL_SURFACE, NU_COLOR_RED);
    nu_material_t greenmat
        = nu_material_create_color(NU_MATERIAL_SURFACE, NU_COLOR_GREEN);
    nu_material_t bluemat
        = nu_material_create_color(NU_MATERIAL_SURFACE, NU_COLOR_BLUE_SKY);

    // Create font
    nu_font_t font = nu_font_create_default();

    // Create camera
    nu_camera_t camera = nu_camera_create();
    nu_camera_set_proj(
        camera, nu_perspective(nu_radian(70), nu_surface_aspect(), 0.01, 500));

    // Create renderpasses
    nu_texture_t surface_tex = nu_surface_color_target();
    nu_color_t   clear_color = NU_COLOR_BLACK;

    nu_renderpass_t main_pass = nu_renderpass_create(NU_RENDERPASS_FORWARD);
    nu_renderpass_set_camera(main_pass, camera);
    nu_renderpass_set_color_target(main_pass, surface_tex);
    nu_renderpass_set_depth_target(main_pass, depth_buffer);
    nu_renderpass_set_clear_color(main_pass, &clear_color);
    nu_renderpass_set_shade(main_pass, NU_SHADE_LIT);

    nu_renderpass_t gui_pass = nu_renderpass_create(NU_RENDERPASS_CANVAS);
    nu_renderpass_set_color_target(gui_pass, surface_tex);

    nu_renderpass_t wireframe_pass
        = nu_renderpass_create(NU_RENDERPASS_FORWARD);
    nu_renderpass_set_camera(wireframe_pass, camera);
    nu_renderpass_set_color_target(wireframe_pass, surface_tex);
    nu_renderpass_set_shade(wireframe_pass, NU_SHADE_WIREFRAME);
    // nu_renderpass_texture(
    //     wireframe_pass, NU_RENDERPASS_DEPTH_TARGET, depth_buffer);

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
            nu_m4_t transform = nu_controller_transform(controller);
            nu_v3_t pos       = nu_m4_mulv3(transform, NU_VEC3_ZEROS);
            nu_v4_t dir0      = nu_m4_mulv(transform, nu_v4(0, 0, -1, 0));
            nu_v3_t dir       = nu_v3(dir0.x, dir0.y, dir0.z);
            shoot_context(pos, dir);
        }

        nu_fixed_loop_update(&loop, 1, delta);
        nu_u32_t id;
        while (nu_fixed_loop_next(&loop, 1, &id))
        {
            if (id == 0)
            {
                update_context(loop.timestep);
            }
        }

        delta = nu_timer_elapsed(&timer);
        nu_timer_reset(&timer);

        for (nu_size_t i = 0; i < ctx.point_masses.size; ++i)
        {
            point_mass_t *pm = ctx.point_masses.data + i;
            nu_draw_mesh(wireframe_pass, cube, redmat, nu_m4_translate(pm->x));
        }
        for (nu_size_t i = 0; i < ctx.distance_constraints.size; ++i)
        {
            nu_v3_t p0
                = ctx.point_masses.data[ctx.distance_constraints.data[i].a].x;
            nu_v3_t p1
                = ctx.point_masses.data[ctx.distance_constraints.data[i].b].x;
            const nu_v3_t points[] = { p0, p1 };
            nu_draw_lines(
                wireframe_pass, points, 1, greenmat, nu_m4_identity());
        }

        // Update camera controller
        nu_controller_update(controller, delta, camera);

        nu_draw_mesh(wireframe_pass, grid, NU_NULL, nu_m4_identity());
        nu_draw_stats(gui_pass, font, nu_v2i(10, 10));
        for (nu_size_t i = 0; i < NU_ARRAY_SIZE(boxes); ++i)
        {
            nu_draw_box(wireframe_pass, boxes[i], bluemat, nu_m4_identity());
        }

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
