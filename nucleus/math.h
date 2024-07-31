#ifndef NU_MATH_H
#define NU_MATH_H

#include <nucleus/types.h>

NU_API nu_bool_t nu_is_power_of_two(nu_size_t n);
NU_API nu_size_t nu_log2(nu_size_t n);
NU_API float     nu_floor(float f);
NU_API float     nu_radian(float d);
NU_API float     nu_sqrt(float x);
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
NU_API nu_vec3_t nu_vec3_normalize(nu_vec3_t a);
NU_API nu_vec3_t nu_vec3_cross(nu_vec3_t a, nu_vec3_t b);

NU_API nu_vec4_t nu_vec4(float x, float y, float z, float w);
NU_API nu_vec2_t nu_vec4_xy(nu_vec4_t v);
NU_API nu_vec2_t nu_vec4_zw(nu_vec4_t v);

NU_API nu_uvec2_t nu_uvec2(nu_u32_t x, nu_u32_t y);

NU_API nu_uvec4_t nu_uvec4(nu_u32_t x, nu_u32_t y, nu_u32_t z, nu_u32_t w);

NU_API nu_mat4_t nu_mat4_zero(void);
NU_API nu_mat4_t nu_mat4_identity(void);
NU_API nu_mat4_t nu_mat4_rotate_x(float x);
NU_API nu_mat4_t nu_mat4_mul(nu_mat4_t a, nu_mat4_t b);

NU_API nu_mat4_t nu_perspective(float fov,
                                float aspect_ratio,
                                float z_near,
                                float z_far);
NU_API nu_mat4_t nu_lookat(nu_vec3_t eye, nu_vec3_t center, nu_vec3_t up);

#ifdef NU_IMPLEMENTATION

#ifdef NU_STDLIB
#include <math.h>
#endif

nu_bool_t
nu_is_power_of_two (nu_size_t n)
{
    return (n & (n - 1)) == 0;
}
nu_size_t
nu_log2 (nu_size_t n)
{
    nu_size_t result = 0;
    n                = n >> 1;
    while (n != 0)
    {
        result++;
        n = n >> 1;
    }
    return result;
}
float
nu_floor (float f)
{
#ifdef NU_STDLIB
    return floorf(f);
#endif
}
float
nu_radian (float d)
{
    return d * (NU_PI / 180.0f);
}
float
nu_sqrt (float x)
{
#ifdef NU_STDLIB
    return sqrtf(x);
#endif
}
float
nu_cos (float x)
{
#ifdef NU_STDLIB
    return cosf(x);
#endif
}
float
nu_sin (float x)
{
#ifdef NU_STDLIB
    return sinf(x);
#endif
}
float
nu_tan (float x)
{
#ifdef NU_STDLIB
    return tanf(x);
#endif
}

nu_vec2_t
nu_vec2 (float x, float y)
{
    nu_vec2_t ret;
    ret.x = x;
    ret.y = y;
    return ret;
}
nu_vec2_t
nu_vec2_add (nu_vec2_t a, nu_vec2_t b)
{
    nu_vec2_t ret;
    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    return ret;
}
nu_vec2_t
nu_vec2_sub (nu_vec2_t a, nu_vec2_t b)
{
    nu_vec2_t ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    return ret;
}
nu_vec2_t
nu_vec2_mul (nu_vec2_t a, nu_vec2_t b)
{
    nu_vec2_t ret;
    ret.x = a.x * b.x;
    ret.y = a.y * b.y;
    return ret;
}
nu_vec2_t
nu_vec2_muls (nu_vec2_t a, float s)
{
    nu_vec2_t ret;
    ret.x = a.x * s;
    ret.y = a.y * s;
    return ret;
}
nu_vec2_t
nu_vec2_div (nu_vec2_t a, nu_vec2_t b)
{
    nu_vec2_t ret;
    ret.x = a.x / b.x;
    ret.y = a.y / b.y;
    return ret;
}
nu_vec2_t
nu_vec2_divs (nu_vec2_t a, float s)
{
    nu_vec2_t ret;
    ret.x = a.x / s;
    ret.y = a.y / s;
    return ret;
}
nu_vec2_t
nu_vec2_floor (nu_vec2_t a)
{
    nu_vec2_t ret;
    ret.x = nu_floor(a.x);
    ret.y = nu_floor(a.y);
    return ret;
}

nu_vec3_t
nu_vec3 (float x, float y, float z)
{
    nu_vec3_t ret;
    ret.x = x;
    ret.y = y;
    ret.z = z;
    return ret;
}
nu_vec3_t
nu_vec3_normalize (nu_vec3_t v)
{
    float     length = nu_sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    nu_vec3_t ret;
    ret.x = v.x / length;
    ret.y = v.y / length;
    ret.z = v.z / length;
    return ret;
}
nu_vec3_t
nu_vec3_cross (nu_vec3_t a, nu_vec3_t b)
{
    nu_vec3_t ret;
    ret.x = a.y * b.z - a.z * b.y;
    ret.y = a.z * b.x - a.x * b.z;
    ret.z = a.x * b.y - a.y * b.x;
    return ret;
}

nu_vec4_t
nu_vec4 (float x, float y, float z, float w)
{
    nu_vec4_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}
nu_vec2_t
nu_vec4_xy (nu_vec4_t v)
{
    nu_vec2_t ret;
    ret.x = v.x;
    ret.y = v.y;
    return ret;
}
nu_vec2_t
nu_vec4_zw (nu_vec4_t v)
{
    nu_vec2_t ret;
    ret.x = v.z;
    ret.y = v.w;
    return ret;
}

nu_uvec2_t
nu_uvec2 (nu_u32_t x, nu_u32_t y)
{
    nu_uvec2_t v;
    v.x = x;
    v.y = y;
    return v;
}

nu_uvec4_t
nu_uvec4 (nu_u32_t x, nu_u32_t y, nu_u32_t z, nu_u32_t w)
{
    nu_uvec4_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}

nu_mat4_t
nu_mat4_zero (void)
{
    nu_mat4_t m;
    for (nu_size_t i = 0; i < NU_MAT4_SIZE; ++i)
    {
        m.data[i] = 0.0f;
    }
    return m;
}
nu_mat4_t
nu_mat4_identity (void)
{
    nu_mat4_t m = nu_mat4_zero();
    m.data[0]   = 1.0f;
    m.data[5]   = 1.0f;
    m.data[10]  = 1.0f;
    m.data[15]  = 1.0f;
    return m;
}
nu_mat4_t
nu_mat4_mul (nu_mat4_t a, nu_mat4_t b)
{
    nu_mat4_t m = nu_mat4_zero();
    for (nu_size_t i = 0; i < 4; ++i)
    {
        for (nu_size_t j = 0; j < 4; ++j)
        {
            for (nu_size_t k = 0; k < 4; ++k)
            {
                m.data[j * 4 + i] += a.data[k * 4 + i] * b.data[j * 4 + k];
            }
        }
    }
    return m;
}

nu_mat4_t
nu_perspective (float fov, float aspect_ratio, float z_near, float z_far)
{
    nu_mat4_t m;
    float     y_scale  = 1.0f / nu_tan(fov / 2.0f);
    float     x_scale  = y_scale / aspect_ratio;
    float     near_far = z_near - z_far;

    m.data[0] = x_scale;
    m.data[1] = 0.0f;
    m.data[2] = 0.0f;
    m.data[3] = 0.0f;

    m.data[4] = 0.0f;
    m.data[5] = y_scale;
    m.data[6] = 0.0f;
    m.data[7] = 0.0f;

    m.data[8]  = 0.0f;
    m.data[9]  = 0.0f;
    m.data[10] = (z_far + z_near) / near_far;
    m.data[11] = -1.0f;

    m.data[12] = 0.0f;
    m.data[13] = 0.0f;
    m.data[14] = (2.0f * z_far * z_near) / near_far;
    m.data[15] = 0.0f;

    return m;
}
nu_mat4_t
nu_lookat (nu_vec3_t eye, nu_vec3_t center, nu_vec3_t up)
{
    nu_vec3_t f = nu_vec3(center.x - eye.x, center.y - eye.y, center.z - eye.z);
    f           = nu_vec3_normalize(f);

    nu_vec3_t u = nu_vec3_normalize(up);

    nu_vec3_t s = nu_vec3_cross(f, u);
    s           = nu_vec3_normalize(s);

    nu_vec3_t u_prime = nu_vec3_cross(s, f);

    nu_mat4_t m;
    m.data[0] = s.x;
    m.data[1] = u_prime.x;
    m.data[2] = -f.x;
    m.data[3] = 0.0f;

    m.data[4] = s.y;
    m.data[5] = u_prime.y;
    m.data[6] = -f.y;
    m.data[7] = 0.0f;

    m.data[8]  = s.z;
    m.data[9]  = u_prime.z;
    m.data[10] = -f.z;
    m.data[11] = 0.0f;

    m.data[12] = -s.x * eye.x - s.y * eye.y - s.z * eye.z;
    m.data[13] = -u_prime.x * eye.x - u_prime.y * eye.y - u_prime.z * eye.z;
    m.data[14] = f.x * eye.x + f.y * eye.y + f.z * eye.z;
    m.data[15] = 1.0f;
    return m;
}

#endif

#endif
