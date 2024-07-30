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

NU_API nu_v2_t nu_v2(float x, float y);
NU_API nu_v2_t nu_v2_add(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_sub(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_mul(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_muls(nu_v2_t a, float s);
NU_API nu_v2_t nu_v2_div(nu_v2_t a, nu_v2_t b);
NU_API nu_v2_t nu_v2_divs(nu_v2_t a, float s);
NU_API nu_v2_t nu_v2_floor(nu_v2_t a);

NU_API nu_v3_t nu_v3(float x, float y, float z);
NU_API nu_v3_t nu_v3_normalize(nu_v3_t a);
NU_API nu_v3_t nu_v3_cross(nu_v3_t a, nu_v3_t b);

NU_API nu_m4_t nu_m4_identity(void);
NU_API nu_m4_t nu_m4_rotate_x(float x);
NU_API nu_m4_t nu_m4_mul(nu_m4_t a, nu_m4_t b);

NU_API nu_m4_t nu_perspective(float fov,
                              float aspect_ratio,
                              float z_near,
                              float z_far);
NU_API nu_m4_t nu_lookat(nu_v3_t eye, nu_v3_t center, nu_v3_t up);

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

nu_v2_t
nu_v2 (float x, float y)
{
    nu_v2_t ret;
    ret.data[0] = x;
    ret.data[1] = y;
    return ret;
}
nu_v2_t
nu_v2_add (nu_v2_t a, nu_v2_t b)
{
    nu_v2_t ret;
    ret.data[0] = a.data[0] + b.data[0];
    ret.data[1] = a.data[1] + b.data[1];
    return ret;
}
nu_v2_t
nu_v2_sub (nu_v2_t a, nu_v2_t b)
{
    nu_v2_t ret;
    ret.data[0] = a.data[0] - b.data[0];
    ret.data[1] = a.data[1] - b.data[1];
    return ret;
}
nu_v2_t
nu_v2_mul (nu_v2_t a, nu_v2_t b)
{
    nu_v2_t ret;
    ret.data[0] = a.data[0] * b.data[0];
    ret.data[1] = a.data[1] * b.data[1];
    return ret;
}
nu_v2_t
nu_v2_muls (nu_v2_t a, float s)
{
    nu_v2_t ret;
    ret.data[0] = a.data[0] * s;
    ret.data[1] = a.data[1] * s;
    return ret;
}
nu_v2_t
nu_v2_div (nu_v2_t a, nu_v2_t b)
{
    nu_v2_t ret;
    ret.data[0] = a.data[0] / b.data[0];
    ret.data[1] = a.data[1] / b.data[1];
    return ret;
}
nu_v2_t
nu_v2_divs (nu_v2_t a, float s)
{
    nu_v2_t ret;
    ret.data[0] = a.data[0] / s;
    ret.data[1] = a.data[1] / s;
    return ret;
}
nu_v2_t
nu_v2_floor (nu_v2_t a)
{
    nu_v2_t ret;
    ret.data[0] = nu_floor(a.data[0]);
    ret.data[1] = nu_floor(a.data[1]);
    return ret;
}

nu_v3_t
nu_v3 (float x, float y, float z)
{
    nu_v3_t ret;
    ret.data[0] = x;
    ret.data[1] = y;
    ret.data[2] = z;
    return ret;
}
nu_v3_t
nu_v3_normalize (nu_v3_t v)
{
    float   length = nu_sqrt(v.data[0] * v.data[0] + v.data[1] * v.data[1]
                           + v.data[2] * v.data[2]);
    nu_v3_t ret;
    ret.data[0] = v.data[0] / length;
    ret.data[1] = v.data[1] / length;
    ret.data[2] = v.data[2] / length;
    return ret;
}
nu_v3_t
nu_v3_cross (nu_v3_t a, nu_v3_t b)
{
    nu_v3_t ret;
    ret.data[0] = a.data[1] * b.data[2] - a.data[2] * b.data[1];
    ret.data[1] = a.data[2] * b.data[0] - a.data[0] * b.data[2];
    ret.data[2] = a.data[0] * b.data[1] - a.data[1] * b.data[0];
    return ret;
}

nu_m4_t
nu_m4_identity (void)
{
    nu_m4_t m;
    for (nu_size_t i = 0; i < NU_M4; ++i)
    {
        m.data[i] = 0.0f;
    }
    m.data[0]  = 1.0f;
    m.data[5]  = 1.0f;
    m.data[10] = 1.0f;
    m.data[15] = 1.0f;
    return m;
}
nu_m4_t
nu_m4_rotate_x (float x)
{
    nu_m4_t m;
    return m;
}
nu_m4_t
nu_m4_mul (nu_m4_t a, nu_m4_t b)
{
    nu_m4_t m = NU_M4_ZERO;
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

nu_m4_t
nu_perspective (float fov, float aspect_ratio, float z_near, float z_far)
{
    nu_m4_t m;
    float   y_scale  = 1.0f / nu_tan(fov / 2.0f);
    float   x_scale  = y_scale / aspect_ratio;
    float   near_far = z_near - z_far;

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
nu_m4_t
nu_lookat (nu_v3_t eye, nu_v3_t center, nu_v3_t up)
{
    nu_v3_t f = nu_v3(center.data[0] - eye.data[0],
                      center.data[1] - eye.data[1],
                      center.data[2] - eye.data[2]);
    f         = nu_v3_normalize(f);

    nu_v3_t u = nu_v3_normalize(up);

    nu_v3_t s = nu_v3_cross(f, u);
    s         = nu_v3_normalize(s);

    nu_v3_t u_prime = nu_v3_cross(s, f);

    nu_m4_t m;
    m.data[0] = s.data[0];
    m.data[1] = u_prime.data[0];
    m.data[2] = -f.data[0];
    m.data[3] = 0.0f;

    m.data[4] = s.data[1];
    m.data[5] = u_prime.data[1];
    m.data[6] = -f.data[1];
    m.data[7] = 0.0f;

    m.data[8]  = s.data[2];
    m.data[9]  = u_prime.data[2];
    m.data[10] = -f.data[2];
    m.data[11] = 0.0f;

    m.data[12] = -s.data[0] * eye.data[0] - s.data[1] * eye.data[1]
                 - s.data[2] * eye.data[2];
    m.data[13] = -u_prime.data[0] * eye.data[0] - u_prime.data[1] * eye.data[1]
                 - u_prime.data[2] * eye.data[2];
    m.data[14] = f.data[0] * eye.data[0] + f.data[1] * eye.data[1]
                 + f.data[2] * eye.data[2];
    m.data[15] = 1.0f;
}

#endif

#endif
