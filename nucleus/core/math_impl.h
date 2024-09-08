#ifndef NU_MATH_IMPL_H
#define NU_MATH_IMPL_H

#include <nucleus/core/math.h>

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
nu_fabs (float f)
{
#ifdef NU_STDLIB
    return fabsf(f);
#endif
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
nu_pow (float b, float e)
{
#ifdef NU_STDLIB
    return powf(b, e);
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
nu_vec3_add (nu_vec3_t a, nu_vec3_t b)
{
    nu_vec3_t ret;
    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    ret.z = a.z + b.z;
    return ret;
}
nu_vec3_t
nu_vec3_sub (nu_vec3_t a, nu_vec3_t b)
{
    nu_vec3_t ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    ret.z = a.z - b.z;
    return ret;
}
nu_vec3_t
nu_vec3_mul (nu_vec3_t a, nu_vec3_t b)
{
    nu_vec3_t ret;
    ret.x = a.x * b.x;
    ret.y = a.y * b.y;
    ret.z = a.z * b.z;
    return ret;
}
nu_vec3_t
nu_vec3_muls (nu_vec3_t a, float s)
{
    nu_vec3_t ret;
    ret.x = a.x * s;
    ret.y = a.y * s;
    ret.z = a.z * s;
    return ret;
}
nu_vec3_t
nu_vec3_div (nu_vec3_t a, nu_vec3_t b)
{
    nu_vec3_t ret;
    ret.x = a.x / b.x;
    ret.y = a.y / b.y;
    ret.z = a.z / b.z;
    return ret;
}
nu_vec3_t
nu_vec3_divs (nu_vec3_t a, float s)
{
    nu_vec3_t ret;
    ret.x = a.x / s;
    ret.y = a.y / s;
    ret.z = a.z / s;
    return ret;
}
float
nu_vec3_norm (nu_vec3_t a)
{
    return nu_sqrt(nu_vec3_dot(a, a));
}
nu_vec3_t
nu_vec3_normalize (nu_vec3_t v)
{
    float norm = nu_vec3_norm(v);
    if (norm == 0)
    {
        return NU_VEC3_ZERO;
    }
    nu_vec3_t ret;
    ret.x = v.x / norm;
    ret.y = v.y / norm;
    ret.z = v.z / norm;
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
float
nu_vec3_dot (nu_vec3_t a, nu_vec3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
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
float
nu_vec4_dot (nu_vec4_t a, nu_vec4_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
float
nu_vec4_norm (nu_vec4_t v)
{
    return nu_sqrt(nu_vec4_dot(v, v));
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

nu_ivec2_t
nu_ivec2 (nu_i32_t x, nu_i32_t y)
{
    nu_ivec2_t v;
    v.x = x;
    v.y = y;
    return v;
}

nu_uvec2_t
nu_uvec2 (nu_u32_t x, nu_u32_t y)
{
    nu_uvec2_t v;
    v.x = x;
    v.y = y;
    return v;
}
nu_uvec2_t
nu_uvec2_min (nu_uvec2_t a, nu_uvec2_t b)
{
    return nu_uvec2(nu_min(a.x, b.x), nu_min(a.y, b.y));
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

nu_quat_t
nu_quat (float x, float y, float z, float w)
{
    nu_quat_t q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}
nu_quat_t
nu_quat_identity (void)
{
    return nu_quat(0, 0, 0, 1);
}
nu_vec4_t
nu_quat_vec4 (nu_quat_t a)
{
    return nu_vec4(a.x, a.y, a.z, a.w);
}
float
nu_quat_norm (nu_quat_t a)
{
    return nu_vec4_norm(nu_quat_vec4(a));
}
nu_quat_t
nu_quat_axis (nu_vec3_t axis, float angle)
{
    float a = angle * 0.5;
    float c = nu_cos(a);
    float s = nu_sin(a);

    nu_vec3_t k = nu_vec3_normalize(axis);

    return nu_quat(s * k.x, s * k.y, s * k.z, c);
}
nu_quat_t
nu_quat_mul (nu_quat_t a, nu_quat_t b)
{
    nu_quat_t q;
    q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
    q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
    q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
    q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    return q;
}
nu_vec3_t
nu_quat_mulv3 (nu_quat_t a, nu_vec3_t v)
{
    nu_vec3_t u = nu_vec3(a.x, a.y, a.z);

    nu_vec3_t v1 = nu_vec3_muls(u, 2 * nu_vec3_dot(u, v));
    nu_vec3_t v2 = nu_vec3_muls(v, a.w * a.w - nu_vec3_dot(u, u));
    nu_vec3_t v3 = nu_vec3_muls(nu_vec3_cross(u, v), 2 * a.w);

    return nu_vec3_add(v1, nu_vec3_add(v2, v3));
}
nu_quat_t
nu_quat_mul_axis (nu_quat_t q, nu_vec3_t axis, float angle)
{
    return nu_quat_mul(q, nu_quat_axis(axis, angle));
}
nu_mat3_t
nu_quat_mat3 (nu_quat_t q)
{
    float norm = nu_quat_norm(q);
    float s    = norm > 0.0 ? 2.0 / norm : 0.0;

    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;

    float xx = s * x * x;
    float xy = s * x * y;
    float wx = s * w * x;
    float yy = s * y * y;
    float yz = s * y * z;
    float wy = s * w * y;
    float zz = s * z * z;
    float xz = s * x * z;
    float wz = s * w * z;

    nu_mat3_t m;

    m.x1 = 1.0 - yy - zz;
    m.y2 = 1.0 - xx - zz;
    m.z3 = 1.0 - xx - yy;

    m.x2 = xy + wz;
    m.y3 = yz + wx;
    m.z1 = xz + wy;

    m.y1 = xy - wz;
    m.z2 = yz - wx;
    m.x3 = xz - wy;

    return m;
}
nu_mat4_t
nu_quat_mat4 (nu_quat_t q)
{
    float norm = nu_quat_norm(q);
    float s    = norm > 0.0 ? 2.0 / norm : 0.0;

    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;

    float xx = s * x * x;
    float xy = s * x * y;
    float wx = s * w * x;
    float yy = s * y * y;
    float yz = s * y * z;
    float wy = s * w * y;
    float zz = s * z * z;
    float xz = s * x * z;
    float wz = s * w * z;

    nu_mat4_t m;

    m.x1 = 1.0 - yy - zz;
    m.y2 = 1.0 - xx - zz;
    m.z3 = 1.0 - xx - yy;

    m.x2 = xy + wz;
    m.y3 = yz + wx;
    m.z1 = xz + wy;

    m.y1 = xy - wz;
    m.z2 = yz - wx;
    m.x3 = xz - wy;

    m.x4 = 0;
    m.y4 = 0;
    m.z4 = 0;
    m.w1 = 0;
    m.w2 = 0;
    m.w3 = 0;
    m.w4 = 1;

    return m;
}
nu_mat4_t
nu_quat_mulm4 (nu_quat_t a, nu_mat4_t m)
{
    // TODO: allow mat4 multiplication on rotation only
    return nu_mat4_mul(nu_quat_mat4(a), m);
}

nu_mat3_t
nu_mat3_zero (void)
{
    nu_mat3_t m;
    for (nu_size_t i = 0; i < NU_MAT3_SIZE; ++i)
    {
        m.data[i] = 0;
    }
    return m;
}
nu_mat3_t
nu_mat3_identity (void)
{
    nu_mat3_t m = nu_mat3_zero();
    m.x1        = 1;
    m.y2        = 1;
    m.z3        = 1;
    return m;
}
nu_mat3_t
nu_mat3_translate (float x, float y)
{
    nu_mat3_t m = nu_mat3_identity();
    m.z1        = x;
    m.z2        = y;
    return m;
}
nu_mat3_t
nu_mat3_scale (float x, float y)
{
    nu_mat3_t m = nu_mat3_identity();
    m.x1        = x;
    m.y2        = y;
    return m;
}
nu_mat3_t
nu_mat3_mul (nu_mat3_t a, nu_mat3_t b)
{
    nu_mat3_t m = nu_mat3_zero();
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            for (int k = 0; k < 3; ++k)
            {
                m.data[j * 3 + i] += a.data[k * 3 + i] * b.data[j * 3 + k];
            }
        }
    }
    return m;
}

nu_mat4_t
nu_mat4_zero (void)
{
    nu_mat4_t m;
    for (nu_size_t i = 0; i < NU_MAT4_SIZE; ++i)
    {
        m.data[i] = 0;
    }
    return m;
}
nu_mat4_t
nu_mat4_identity (void)
{
    nu_mat4_t m = nu_mat4_zero();
    m.x1        = 1;
    m.y2        = 1;
    m.z3        = 1;
    m.w4        = 1;
    return m;
}
nu_mat4_t
nu_mat4_translate (float x, float y, float z)
{
    nu_mat4_t m = nu_mat4_identity();
    m.w1        = x;
    m.w2        = y;
    m.w3        = z;
    return m;
}
nu_mat4_t
nu_mat4_scale (float x, float y, float z)
{
    nu_mat4_t m = nu_mat4_identity();
    m.x1        = x;
    m.y2        = y;
    m.z3        = z;
    return m;
}
nu_mat4_t
nu_mat4_rotate_y (float angle)
{
    nu_mat4_t m = nu_mat4_identity();
    m.x1        = nu_cos(angle);
    m.z1        = nu_sin(angle);
    m.x3        = -nu_sin(angle);
    m.z3        = nu_cos(angle);
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

nu_rect_t
nu_rect (nu_i32_t x, nu_i32_t y, nu_u32_t w, nu_u32_t h)
{
    nu_rect_t ret;
    ret.p = nu_ivec2(x, y);
    ret.s = nu_uvec2(w, h);
    return ret;
}
nu_bool_t
nu_rect_containsi (nu_rect_t r, nu_ivec2_t p)
{
    nu_i32_t px = p.x;
    nu_i32_t py = p.y;
    if (px < r.p.x || py < r.p.y)
    {
        return NU_FALSE;
    }
    nu_i32_t xw = r.p.x + r.s.x;
    nu_i32_t xh = r.p.y + r.s.y;
    if (px > xw || py > xh)
    {
        return NU_FALSE;
    }
    return NU_TRUE;
}
nu_bool_t
nu_rect_contains (nu_rect_t r, nu_vec2_t p)
{
    return nu_rect_containsi(r, nu_ivec2(p.x, p.y));
}
nu_vec2_t
nu_rect_normalize (nu_rect_t r, nu_vec2_t p)
{
    nu_vec2_t ret;
    ret.x = (p.x - (float)r.p.x) / (float)r.s.x;
    ret.y = (p.y - (float)r.p.y) / (float)r.s.y;
    return ret;
}

nu_mat4_t
nu_perspective (float fov, float aspect_ratio, float z_near, float z_far)
{
    nu_mat4_t m;
    float     y_scale  = 1.0f / nu_tan(fov / 2.0f);
    float     x_scale  = y_scale / aspect_ratio;
    float     near_far = z_near - z_far;

    m.data[0] = x_scale;
    m.data[1] = 0;
    m.data[2] = 0;
    m.data[3] = 0;

    m.data[4] = 0;
    m.data[5] = y_scale;
    m.data[6] = 0;
    m.data[7] = 0;

    m.data[8]  = 0;
    m.data[9]  = 0;
    m.data[10] = (z_far + z_near) / near_far;
    m.data[11] = -1;

    m.data[12] = 0;
    m.data[13] = 0;
    m.data[14] = (2 * z_far * z_near) / near_far;
    m.data[15] = 0;

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
    m.data[3] = 0;

    m.data[4] = s.y;
    m.data[5] = u_prime.y;
    m.data[6] = -f.y;
    m.data[7] = 0;

    m.data[8]  = s.z;
    m.data[9]  = u_prime.z;
    m.data[10] = -f.z;
    m.data[11] = 0;

    m.data[12] = -s.x * eye.x - s.y * eye.y - s.z * eye.z;
    m.data[13] = -u_prime.x * eye.x - u_prime.y * eye.y - u_prime.z * eye.z;
    m.data[14] = f.x * eye.x + f.y * eye.y + f.z * eye.z;
    m.data[15] = 1;
    return m;
}

#endif
