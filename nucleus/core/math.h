#ifndef NU_MATH_H
#define NU_MATH_H

#include <nucleus/core/config.h>

#define NU_PI 3.14159265359

#define NU_MIN(a, b)          (((a) < (b)) ? (a) : (b))
#define NU_MAX(a, b)          (((a) > (b)) ? (a) : (b))
#define NU_CLAMP(x, min, max) (NU_MAX(min, NU_MIN(max, x)))
#define NU_CLAMP01(x)         (NU_CLAMP(x, 0, 1))

#define NU_VEC2_SIZE 2
#define NU_VEC3_SIZE 3
#define NU_VEC4_SIZE 4
#define NU_QUAT_SIZE 4
#define NU_MAT3_SIZE 9
#define NU_MAT4_SIZE 16

#define NU_VEC2_ZERO nu_vec2(0, 0)
#define NU_VEC2_ONE  nu_vec2(1, 1)

#define NU_IVEC2_ZERO nu_ivec2(0, 0)

#define NU_UVEC2_ZERO nu_uvec2(0, 0)
#define NU_UVEC2_ONE  nu_uvec2(1, 1)

#define NU_VEC3_ZERO     nu_vec3(0, 0, 0)
#define NU_VEC3_ONE      nu_vec3(1, 1, 1)
#define NU_VEC3_UP       nu_vec3(0, 1, 0)
#define NU_VEC3_DOWN     nu_vec3(0, -1, 0)
#define NU_VEC3_FORWARD  nu_vec3(0, 0, -1)
#define NU_VEC3_BACKWARD nu_vec3(0, 0, 1)
#define NU_VEC3_LEFT     nu_vec3(-1, 0, 0)
#define NU_VEC3_RIGHT    nu_vec3(1, 0, 0)

#define NU_VEC4_ZERO nu_vec4(0, 0, 0, 0)

#define NU_VEC2_FORMAT "%lf %lf"
#define NU_VEC3_FORMAT "%lf %lf %lf"
#define NU_VEC4_FORMAT "%lf %lf %lf %lf"

typedef union
{
    struct
    {
        float x;
        float y;
    };
    float xy[NU_VEC2_SIZE];
} nu_vec2_t;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
    };
    float xyz[NU_VEC3_SIZE];
} nu_vec3_t;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
    float xyzw[NU_VEC4_SIZE];
} nu_vec4_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
    };
    nu_u32_t xy[NU_VEC2_SIZE];
} nu_uvec2_t;

typedef union
{
    struct
    {
        nu_i32_t x;
        nu_i32_t y;
    };
    nu_i32_t xy[NU_VEC2_SIZE];
} nu_ivec2_t;

typedef union
{
    struct
    {
        nu_u32_t x;
        nu_u32_t y;
        nu_u32_t z;
        nu_u32_t w;
    };
    nu_u32_t xyzw[NU_VEC4_SIZE];
} nu_uvec4_t;

typedef union
{
    struct
    {
        float x;
        float y;
        float z;
        float w;
    };
    float xyzw[NU_QUAT_SIZE];
} nu_quat_t;

typedef union
{

    struct
    {
        float x1;
        float x2;
        float x3;
        float y1;
        float y2;
        float y3;
        float z1;
        float z2;
        float z3;
    };
    float data[NU_MAT3_SIZE];
} nu_mat3_t;

typedef union
{
    struct
    {
        float x1;
        float x2;
        float x3;
        float x4;
        float y1;
        float y2;
        float y3;
        float y4;
        float z1;
        float z2;
        float z3;
        float z4;
        float w1;
        float w2;
        float w3;
        float w4;
    };
    float data[NU_MAT4_SIZE];
} nu_mat4_t;

typedef struct
{
    nu_ivec2_t p;
    nu_uvec2_t s;
} nu_rect_t;

NU_API nu_bool_t nu_is_power_of_two(nu_size_t n);
NU_API nu_size_t nu_log2(nu_size_t n);
NU_API float     nu_fabs(float f);
NU_API float     nu_floor(float f);
NU_API float     nu_radian(float d);
NU_API float     nu_sqrt(float x);
NU_API float     nu_pow(float b, float e);
NU_API float     nu_cos(float x);
NU_API float     nu_sin(float x);
NU_API float     nu_tan(float x);

NU_API nu_vec2_t nu_vec2(float x, float y);
NU_API nu_vec2_t nu_vec2_add(nu_vec2_t a, nu_vec2_t b);
NU_API nu_vec2_t nu_vec2_sub(nu_vec2_t a, nu_vec2_t b);
NU_API nu_vec2_t nu_vec2_mul(nu_vec2_t a, nu_vec2_t b);
NU_API nu_vec2_t nu_vec2_muls(nu_vec2_t a, float s);
NU_API nu_vec2_t nu_vec2_div(nu_vec2_t a, nu_vec2_t b);
NU_API nu_vec2_t nu_vec2_divs(nu_vec2_t a, float s);
NU_API nu_vec2_t nu_vec2_floor(nu_vec2_t a);

NU_API nu_vec3_t nu_vec3(float x, float y, float z);
NU_API nu_vec3_t nu_vec3_add(nu_vec3_t a, nu_vec3_t b);
NU_API nu_vec3_t nu_vec3_sub(nu_vec3_t a, nu_vec3_t b);
NU_API nu_vec3_t nu_vec3_mul(nu_vec3_t a, nu_vec3_t b);
NU_API nu_vec3_t nu_vec3_muls(nu_vec3_t a, float s);
NU_API nu_vec3_t nu_vec3_div(nu_vec3_t a, nu_vec3_t b);
NU_API nu_vec3_t nu_vec3_divs(nu_vec3_t a, float s);
NU_API float     nu_vec3_norm(nu_vec3_t a);
NU_API nu_vec3_t nu_vec3_normalize(nu_vec3_t a);
NU_API nu_vec3_t nu_vec3_cross(nu_vec3_t a, nu_vec3_t b);
NU_API float     nu_vec3_dot(nu_vec3_t a, nu_vec3_t b);

NU_API nu_vec4_t nu_vec4(float x, float y, float z, float w);
NU_API float     nu_vec4_dot(nu_vec4_t a, nu_vec4_t b);
NU_API float     nu_vec4_norm(nu_vec4_t v);
NU_API nu_vec2_t nu_vec4_xy(nu_vec4_t v);
NU_API nu_vec2_t nu_vec4_zw(nu_vec4_t v);

NU_API nu_ivec2_t nu_ivec2(nu_i32_t x, nu_i32_t y);

NU_API nu_uvec2_t nu_uvec2(nu_u32_t x, nu_u32_t y);

NU_API nu_uvec4_t nu_uvec4(nu_u32_t x, nu_u32_t y, nu_u32_t z, nu_u32_t w);

NU_API nu_quat_t nu_quat(float x, float y, float z, float w);
NU_API nu_quat_t nu_quat_identity(void);
NU_API nu_vec4_t nu_quat_vec4(nu_quat_t a);
NU_API float     nu_quat_norm(nu_quat_t a);
NU_API nu_quat_t nu_quat_axis(nu_vec3_t axis, float angle);
NU_API nu_quat_t nu_quat_mul(nu_quat_t a, nu_quat_t b);
NU_API nu_vec3_t nu_quat_mulv3(nu_quat_t a, nu_vec3_t v);
NU_API nu_quat_t nu_quat_mul_axis(nu_quat_t q, nu_vec3_t axis, float angle);
NU_API nu_mat3_t nu_quat_mat3(nu_quat_t q);
NU_API nu_mat4_t nu_quat_mat4(nu_quat_t q);
NU_API nu_mat4_t nu_quat_mulm4(nu_quat_t a, nu_mat4_t m);

NU_API nu_mat3_t nu_mat3_zero(void);
NU_API nu_mat3_t nu_mat3_identity(void);
NU_API nu_mat3_t nu_mat3_translate(float x, float y);
NU_API nu_mat3_t nu_mat3_scale(float x, float y);
NU_API nu_mat3_t nu_mat3_mul(nu_mat3_t a, nu_mat3_t b);

NU_API nu_mat4_t nu_mat4_zero(void);
NU_API nu_mat4_t nu_mat4_identity(void);
NU_API nu_mat4_t nu_mat4_translate(float x, float y, float z);
NU_API nu_mat4_t nu_mat4_scale(float x, float y, float z);
NU_API nu_mat4_t nu_mat4_rotate_y(float angle);
NU_API nu_mat4_t nu_mat4_mul(nu_mat4_t a, nu_mat4_t b);

NU_API nu_rect_t nu_rect(nu_i32_t x, nu_i32_t y, nu_u32_t w, nu_u32_t h);
NU_API nu_bool_t nu_rect_contains(nu_rect_t r, nu_vec2_t p);
NU_API nu_vec2_t nu_rect_normalize(nu_rect_t r, nu_vec2_t p);

NU_API nu_mat4_t nu_perspective(float fov,
                                float aspect_ratio,
                                float z_near,
                                float z_far);
NU_API nu_mat4_t nu_lookat(nu_vec3_t eye, nu_vec3_t center, nu_vec3_t up);

#endif
