#ifndef NU_MATH_IMPL_H
#define NU_MATH_IMPL_H

#include <nucleus/internal.h>

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
float
nu_exp (float x)
{
#ifdef NU_STDLIB
    return expf(x);
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
nu_vec2_v2u (nu_vec2u_t v)
{
    return nu_vec2(v.x, v.y);
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
nu_vec3s (float s)
{
    return nu_vec3(s, s, s);
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
        return NU_VEC3_ZEROS;
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
nu_vec3_t
nu_vec3_min (nu_vec3_t a, nu_vec3_t b)
{
    nu_vec3_t v;
    v.x = NU_MIN(a.x, b.x);
    v.y = NU_MIN(a.y, b.y);
    v.z = NU_MIN(a.z, b.z);
    return v;
}
nu_vec3_t
nu_vec3_max (nu_vec3_t a, nu_vec3_t b)
{
    nu_vec3_t v;
    v.x = NU_MAX(a.x, b.x);
    v.y = NU_MAX(a.y, b.y);
    v.z = NU_MAX(a.z, b.z);
    return v;
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

nu_vec2i_t
nu_vec2i (nu_i32_t x, nu_i32_t y)
{
    nu_vec2i_t v;
    v.x = x;
    v.y = y;
    return v;
}
nu_vec2i_t
nu_vec2i_add (nu_vec2i_t a, nu_vec2i_t b)
{
    nu_vec2i_t ret;
    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    return ret;
}
nu_vec2i_t
nu_vec2i_sub (nu_vec2i_t a, nu_vec2i_t b)
{
    nu_vec2i_t ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    return ret;
}
nu_vec2i_t
nu_vec2i_min (nu_vec2i_t a, nu_vec2i_t b)
{
    nu_vec2i_t ret;
    ret.x = NU_MIN(a.x, b.x);
    ret.y = NU_MIN(a.y, b.y);
    return ret;
}
nu_vec2i_t
nu_vec2i_max (nu_vec2i_t a, nu_vec2i_t b)
{
    nu_vec2i_t ret;
    ret.x = NU_MAX(a.x, b.x);
    ret.y = NU_MAX(a.y, b.y);
    return ret;
}

nu_vec2u_t
nu_vec2u (nu_u32_t x, nu_u32_t y)
{
    nu_vec2u_t v;
    v.x = x;
    v.y = y;
    return v;
}
nu_vec2u_t
nu_vec2u_min (nu_vec2u_t a, nu_vec2u_t b)
{
    return nu_vec2u(NU_MIN(a.x, b.x), NU_MIN(a.y, b.y));
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
nu_mat3_translate (nu_vec2_t v)
{
    nu_mat3_t m = nu_mat3_identity();
    m.z1        = v.x;
    m.z2        = v.y;
    return m;
}
nu_mat3_t
nu_mat3_scale (nu_vec2_t v)
{
    nu_mat3_t m = nu_mat3_identity();
    m.x1        = v.x;
    m.y2        = v.y;
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
nu_mat4_translate (nu_vec3_t v)
{
    nu_mat4_t m = nu_mat4_identity();
    m.w1        = v.x;
    m.w2        = v.y;
    m.w3        = v.z;
    return m;
}
nu_mat4_t
nu_mat4_scale (nu_vec3_t v)
{
    nu_mat4_t m = nu_mat4_identity();
    m.x1        = v.x;
    m.y2        = v.y;
    m.z3        = v.z;
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
nu_vec4_t
nu_mat4_mulv (nu_mat4_t m, nu_vec4_t v)
{
    nu_vec4_t ret;
    ret.x = m.x1 * v.x + m.y1 * v.y + m.z1 * v.z + m.w1 * v.w;
    ret.y = m.x2 * v.x + m.y2 * v.y + m.z2 * v.z + m.w2 * v.w;
    ret.z = m.x3 * v.x + m.y3 * v.y + m.z3 * v.z + m.w3 * v.w;
    ret.w = m.x4 * v.x + m.y4 * v.y + m.z4 * v.z + m.w4 * v.w;
    return ret;
}
nu_vec3_t
nu_mat4_mulv3 (nu_mat4_t a, nu_vec3_t v)
{
    nu_vec4_t v4 = nu_vec4(v.x, v.y, v.z, 1);
    v4           = nu_mat4_mulv(a, v4);
    return nu_vec3(v4.x, v4.y, v4.z);
}

nu_box2i_t
nu_box2i (nu_vec2i_t min, nu_vec2i_t max)
{
    nu_box2i_t ret;
    ret.min = min;
    ret.max = max;
    return ret;
}
nu_box2i_t
nu_box2i_xywh (nu_i32_t x, nu_i32_t y, nu_u32_t w, nu_u32_t h)
{
    NU_ASSERT(w && h);
    nu_box2i_t ret;
    ret.min = nu_vec2i(x, y);
    ret.max = nu_vec2i(x + w - 1, y + h - 1);
    return ret;
}
nu_vec2u_t
nu_box2i_size (nu_box2i_t b)
{
    NU_ASSERT((b.max.x - b.min.x) >= 0 && (b.max.y - b.min.y) >= 0);
    return nu_vec2u(b.max.x - b.min.x + 1, b.max.y - b.min.y + 1);
}
nu_box2i_t
nu_box2i_resize (nu_box2i_t b, nu_vec2u_t size)
{
    NU_ASSERT(size.x && size.y);
    return nu_box2i(b.min,
                    nu_vec2i(b.min.x + size.x - 1, b.min.y + size.y - 1));
}
nu_vec2i_t
nu_box2i_tr (nu_box2i_t b)
{
    return nu_vec2i(b.max.x, b.min.y);
}
nu_vec2i_t
nu_box2i_bl (nu_box2i_t b)
{
    return nu_vec2i(b.min.x, b.max.y);
}

nu_box2i_t
nu_box2i_translate (nu_box2i_t b, nu_vec2i_t t)
{
    return nu_box2i(nu_vec2i_add(b.min, t), nu_vec2i_add(b.max, t));
}
nu_box2i_t
nu_box2i_moveto (nu_box2i_t b, nu_vec2i_t p)
{
    return nu_box2i_translate(b, nu_vec2i_sub(p, b.min));
}
nu_box2i_t
nu_box2i_union (nu_box2i_t a, nu_box2i_t b)
{
    return nu_box2i(nu_vec2i_max(a.min, b.min), nu_vec2i_min(a.max, b.max));
}
nu_bool_t
nu_box2i_containsi (nu_box2i_t b, nu_vec2i_t p)
{
    return (p.x >= b.min.x && p.x <= b.max.x && p.y >= b.min.y
            && p.y <= b.max.y);
}
nu_bool_t
nu_box2i_contains (nu_box2i_t b, nu_vec2_t p)
{
    return nu_box2i_containsi(b, nu_vec2i(p.x, p.y));
}
nu_vec2_t
nu_box2i_normalize (nu_box2i_t b, nu_vec2_t p)
{
    nu_vec2_t  ret;
    nu_vec2u_t size = nu_box2i_size(b);
    ret.x           = (p.x - (float)b.min.x) / (float)(size.x);
    ret.y           = (p.y - (float)b.min.y) / (float)(size.y);
    return ret;
}

nu_box3_t
nu_box3 (nu_vec3_t min, nu_vec3_t max)
{
    NU_ASSERT(max.x >= min.x && max.y >= min.y && max.z >= min.z);
    nu_box3_t b;
    b.min = min;
    b.max = max;
    return b;
}
nu_vec3_t
nu_box3_center (nu_box3_t b)
{
    return nu_vec3_muls(nu_vec3_sub(b.max, b.min), 0.5);
}
nu_bool_t
nu_box3_contains (nu_box3_t b, nu_vec3_t p)
{
    return (p.x >= b.min.x && p.y >= b.min.y && p.z >= b.min.z && p.x <= b.max.x
            && p.y <= b.max.y && p.z <= b.max.z);
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
nu_ortho (
    float left, float right, float bottom, float top, float near, float far)
{
    nu_mat4_t m;

    float rl = 1.0 / (right - left);
    float tb = 1.0 / (top - bottom);
    float fn = -1.0 / (far - near);

    m.data[0] = 2.0 * rl;
    m.data[1] = 0;
    m.data[2] = 0;
    m.data[3] = 0;

    m.data[4] = 0;
    m.data[5] = 2.0 * tb;
    m.data[6] = 0;
    m.data[7] = 0;

    m.data[8]  = 0;
    m.data[9]  = 0;
    m.data[10] = 2.0 * fn;
    m.data[11] = 0;

    m.data[12] = -(right + left) * rl;
    m.data[13] = -(top + bottom) * tb;
    m.data[14] = (far + near) * fn;
    m.data[15] = 1;

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

nu_vec3_t
nu_axis3d (float     pos_x,
           float     neg_x,
           float     pos_y,
           float     neg_y,
           float     pos_z,
           float     neg_z,
           nu_bool_t normalize)
{
    nu_vec3_t ax;

    ax.x = neg_x - pos_x;
    ax.y = pos_y - neg_y;
    ax.z = pos_z - neg_z;
    if (normalize)
    {
        ax = nu_vec3_normalize(ax);
    }

    return ax;
}

nu_vec3_t
nu_triangle_normal (nu_vec3_t p0, nu_vec3_t p1, nu_vec3_t p2)
{
    return nu_vec3_normalize(
        nu_vec3_cross(nu_vec3_sub(p2, p0), nu_vec3_sub(p0, p1)));
}

#endif
