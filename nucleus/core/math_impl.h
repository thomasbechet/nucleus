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
nu_f32_t
nu_fabs (nu_f32_t f)
{
#ifdef NU_STDLIB
    return fabsf(f);
#endif
}
nu_f32_t
nu_floor (nu_f32_t f)
{
#ifdef NU_STDLIB
    return floorf(f);
#endif
}
nu_f32_t
nu_radian (nu_f32_t d)
{
    return d * (NU_PI / 180.0f);
}
nu_f32_t
nu_sqrt (nu_f32_t x)
{
#ifdef NU_STDLIB
    return sqrtf(x);
#endif
}
nu_f32_t
nu_pow (nu_f32_t b, nu_f32_t e)
{
#ifdef NU_STDLIB
    return powf(b, e);
#endif
}
nu_f32_t
nu_cos (nu_f32_t x)
{
#ifdef NU_STDLIB
    return cosf(x);
#endif
}
nu_f32_t
nu_sin (nu_f32_t x)
{
#ifdef NU_STDLIB
    return sinf(x);
#endif
}
nu_f32_t
nu_tan (nu_f32_t x)
{
#ifdef NU_STDLIB
    return tanf(x);
#endif
}
nu_f32_t
nu_exp (nu_f32_t x)
{
#ifdef NU_STDLIB
    return expf(x);
#endif
}

nu_v2_t
nu_v2 (nu_f32_t x, nu_f32_t y)
{
    nu_v2_t ret;
    ret.x = x;
    ret.y = y;
    return ret;
}
nu_v2_t
nu_v2_v2u (nu_v2u_t v)
{
    return nu_v2(v.x, v.y);
}
nu_v2_t
nu_v2_add (nu_v2_t a, nu_v2_t b)
{
    nu_v2_t ret;
    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    return ret;
}
nu_v2_t
nu_v2_sub (nu_v2_t a, nu_v2_t b)
{
    nu_v2_t ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    return ret;
}
nu_v2_t
nu_v2_mul (nu_v2_t a, nu_v2_t b)
{
    nu_v2_t ret;
    ret.x = a.x * b.x;
    ret.y = a.y * b.y;
    return ret;
}
nu_v2_t
nu_v2_muls (nu_v2_t a, nu_f32_t s)
{
    nu_v2_t ret;
    ret.x = a.x * s;
    ret.y = a.y * s;
    return ret;
}
nu_v2_t
nu_v2_div (nu_v2_t a, nu_v2_t b)
{
    nu_v2_t ret;
    ret.x = a.x / b.x;
    ret.y = a.y / b.y;
    return ret;
}
nu_v2_t
nu_v2_divs (nu_v2_t a, nu_f32_t s)
{
    nu_v2_t ret;
    ret.x = a.x / s;
    ret.y = a.y / s;
    return ret;
}
nu_v2_t
nu_v2_floor (nu_v2_t a)
{
    nu_v2_t ret;
    ret.x = nu_floor(a.x);
    ret.y = nu_floor(a.y);
    return ret;
}

nu_v3_t
nu_v3 (nu_f32_t x, nu_f32_t y, nu_f32_t z)
{
    nu_v3_t ret;
    ret.x = x;
    ret.y = y;
    ret.z = z;
    return ret;
}
nu_v3_t
nu_v3s (nu_f32_t s)
{
    return nu_v3(s, s, s);
}
nu_v3_t
nu_v3_add (nu_v3_t a, nu_v3_t b)
{
    nu_v3_t ret;
    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    ret.z = a.z + b.z;
    return ret;
}
nu_v3_t
nu_v3_sub (nu_v3_t a, nu_v3_t b)
{
    nu_v3_t ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    ret.z = a.z - b.z;
    return ret;
}
nu_v3_t
nu_v3_mul (nu_v3_t a, nu_v3_t b)
{
    nu_v3_t ret;
    ret.x = a.x * b.x;
    ret.y = a.y * b.y;
    ret.z = a.z * b.z;
    return ret;
}
nu_v3_t
nu_v3_muls (nu_v3_t a, nu_f32_t s)
{
    nu_v3_t ret;
    ret.x = a.x * s;
    ret.y = a.y * s;
    ret.z = a.z * s;
    return ret;
}
nu_v3_t
nu_v3_div (nu_v3_t a, nu_v3_t b)
{
    nu_v3_t ret;
    ret.x = a.x / b.x;
    ret.y = a.y / b.y;
    ret.z = a.z / b.z;
    return ret;
}
nu_v3_t
nu_v3_divs (nu_v3_t a, nu_f32_t s)
{
    nu_v3_t ret;
    ret.x = a.x / s;
    ret.y = a.y / s;
    ret.z = a.z / s;
    return ret;
}
nu_f32_t
nu_v3_norm (nu_v3_t a)
{
    return nu_sqrt(nu_v3_dot(a, a));
}
nu_v3_t
nu_v3_normalize (nu_v3_t v)
{
    nu_f32_t norm = nu_v3_norm(v);
    if (norm == 0)
    {
        return NU_V3_ZEROS;
    }
    nu_v3_t ret;
    ret.x = v.x / norm;
    ret.y = v.y / norm;
    ret.z = v.z / norm;
    return ret;
}
nu_v3_t
nu_v3_cross (nu_v3_t a, nu_v3_t b)
{
    nu_v3_t ret;
    ret.x = a.y * b.z - a.z * b.y;
    ret.y = a.z * b.x - a.x * b.z;
    ret.z = a.x * b.y - a.y * b.x;
    return ret;
}
nu_f32_t
nu_v3_dot (nu_v3_t a, nu_v3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
nu_v3_t
nu_v3_min (nu_v3_t a, nu_v3_t b)
{
    nu_v3_t v;
    v.x = NU_MIN(a.x, b.x);
    v.y = NU_MIN(a.y, b.y);
    v.z = NU_MIN(a.z, b.z);
    return v;
}
nu_v3_t
nu_v3_max (nu_v3_t a, nu_v3_t b)
{
    nu_v3_t v;
    v.x = NU_MAX(a.x, b.x);
    v.y = NU_MAX(a.y, b.y);
    v.z = NU_MAX(a.z, b.z);
    return v;
}

nu_v4_t
nu_v4 (nu_f32_t x, nu_f32_t y, nu_f32_t z, nu_f32_t w)
{
    nu_v4_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}
nu_f32_t
nu_v4_dot (nu_v4_t a, nu_v4_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
nu_f32_t
nu_v4_norm (nu_v4_t v)
{
    return nu_sqrt(nu_v4_dot(v, v));
}
nu_v2_t
nu_v4_xy (nu_v4_t v)
{
    nu_v2_t ret;
    ret.x = v.x;
    ret.y = v.y;
    return ret;
}
nu_v2_t
nu_v4_zw (nu_v4_t v)
{
    nu_v2_t ret;
    ret.x = v.z;
    ret.y = v.w;
    return ret;
}

nu_v2i_t
nu_v2i (nu_i32_t x, nu_i32_t y)
{
    nu_v2i_t v;
    v.x = x;
    v.y = y;
    return v;
}
nu_v2i_t
nu_v2i_add (nu_v2i_t a, nu_v2i_t b)
{
    nu_v2i_t ret;
    ret.x = a.x + b.x;
    ret.y = a.y + b.y;
    return ret;
}
nu_v2i_t
nu_v2i_sub (nu_v2i_t a, nu_v2i_t b)
{
    nu_v2i_t ret;
    ret.x = a.x - b.x;
    ret.y = a.y - b.y;
    return ret;
}
nu_v2i_t
nu_v2i_min (nu_v2i_t a, nu_v2i_t b)
{
    nu_v2i_t ret;
    ret.x = NU_MIN(a.x, b.x);
    ret.y = NU_MIN(a.y, b.y);
    return ret;
}
nu_v2i_t
nu_v2i_max (nu_v2i_t a, nu_v2i_t b)
{
    nu_v2i_t ret;
    ret.x = NU_MAX(a.x, b.x);
    ret.y = NU_MAX(a.y, b.y);
    return ret;
}

nu_v2u_t
nu_v2u (nu_u32_t x, nu_u32_t y)
{
    nu_v2u_t v;
    v.x = x;
    v.y = y;
    return v;
}
nu_v2u_t
nu_v2u_min (nu_v2u_t a, nu_v2u_t b)
{
    return nu_v2u(NU_MIN(a.x, b.x), NU_MIN(a.y, b.y));
}

nu_v3u_t
nu_v3u (nu_u32_t x, nu_u32_t y, nu_u32_t z)
{
    nu_v3u_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}
nu_v3u_t
nu_v3u_v2u (nu_v2u_t v, nu_u32_t z)
{
    return nu_v3u(v.x, v.y, z);
}
nu_v2u_t
nu_v3u_xy (nu_v3u_t v)
{
    return nu_v2u(v.x, v.y);
}

nu_v4u_t
nu_v4u (nu_u32_t x, nu_u32_t y, nu_u32_t z, nu_u32_t w)
{
    nu_v4u_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    v.w = w;
    return v;
}

nu_q4_t
nu_q4 (nu_f32_t x, nu_f32_t y, nu_f32_t z, nu_f32_t w)
{
    nu_q4_t q;
    q.x = x;
    q.y = y;
    q.z = z;
    q.w = w;
    return q;
}
nu_q4_t
nu_q4_identity (void)
{
    return nu_q4(0, 0, 0, 1);
}
nu_v4_t
nu_q4_vec4 (nu_q4_t a)
{
    return nu_v4(a.x, a.y, a.z, a.w);
}
nu_f32_t
nu_q4_norm (nu_q4_t a)
{
    return nu_v4_norm(nu_q4_vec4(a));
}
nu_q4_t
nu_q4_axis (nu_v3_t axis, nu_f32_t angle)
{
    nu_f32_t a = angle * 0.5;
    nu_f32_t c = nu_cos(a);
    nu_f32_t s = nu_sin(a);

    nu_v3_t k = nu_v3_normalize(axis);

    return nu_q4(s * k.x, s * k.y, s * k.z, c);
}
nu_q4_t
nu_q4_mul (nu_q4_t a, nu_q4_t b)
{
    nu_q4_t q;
    q.x = a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y;
    q.y = a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x;
    q.z = a.w * b.z + a.x * b.y - a.y * b.x + a.z * b.w;
    q.w = a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z;
    return q;
}
nu_v3_t
nu_q4_mulv3 (nu_q4_t a, nu_v3_t v)
{
    nu_v3_t u = nu_v3(a.x, a.y, a.z);

    nu_v3_t v1 = nu_v3_muls(u, 2 * nu_v3_dot(u, v));
    nu_v3_t v2 = nu_v3_muls(v, a.w * a.w - nu_v3_dot(u, u));
    nu_v3_t v3 = nu_v3_muls(nu_v3_cross(u, v), 2 * a.w);

    return nu_v3_add(v1, nu_v3_add(v2, v3));
}
nu_q4_t
nu_q4_mul_axis (nu_q4_t q, nu_v3_t axis, nu_f32_t angle)
{
    return nu_q4_mul(q, nu_q4_axis(axis, angle));
}
nu_m3_t
nu_q4_mat3 (nu_q4_t q)
{
    nu_f32_t norm = nu_q4_norm(q);
    nu_f32_t s    = norm > 0.0 ? 2.0 / norm : 0.0;

    nu_f32_t x = q.x;
    nu_f32_t y = q.y;
    nu_f32_t z = q.z;
    nu_f32_t w = q.w;

    nu_f32_t xx = s * x * x;
    nu_f32_t xy = s * x * y;
    nu_f32_t wx = s * w * x;
    nu_f32_t yy = s * y * y;
    nu_f32_t yz = s * y * z;
    nu_f32_t wy = s * w * y;
    nu_f32_t zz = s * z * z;
    nu_f32_t xz = s * x * z;
    nu_f32_t wz = s * w * z;

    nu_m3_t m;

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
nu_m4_t
nu_q4_mat4 (nu_q4_t q)
{
    nu_f32_t norm = nu_q4_norm(q);
    nu_f32_t s    = norm > 0.0 ? 2.0 / norm : 0.0;

    nu_f32_t x = q.x;
    nu_f32_t y = q.y;
    nu_f32_t z = q.z;
    nu_f32_t w = q.w;

    nu_f32_t xx = s * x * x;
    nu_f32_t xy = s * x * y;
    nu_f32_t wx = s * w * x;
    nu_f32_t yy = s * y * y;
    nu_f32_t yz = s * y * z;
    nu_f32_t wy = s * w * y;
    nu_f32_t zz = s * z * z;
    nu_f32_t xz = s * x * z;
    nu_f32_t wz = s * w * z;

    nu_m4_t m;

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
nu_m4_t
nu_q4_mulm4 (nu_q4_t a, nu_m4_t m)
{
    // TODO: allow mat4 multiplication on rotation only
    return nu_m4_mul(nu_q4_mat4(a), m);
}

nu_m3_t
nu_m3_zero (void)
{
    nu_m3_t m;
    for (nu_size_t i = 0; i < NU_M3_SIZE; ++i)
    {
        m.data[i] = 0;
    }
    return m;
}
nu_m3_t
nu_m3_identity (void)
{
    nu_m3_t m = nu_m3_zero();
    m.x1      = 1;
    m.y2      = 1;
    m.z3      = 1;
    return m;
}
nu_m3_t
nu_m3_translate (nu_v2_t v)
{
    nu_m3_t m = nu_m3_identity();
    m.z1      = v.x;
    m.z2      = v.y;
    return m;
}
nu_m3_t
nu_m3_scale (nu_v2_t v)
{
    nu_m3_t m = nu_m3_identity();
    m.x1      = v.x;
    m.y2      = v.y;
    return m;
}
nu_m3_t
nu_m3_mul (nu_m3_t a, nu_m3_t b)
{
    nu_m3_t m = nu_m3_zero();
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

nu_m4_t
nu_m4_zero (void)
{
    nu_m4_t m;
    for (nu_size_t i = 0; i < NU_M4_SIZE; ++i)
    {
        m.data[i] = 0;
    }
    return m;
}
nu_m4_t
nu_m4_identity (void)
{
    nu_m4_t m = nu_m4_zero();
    m.x1      = 1;
    m.y2      = 1;
    m.z3      = 1;
    m.w4      = 1;
    return m;
}
nu_m4_t
nu_m4_translate (nu_v3_t v)
{
    nu_m4_t m = nu_m4_identity();
    m.w1      = v.x;
    m.w2      = v.y;
    m.w3      = v.z;
    return m;
}
nu_m4_t
nu_m4_scale (nu_v3_t v)
{
    nu_m4_t m = nu_m4_identity();
    m.x1      = v.x;
    m.y2      = v.y;
    m.z3      = v.z;
    return m;
}
nu_m4_t
nu_m4_rotate_y (nu_f32_t angle)
{
    nu_m4_t m = nu_m4_identity();
    m.x1      = nu_cos(angle);
    m.z1      = nu_sin(angle);
    m.x3      = -nu_sin(angle);
    m.z3      = nu_cos(angle);
    return m;
}
nu_m4_t
nu_m4_mul (nu_m4_t a, nu_m4_t b)
{
    nu_m4_t m = nu_m4_zero();
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
nu_v4_t
nu_m4_mulv (nu_m4_t m, nu_v4_t v)
{
    nu_v4_t ret;
    ret.x = m.x1 * v.x + m.y1 * v.y + m.z1 * v.z + m.w1 * v.w;
    ret.y = m.x2 * v.x + m.y2 * v.y + m.z2 * v.z + m.w2 * v.w;
    ret.z = m.x3 * v.x + m.y3 * v.y + m.z3 * v.z + m.w3 * v.w;
    ret.w = m.x4 * v.x + m.y4 * v.y + m.z4 * v.z + m.w4 * v.w;
    return ret;
}
nu_v3_t
nu_m4_mulv3 (nu_m4_t a, nu_v3_t v)
{
    nu_v4_t v4 = nu_v4(v.x, v.y, v.z, 1);
    v4         = nu_m4_mulv(a, v4);
    return nu_v3(v4.x, v4.y, v4.z);
}

nu_b2i_t
nu_b2i (nu_v2i_t min, nu_v2i_t max)
{
    nu_b2i_t ret;
    ret.min = min;
    ret.max = max;
    return ret;
}
nu_b2i_t
nu_b2i_xywh (nu_i32_t x, nu_i32_t y, nu_u32_t w, nu_u32_t h)
{
    NU_ASSERT(w && h);
    nu_b2i_t ret;
    ret.min = nu_v2i(x, y);
    ret.max = nu_v2i(x + w - 1, y + h - 1);
    return ret;
}
nu_v2u_t
nu_b2i_size (nu_b2i_t b)
{
    NU_ASSERT((b.max.x - b.min.x) >= 0 && (b.max.y - b.min.y) >= 0);
    return nu_v2u(b.max.x - b.min.x + 1, b.max.y - b.min.y + 1);
}
nu_b2i_t
nu_b2i_resize (nu_b2i_t b, nu_v2u_t size)
{
    NU_ASSERT(size.x && size.y);
    return nu_b2i(b.min, nu_v2i(b.min.x + size.x - 1, b.min.y + size.y - 1));
}
nu_v2i_t
nu_b2i_tr (nu_b2i_t b)
{
    return nu_v2i(b.max.x, b.min.y);
}
nu_v2i_t
nu_b2i_bl (nu_b2i_t b)
{
    return nu_v2i(b.min.x, b.max.y);
}

nu_b2i_t
nu_b2i_translate (nu_b2i_t b, nu_v2i_t t)
{
    return nu_b2i(nu_v2i_add(b.min, t), nu_v2i_add(b.max, t));
}
nu_b2i_t
nu_b2i_moveto (nu_b2i_t b, nu_v2i_t p)
{
    return nu_b2i_translate(b, nu_v2i_sub(p, b.min));
}
nu_b2i_t
nu_b2i_union (nu_b2i_t a, nu_b2i_t b)
{
    return nu_b2i(nu_v2i_max(a.min, b.min), nu_v2i_min(a.max, b.max));
}
nu_bool_t
nu_b2i_containsi (nu_b2i_t b, nu_v2i_t p)
{
    return (p.x >= b.min.x && p.x <= b.max.x && p.y >= b.min.y
            && p.y <= b.max.y);
}
nu_bool_t
nu_b2i_contains (nu_b2i_t b, nu_v2_t p)
{
    return nu_b2i_containsi(b, nu_v2i(p.x, p.y));
}
nu_v2_t
nu_b2i_normalize (nu_b2i_t b, nu_v2_t p)
{
    nu_v2_t  ret;
    nu_v2u_t size = nu_b2i_size(b);
    ret.x         = (p.x - (nu_f32_t)b.min.x) / (nu_f32_t)(size.x);
    ret.y         = (p.y - (nu_f32_t)b.min.y) / (nu_f32_t)(size.y);
    return ret;
}

nu_b3_t
nu_b3 (nu_v3_t min, nu_v3_t max)
{
    NU_ASSERT(max.x >= min.x && max.y >= min.y && max.z >= min.z);
    nu_b3_t b;
    b.min = min;
    b.max = max;
    return b;
}
nu_v3_t
nu_b3_center (nu_b3_t b)
{
    return nu_v3_muls(nu_v3_sub(b.max, b.min), 0.5);
}
nu_bool_t
nu_b3_contains (nu_b3_t b, nu_v3_t p)
{
    return (p.x >= b.min.x && p.y >= b.min.y && p.z >= b.min.z && p.x <= b.max.x
            && p.y <= b.max.y && p.z <= b.max.z);
}

nu_m4_t
nu_perspective (nu_f32_t fov,
                nu_f32_t aspect_ratio,
                nu_f32_t z_near,
                nu_f32_t z_far)
{
    nu_m4_t  m;
    nu_f32_t y_scale  = 1.0f / nu_tan(fov / 2.0f);
    nu_f32_t x_scale  = y_scale / aspect_ratio;
    nu_f32_t near_far = z_near - z_far;

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
nu_m4_t
nu_ortho (nu_f32_t left,
          nu_f32_t right,
          nu_f32_t bottom,
          nu_f32_t top,
          nu_f32_t nnear,
          nu_f32_t far)
{
    nu_m4_t m;

    nu_f32_t rl = 1.0 / (right - left);
    nu_f32_t tb = 1.0 / (top - bottom);
    nu_f32_t fn = -1.0 / (far - nnear);

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
    m.data[14] = (far + nnear) * fn;
    m.data[15] = 1;

    return m;
}
nu_m4_t
nu_lookat (nu_v3_t eye, nu_v3_t center, nu_v3_t up)
{
    nu_v3_t f = nu_v3(center.x - eye.x, center.y - eye.y, center.z - eye.z);
    f         = nu_v3_normalize(f);

    nu_v3_t u = nu_v3_normalize(up);

    nu_v3_t s = nu_v3_cross(f, u);
    s         = nu_v3_normalize(s);

    nu_v3_t u_prime = nu_v3_cross(s, f);

    nu_m4_t m;
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

nu_v3_t
nu_axis3d (nu_f32_t  pos_x,
           nu_f32_t  neg_x,
           nu_f32_t  pos_y,
           nu_f32_t  neg_y,
           nu_f32_t  pos_z,
           nu_f32_t  neg_z,
           nu_bool_t normalize)
{
    nu_v3_t ax;

    ax.x = neg_x - pos_x;
    ax.y = pos_y - neg_y;
    ax.z = pos_z - neg_z;
    if (normalize)
    {
        ax = nu_v3_normalize(ax);
    }

    return ax;
}

nu_v3_t
nu_triangle_normal (nu_v3_t p0, nu_v3_t p1, nu_v3_t p2)
{
    return nu_v3_normalize(nu_v3_cross(nu_v3_sub(p2, p0), nu_v3_sub(p0, p1)));
}

#endif
