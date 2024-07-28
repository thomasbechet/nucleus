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

NU_API void nu_v2_copy(const float *s, float *d);
NU_API void nu_v2_zero(float *v);
NU_API void nu_v2_add(const float *a, const float *b, float *d);
NU_API void nu_v2_sub(const float *a, const float *b, float *d);
NU_API void nu_v2_mul(const float *a, const float *b, float *d);
NU_API void nu_v2_muls(const float *a, float s, float *d);
NU_API void nu_v2_div(const float *a, const float *b, float *d);
NU_API void nu_v2_divs(const float *a, float s, float *d);
NU_API void nu_v2_floor(const float *a, float *d);

NU_API void nu_v3_copy(const float *a, float *d);
NU_API void nu_v3_zero(float *d);
NU_API void nu_v3_norm(const float *v, float *d);
NU_API void nu_v3_cross(const float *a, const float *b, float *d);

NU_API void nu_v4_zero(float *v);

NU_API void nu_iv2_zero(nu_i32_t *v);
NU_API void nu_iv2_copy(const nu_i32_t *a, nu_i32_t *d);

NU_API void nu_iv4_copy(const nu_i32_t *a, nu_i32_t *d);

NU_API void nu_m4_identity(float *m);
NU_API void nu_m4_zero(float *m);
NU_API void nu_m4_rotate_x(float x, float *d);
NU_API
void nu_m4_mul(const float *a, const float *b, float *d);

NU_API void nu_perspective(
    float fov, float aspect_ratio, float z_near, float z_far, float *m);
NU_API void nu_lookat(const float *eye,
                      const float *center,
                      const float *up,
                      float       *m);

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

void
nu_v2_copy (const float *s, float *d)
{
    d[0] = s[0];
    d[1] = s[1];
}
void
nu_v2_zero (float *v)
{
    v[0] = 0.0f;
    v[1] = 0.0f;
}
void
nu_v2_add (const float *a, const float *b, float *d)
{
    d[0] = a[0] + b[0];
    d[1] = a[1] + b[1];
}
void
nu_v2_sub (const float *a, const float *b, float *d)
{
    d[0] = a[0] - b[0];
    d[1] = a[1] - b[1];
}
void
nu_v2_mul (const float *a, const float *b, float *d)
{
    d[0] = a[0] * b[0];
    d[1] = a[1] * b[1];
}
void
nu_v2_muls (const float *a, float s, float *d)
{
    d[0] = a[0] * s;
    d[1] = a[1] * s;
}
void
nu_v2_div (const float *a, const float *b, float *d)
{
    d[0] = a[0] / b[0];
    d[1] = a[1] / b[1];
}
void
nu_v2_divs (const float *a, float s, float *d)
{
    d[0] = a[0] / s;
    d[1] = a[1] / s;
}
void
nu_v2_floor (const float *a, float *d)
{
    d[0] = nu_floor(a[0]);
    d[1] = nu_floor(a[1]);
}

void
nu_v3_copy (const float *a, float *d)
{
    d[0] = a[0];
    d[1] = a[1];
    d[2] = a[2];
}
void
nu_v3_zero (float *d)
{
    d[0] = 0.0f;
    d[1] = 0.0f;
    d[2] = 0.0f;
}
void
nu_v3_norm (const float *v, float *d)
{
    float length = nu_sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    d[0]         = v[0] / length;
    d[1]         = v[1] / length;
    d[2]         = v[2] / length;
}
void
nu_v3_cross (const float *a, const float *b, float *d)
{
    d[0] = a[1] * b[2] - a[2] * b[1];
    d[1] = a[2] * b[0] - a[0] * b[2];
    d[2] = a[0] * b[1] - a[1] * b[0];
}

void
nu_v4_zero (float *v)
{
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
    v[3] = 0.0f;
}

void
nu_iv2_zero (nu_i32_t *v)
{
    v[0] = 0;
    v[1] = 0;
}
void
nu_iv2_copy (const nu_i32_t *a, nu_i32_t *d)
{
    d[0] = a[0];
    d[1] = a[1];
}

void
nu_iv4_copy (const nu_i32_t *a, nu_i32_t *d)
{
    d[0] = a[0];
    d[1] = a[1];
    d[2] = a[2];
    d[3] = a[3];
}

void
nu_m4_identity (float *m)
{
    for (nu_size_t i = 0; i < NU_M4; ++i)
    {
        m[i] = 0.0f;
    }
    m[0]  = 1.0f;
    m[5]  = 1.0f;
    m[10] = 1.0f;
    m[15] = 1.0f;
}
void
nu_m4_zero (float *m)
{
    for (nu_size_t i = 0; i < NU_M4; ++i)
    {
        m[i] = 0.0f;
    }
}
void
nu_m4_rotate_x (float x, float *d)
{
}
void
nu_m4_mul (const float *a, const float *b, float *d)
{
    nu_m4_zero(d);
    for (nu_size_t i = 0; i < 4; ++i)
    {
        for (nu_size_t j = 0; j < 4; ++j)
        {
            for (nu_size_t k = 0; k < 4; ++k)
            {
                d[j * 4 + i] += a[k * 4 + i] * b[j * 4 + k];
            }
        }
    }
}

void
nu_perspective (
    float fov, float aspect_ratio, float z_near, float z_far, float *m)
{
    float y_scale  = 1.0f / nu_tan(fov / 2.0f);
    float x_scale  = y_scale / aspect_ratio;
    float near_far = z_near - z_far;

    m[0] = x_scale;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;

    m[4] = 0.0f;
    m[5] = y_scale;
    m[6] = 0.0f;
    m[7] = 0.0f;

    m[8]  = 0.0f;
    m[9]  = 0.0f;
    m[10] = (z_far + z_near) / near_far;
    m[11] = -1.0f;

    m[12] = 0.0f;
    m[13] = 0.0f;
    m[14] = (2.0f * z_far * z_near) / near_far;
    m[15] = 0.0f;
}
void
nu_lookat (const float *eye, const float *center, const float *up, float *m)
{
    float f[3] = { center[0] - eye[0], center[1] - eye[1], center[2] - eye[2] };
    nu_v3_norm(f, f);

    float u[3];
    nu_v3_norm(up, u);

    float s[3];
    nu_v3_cross(f, u, s);
    nu_v3_norm(s, s);

    float u_prime[3];
    nu_v3_cross(s, f, u_prime);

    m[0] = s[0];
    m[1] = u_prime[0];
    m[2] = -f[0];
    m[3] = 0.0f;

    m[4] = s[1];
    m[5] = u_prime[1];
    m[6] = -f[1];
    m[7] = 0.0f;

    m[8]  = s[2];
    m[9]  = u_prime[2];
    m[10] = -f[2];
    m[11] = 0.0f;

    m[12] = -s[0] * eye[0] - s[1] * eye[1] - s[2] * eye[2];
    m[13] = -u_prime[0] * eye[0] - u_prime[1] * eye[1] - u_prime[2] * eye[2];
    m[14] = f[0] * eye[0] + f[1] * eye[1] + f[2] * eye[2];
    m[15] = 1.0f;
}

#endif

#endif
