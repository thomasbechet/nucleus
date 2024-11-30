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

static nu_input_t g_quit;
static nu_input_t g_move_x;
static nu_input_t g_move_y;
static nu_input_t g_move_z;
static nu_input_t g_view_yaw;
static nu_input_t g_view_pitch;
static nu_input_t g_view_roll;
static nu_input_t g_switch_mode;
static nu_input_t g_shoot;

static nu_controller_t g_controller;
static nu_texture_t    g_depth_buffer;
static nu_mesh_t       g_grid;
static nu_mesh_t       g_cube;
static nu_material_t   g_redmat;
static nu_material_t   g_greenmat;
static nu_material_t   g_bluemat;
static nu_font_t       g_font;
static nu_camera_t     g_camera;
static nu_texture_t    g_surface_tex;
static nu_color_t      g_clear_color;
static nu_renderpass_t g_main_pass;
static nu_renderpass_t g_gui_pass;
static nu_renderpass_t g_wireframe_pass;

static nu_scope_t g_scope;

typedef struct
{
    point_mass_vec_t           point_masses;
    collision_constraint_vec_t collision_constraints;
    distance_constraint_vec_t  distance_constraints;
} context_t;

#define BOX0 nu_b3(nu_v3(0, 0, 0), nu_v3(5, 2, 5))
#define BOX1 nu_b3(nu_v3(-5, 0, -5), nu_v3(-1, 6, -1))
static nu_b3_t g_boxes[2];

static context_t g_ctx;

static nu_u32_t
add_pm (nu_v3_t pos, nu_v3_t vel)
{
    point_mass_t *pm = NU_VEC_PUSH(&g_ctx.point_masses);
    NU_ASSERT(pm);
    pm->x = pos;
    pm->v = vel;
    pm->f = NU_V3_ZEROS;
    pm->m = 1;
    pm->w = 1.0 / pm->m;
    pm->p = NU_V3_ZEROS;
    return g_ctx.point_masses.size - 1;
}
static void
add_distance_constraint (nu_u32_t a, nu_u32_t b, float distance)
{
    distance_constraint_t *c = NU_VEC_PUSH(&g_ctx.distance_constraints);
    NU_ASSERT(c);
    c->a = a;
    c->b = b;
    c->d = distance;
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
    NU_VEC_ALLOC(&g_ctx.point_masses, 1024);
    NU_VEC_ALLOC(&g_ctx.collision_constraints, 1024);
    NU_VEC_ALLOC(&g_ctx.distance_constraints, 1024);
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
        for (nu_size_t i = 0; i < g_ctx.point_masses.size; ++i)
        {
            point_mass_t *pm        = g_ctx.point_masses.data + i;
            nu_v3_t       sum_force = compute_sum_forces(pm);

            // keep previous position
            pm->p = pm->x;

            // integrate position
            pm->x = nu_v3_add(pm->x, nu_v3_muls(pm->v, subdt));
            pm->x = nu_v3_add(pm->x,
                              nu_v3_muls(sum_force, subdt * subdt * pm->w));
        }

        // (8) generate collision constraints
        NU_VEC_CLEAR(&g_ctx.collision_constraints);
        for (nu_size_t i = 0; i < g_ctx.point_masses.size; ++i)
        {
            point_mass_t *pm = g_ctx.point_masses.data + i;

            // ground collision
            const float ground = 0;
            if (pm->x.y < ground)
            {
                collision_constraint_t *c
                    = NU_VEC_PUSH(&g_ctx.collision_constraints);
                NU_ASSERT(c);
                c->q = nu_v3(pm->x.x, ground, pm->x.z);
                c->n = NU_V3_UP;
                c->a = i;
            }

            // box collision
            for (nu_size_t b = 0; b < NU_ARRAY_SIZE(g_boxes); ++b)
            {
                nu_b3_t box = g_boxes[b];
                if (nu_b3_contains(box, pm->x))
                {
                    nu_v3_t rel = pm->x;

                    // Left
                    float   d, maxd = NU_FLT_MAX;
                    nu_v3_t q, n;
                    q = n = NU_V3_ZEROS;

                    d = box.max.x - rel.x;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(box.max.x, rel.y, rel.z);
                        n    = NU_V3_RIGHT;
                    }
                    d = rel.x - box.min.x;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(box.min.x, rel.y, rel.z);
                        n    = NU_V3_LEFT;
                    }

                    d = box.max.y - rel.y;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(rel.x, box.max.y, rel.z);
                        n    = NU_V3_UP;
                    }
                    d = rel.y - box.min.y;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(rel.x, box.min.y, rel.z);
                        n    = NU_V3_DOWN;
                    }

                    d = box.max.z - rel.z;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(rel.x, rel.y, box.max.z);
                        n    = NU_V3_BACKWARD;
                    }
                    d = rel.z - box.min.z;
                    if (d < maxd)
                    {
                        maxd = d;
                        q    = nu_v3(rel.x, rel.y, box.min.z);
                        n    = NU_V3_FORWARD;
                    }

                    collision_constraint_t *c
                        = NU_VEC_PUSH(&g_ctx.collision_constraints);
                    NU_ASSERT(c);
                    c->q = q;
                    c->n = n;
                    c->a = i;
                }
            }
        }

        // (9) solve constraints
        // solve collision constraints
        for (nu_size_t i = 0; i < g_ctx.collision_constraints.size; ++i)
        {
            collision_constraint_t *c = g_ctx.collision_constraints.data + i;
            point_mass_t           *a = g_ctx.point_masses.data + c->a;
            nu_v3_t                 v = nu_v3_sub(a->x, c->q);
            float                   depth = -nu_v3_dot(v, c->n);
            if (depth > 0)
            {
                a->x = nu_v3_add(a->x, nu_v3_muls(c->n, depth));
            }
        }
        // solve distance constraints
        for (nu_size_t i = 0; i < g_ctx.distance_constraints.size; ++i)
        {
            distance_constraint_t *c = g_ctx.distance_constraints.data + i;
            point_mass_t          *a = g_ctx.point_masses.data + c->a;
            point_mass_t          *b = g_ctx.point_masses.data + c->b;

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
        for (nu_size_t i = 0; i < g_ctx.point_masses.size; ++i)
        {
            point_mass_t *pm = g_ctx.point_masses.data + i;
            pm->v            = nu_v3_divs(nu_v3_sub(pm->x, pm->p), subdt);
        }
        // (16) solve velocities
        for (nu_size_t i = 0; i < g_ctx.collision_constraints.size; ++i)
        {
            const float elasticity = 0.1;
            const float friction   = 30;

            collision_constraint_t *c  = g_ctx.collision_constraints.data + i;
            point_mass_t           *pm = g_ctx.point_masses.data + c->a;

            nu_v3_t vn = nu_v3_muls(c->n, nu_v3_dot(c->n, pm->v));
            nu_v3_t vt = nu_v3_sub(pm->v, vn);
            vn         = nu_v3_muls(vn, -elasticity);
            vt         = nu_v3_muls(vt, nu_exp(-friction * subdt));
            pm->v      = nu_v3_add(vn, vt);
        }
    }
}

void
init (void)
{
    g_scope = nu_scope_new(NU_STR("main"), NU_MEM_1M);
    nu_scope_set_active(g_scope);

    // Configure inputs
    g_quit        = nu_input_new();
    g_move_x      = nu_input_new();
    g_move_y      = nu_input_new();
    g_move_z      = nu_input_new();
    g_view_yaw    = nu_input_new();
    g_view_pitch  = nu_input_new();
    g_view_roll   = nu_input_new();
    g_switch_mode = nu_input_new();
    g_shoot       = nu_input_new();

    // Create camera controller
    g_controller = nu_controller_new(g_view_pitch,
                                     g_view_yaw,
                                     g_view_roll,
                                     g_move_x,
                                     g_move_y,
                                     g_move_z,
                                     g_switch_mode);

    // Bind inputs
    nuext_input_bind_button(g_quit, NUEXT_BUTTON_ESCAPE);
    nuext_input_bind_button_value(g_move_z, NUEXT_BUTTON_W, 1);
    nuext_input_bind_button_value(g_move_z, NUEXT_BUTTON_S, -1);
    nuext_input_bind_button_value(g_move_x, NUEXT_BUTTON_D, 1);
    nuext_input_bind_button_value(g_move_x, NUEXT_BUTTON_A, -1);
    nuext_input_bind_button_value(g_move_y, NUEXT_BUTTON_X, 1);
    nuext_input_bind_button_value(g_move_y, NUEXT_BUTTON_Z, -1);
    nuext_input_bind_axis(g_view_pitch, NUEXT_AXIS_MOUSE_MOTION_Y);
    nuext_input_bind_axis(g_view_yaw, NUEXT_AXIS_MOUSE_MOTION_X);
    nuext_input_bind_button_value(g_view_pitch, NUEXT_BUTTON_J, 0.12);
    nuext_input_bind_button_value(g_view_pitch, NUEXT_BUTTON_K, -0.12);
    nuext_input_bind_button_value(g_view_yaw, NUEXT_BUTTON_L, 0.12);
    nuext_input_bind_button_value(g_view_yaw, NUEXT_BUTTON_H, -0.12);
    nuext_input_bind_button_value(g_view_roll, NUEXT_BUTTON_Q, 0.12);
    nuext_input_bind_button_value(g_view_roll, NUEXT_BUTTON_E, -0.12);
    nuext_input_bind_button(g_switch_mode, NUEXT_BUTTON_C);
    nuext_input_bind_button(g_shoot, NUEXT_BUTTON_R);

    // Setup boxes
    g_boxes[0] = BOX0;
    g_boxes[1] = BOX1;

    // Create depth buffer
    g_depth_buffer = nu_texture_new(
        NU_TEXTURE_DEPTHBUFFER_TARGET, nu_v3u(WIDTH, HEIGHT, 1), 1);
    // Grid mesh
    {
        nu_geometry_t g
            = nu_geometry_new_mesh(NU_PRIMITIVE_TRIANGLES, 1000, 1000, 10000);
        nu_geometry_grid(g, 30, 30, 1, 1);
        nu_geometry_transform(g, nu_m4_translate(nu_v3(-15, 0, -15)));
        g_grid = nu_mesh_new_geometry(g);
    }

    // Cube mesh
    {
        nu_geometry_t g
            = nu_geometry_new_mesh(NU_PRIMITIVE_LINES, 1000, 1000, 10000);
        nu_geometry_cube(g, 0.1);
        nu_geometry_transform(g, nu_m4_translate(nu_v3s(-0.05)));
        g_cube = nu_mesh_new_geometry(g);
    }

    g_redmat   = nu_material_new_color(NU_MATERIAL_SURFACE, NU_COLOR_RED);
    g_greenmat = nu_material_new_color(NU_MATERIAL_SURFACE, NU_COLOR_GREEN);
    g_bluemat  = nu_material_new_color(NU_MATERIAL_SURFACE, NU_COLOR_BLUE_SKY);

    // Create font
    g_font = nu_font_new_default();

    // Create camera
    g_camera = nu_camera_new();
    nu_camera_set_proj(
        g_camera,
        nu_perspective(nu_radian(70), nu_surface_aspect(), 0.01, 500));

    // Create renderpasses
    g_surface_tex = nu_surface_color_target();
    g_clear_color = NU_COLOR_BLACK;

    g_main_pass = nu_renderpass_new(NU_RENDERPASS_FORWARD);
    nu_renderpass_set_camera(g_main_pass, g_camera);
    nu_renderpass_set_color_target(g_main_pass, g_surface_tex);
    nu_renderpass_set_depth_target(g_main_pass, g_depth_buffer);
    nu_renderpass_set_clear_color(g_main_pass, &g_clear_color);
    nu_renderpass_set_shade(g_main_pass, NU_SHADE_LIT);

    g_gui_pass = nu_renderpass_new(NU_RENDERPASS_CANVAS);
    nu_renderpass_set_color_target(g_gui_pass, g_surface_tex);

    g_wireframe_pass = nu_renderpass_new(NU_RENDERPASS_FORWARD);
    nu_renderpass_set_camera(g_wireframe_pass, g_camera);
    nu_renderpass_set_color_target(g_wireframe_pass, g_surface_tex);
    nu_renderpass_set_shade(g_wireframe_pass, NU_SHADE_WIREFRAME);
    // nu_renderpass_texture(
    //     wireframe_pass, NU_RENDERPASS_DEPTH_TARGET, depth_buffer);

    init_context();
    nu_fixedloop_new(update_context, 1.0 / 60.0 * 1000.0);
}

void
update (void)
{
    if (nu_input_just_pressed(g_quit))
    {
        nu_request_stop();
    }
    if (nu_input_just_pressed(g_shoot))
    {
        nu_m4_t transform = nu_controller_transform(g_controller);
        nu_v3_t pos       = nu_m4_mulv3(transform, NU_V3_ZEROS);
        nu_v4_t dir0      = nu_m4_mulv(transform, nu_v4(0, 0, -1, 0));
        nu_v3_t dir       = nu_v3(dir0.x, dir0.y, dir0.z);
        shoot_context(pos, dir);
    }

    nu_f32_t delta = nu_deltatime();
    nu_fixedloop_update(delta);

    for (nu_size_t i = 0; i < g_ctx.point_masses.size; ++i)
    {
        point_mass_t *pm = g_ctx.point_masses.data + i;
        nu_draw_mesh(
            g_wireframe_pass, g_cube, g_redmat, nu_m4_translate(pm->x));
    }
    for (nu_size_t i = 0; i < g_ctx.distance_constraints.size; ++i)
    {
        nu_v3_t p0
            = g_ctx.point_masses.data[g_ctx.distance_constraints.data[i].a].x;
        nu_v3_t p1
            = g_ctx.point_masses.data[g_ctx.distance_constraints.data[i].b].x;
        const nu_v3_t points[] = { p0, p1 };
        nu_draw_lines(
            g_wireframe_pass, points, 2, g_greenmat, nu_m4_identity());
    }

    // Update camera controller
    nu_controller_update(g_controller, delta, g_camera);

    nu_draw_mesh(g_wireframe_pass, g_grid, NU_NULL, nu_m4_identity());
    nu_draw_stats(g_gui_pass, g_font, nu_v2i(10, 10));
    for (nu_size_t i = 0; i < NU_ARRAY_SIZE(g_boxes); ++i)
    {
        nu_draw_box(g_wireframe_pass, g_boxes[i], g_bluemat, nu_m4_identity());
    }

    // Submit renderpass
    nu_renderpass_submit(g_main_pass);
    nu_renderpass_submit(g_wireframe_pass);
    nu_renderpass_submit(g_gui_pass);
}

void
nu_app (void)
{
    nu_app_surface_size(WIDTH, HEIGHT);
    nu_app_init_callback(init);
    nu_app_update_callback(update);
}
