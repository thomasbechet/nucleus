#ifndef NU_MATH_H
#define NU_MATH_H

#include <nucleus/types.h>

NU_API nu_bool_t nu_is_power_of_two(nu_size_t n);
NU_API nu_size_t nu_log2(nu_size_t n);
NU_API float     nu_floor(float f);

NU_API void nu_vec2_copy(const float *s, float *d);
NU_API void nu_vec2_zero(float *v);
NU_API void nu_vec2_add(const float *a, const float *b, float *d);
NU_API void nu_vec2_sub(const float *a, const float *b, float *d);
NU_API void nu_vec2_mul(const float *a, const float *b, float *d);
NU_API void nu_vec2_muls(const float *a, float s, float *d);
NU_API void nu_vec2_div(const float *a, const float *b, float *d);
NU_API void nu_vec2_divs(const float *a, float s, float *d);
NU_API void nu_vec2_floor(const float *a, float *d);

NU_API void nu_vec4_zero(float *v);

NU_API void nu_ivec2_zero(nu_i32_t *v);
NU_API void nu_ivec2_copy(const nu_i32_t *a, nu_i32_t *d);

NU_API void nu_ivec4_copy(const nu_i32_t *a, nu_i32_t *d);

#ifdef NU_IMPLEMENTATION

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
#ifdef NU_STDLIB
#include <math.h>
float
nu_floor (float f)
{
    return floorf(f);
}
#endif

void
nu_vec2_copy (const float *s, float *d)
{
    d[0] = s[0];
    d[1] = s[1];
}
void
nu_vec2_zero (float *v)
{
    v[0] = 0.0f;
    v[1] = 0.0f;
}
void
nu_vec2_add (const float *a, const float *b, float *d)
{
    d[0] = a[0] + b[0];
    d[1] = a[1] + b[1];
}
void
nu_vec2_sub (const float *a, const float *b, float *d)
{
    d[0] = a[0] - b[0];
    d[1] = a[1] - b[1];
}
void
nu_vec2_mul (const float *a, const float *b, float *d)
{
    d[0] = a[0] * b[0];
    d[1] = a[1] * b[1];
}
void
nu_vec2_muls (const float *a, float s, float *d)
{
    d[0] = a[0] * s;
    d[1] = a[1] * s;
}
void
nu_vec2_div (const float *a, const float *b, float *d)
{
    d[0] = a[0] / b[0];
    d[1] = a[1] / b[1];
}
void
nu_vec2_divs (const float *a, float s, float *d)
{
    d[0] = a[0] / s;
    d[1] = a[1] / s;
}
void
nu_vec2_floor (const float *a, float *d)
{
    d[0] = nu_floor(a[0]);
    d[1] = nu_floor(a[1]);
}

void
nu_vec4_zero (float *v)
{
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
    v[3] = 0.0f;
}

void nu_ivec2_zero(nu_i32_t *v)
{
    v[0] = 0;
    v[1] = 0;
}
void
nu_ivec2_copy (const nu_i32_t *a, nu_i32_t *d)
{
    d[0] = a[0];
    d[1] = a[1];
}

void
nu_ivec4_copy (const nu_i32_t *a, nu_i32_t *d)
{
    d[0] = a[0];
    d[1] = a[1];
    d[2] = a[2];
    d[3] = a[3];
}

#endif

#endif
