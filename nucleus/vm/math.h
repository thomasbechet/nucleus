#ifndef NU_MATH_H
#define NU_MATH_H

#include <nucleus/vm/types.h>
#include <nucleus/vm/platform.h>
#include <nucleus/vm/error.h>

#define NU_FIX_FRAC  16
#define NU_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define NU_MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef nu_int_t nu_fix_t;
typedef nu_fix_t nu_vec2_t[2];
typedef nu_int_t nu_ivec2_t[2];
typedef nu_fix_t nu_vec3_t[3];
typedef nu_int_t nu_ivec3_t[3];
typedef nu_fix_t nu_vec4_t[4];
typedef nu_int_t nu_ivec4_t[4];
typedef nu_fix_t nu_mat3_t[9];
typedef nu_fix_t nu_mat4_t[16];
typedef nu_fix_t nu_quat_t[4];

NU_API nu_bool_t nu_is_power_of_two(nu_size_t n);
NU_API nu_size_t nu_log2(nu_size_t n);
NU_API nu_fix_t  nu_pow(nu_fix_t a, nu_fix_t b);
NU_API nu_fix_t  nu_floor(nu_fix_t a);
NU_API nu_fix_t  nu_ceil(nu_fix_t a);
NU_API nu_fix_t  nu_exp(nu_fix_t a);
NU_API nu_fix_t  nu_log(nu_fix_t a);
NU_API nu_fix_t  nu_log10(nu_fix_t a);
NU_API nu_fix_t  nu_sqrt(nu_fix_t a);
NU_API nu_fix_t  nu_fmod(nu_fix_t a, nu_fix_t b);
NU_API nu_fix_t  nu_sin(nu_fix_t a);
NU_API nu_fix_t  nu_cos(nu_fix_t a);
NU_API nu_fix_t  nu_tan(nu_fix_t a);
NU_API nu_fix_t  nu_acos(nu_fix_t a);
NU_API nu_fix_t  nu_asin(nu_fix_t a);
NU_API nu_fix_t  nu_atan2(nu_fix_t a, nu_fix_t b);

NU_API nu_fix_t   nu_fadd(nu_fix_t a, nu_fix_t b);
NU_API nu_fix_t   nu_fsub(nu_fix_t a, nu_fix_t b);
NU_API nu_fix_t   nu_fmul(nu_fix_t a, nu_fix_t b);
NU_API nu_fix_t   nu_fdiv(nu_fix_t a, nu_fix_t b);
NU_API nu_fix_t   nu_itof(nu_int_t a);
NU_API nu_int_t   nu_ftoi(nu_fix_t a);
NU_API float      nu_ftofloat(nu_fix_t a);
NU_API double     nu_ftodouble(nu_fix_t a);
NU_API nu_error_t nu_fparse(const nu_char_t *s, nu_size_t n, nu_fix_t *v);
NU_API nu_error_t nu_iparse(const nu_char_t *s, nu_size_t n, nu_i32_t *v);

#ifdef NU_IMPL

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
nu_fix_t
nu_pow (nu_fix_t a, nu_fix_t b)
{
    (void)a;
    (void)b;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_floor (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_ceil (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

nu_fix_t
nu_exp (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_log (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_log10 (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_sqrt (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_fmod (nu_fix_t a, nu_fix_t b)
{
    (void)a;
    (void)b;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_sin (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}

nu_fix_t
nu_cos (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_tan (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_acos (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_asin (nu_fix_t a)
{
    (void)a;
    /* TODO */
    return 0.0;
}
nu_fix_t
nu_atan2 (nu_fix_t a, nu_fix_t b)
{
    (void)a;
    (void)b;
    /* TODO */
    return 0.0;
}

nu_fix_t
nu_fadd (nu_fix_t a, nu_fix_t b)
{
    return a + b;
}
nu_fix_t
nu_fsub (nu_fix_t a, nu_fix_t b)
{
    return a - b;
}
nu_fix_t
nu_fmul (nu_fix_t a, nu_fix_t b)
{
    return (nu_fix_t)(((nu_u64_t)a * (nu_u64_t)b) >> NU_FIX_FRAC);
}
nu_fix_t
nu_fdiv (nu_fix_t a, nu_fix_t b)
{
    return (nu_fix_t)(((nu_i64_t)a << NU_FIX_FRAC) / (nu_i64_t)b);
}
nu_fix_t
nu_itof (nu_int_t a)
{
    return (nu_fix_t)(a << NU_FIX_FRAC);
}
nu_int_t
nu_ftoi (nu_fix_t a)
{
    return (nu_int_t)(a >> NU_FIX_FRAC);
}
float
nu_ftofloat (nu_fix_t a)
{
    return (float)a / (float)(1 << NU_FIX_FRAC);
}
double
nu_ftodouble (nu_fix_t a)
{
    return (double)a / (double)(1 << NU_FIX_FRAC);
}
static nu_u32_t
nu__fast_pow10 (nu_u32_t n)
{
    static nu_u32_t pow10[8]
        = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000 };
    NU_ASSERT(n < 8);
    return pow10[n];
}
nu_error_t
nu_fparse (const nu_char_t *s, nu_size_t n, nu_fix_t *v)
{
    nu_u32_t  i, fixed, base;
    nu_bool_t sign = NU_FALSE, frac_flag = NU_FALSE;
    nu_u32_t  inte = 0, frac = 0, dp = 0;
    /* parse integer part, fractional part and decimal point */
    for (i = 0; i < n; ++i)
    {
        nu_char_t c = s[i];
        switch (c)
        {
            case '-':
                if (i != 0)
                {
                    return NU_ERROR_INVALID_STATE;
                }
                sign = NU_TRUE;
                break;
            case '.':
                frac_flag = NU_TRUE;
                break;
            default:
                if (c > '9' || c < '0')
                {
                    return NU_ERROR_INVALID_STATE;
                }
                if (frac_flag)
                {
                    if (dp >= 6)
                    {
                        return NU_ERROR_INVALID_STATE;
                    }
                    frac = frac * 10 + ((nu_u32_t)c - (nu_u32_t)'0');
                    dp++;
                }
                else
                {
                    inte = inte * 10 + ((nu_u32_t)c - (nu_u32_t)'0');
                }
                break;
        }
    }
    /* build fixed point float */
    fixed = inte << NU_FIX_FRAC;
    base  = nu__fast_pow10(dp);
    i     = 0;
    while (i < NU_FIX_FRAC)
    {
        frac <<= 1;
        if (frac >= base)
        {
            fixed |= 1 << (NU_FIX_FRAC - 1 - i);
            frac -= base;
        }
        i++;
    }
    if (sign)
    {
        fixed = !fixed + 1;
    }
    *v = (nu_fix_t)fixed;
    return NU_ERROR_NONE;
}
nu_error_t
nu_iparse (const nu_char_t *s, nu_size_t n, nu_i32_t *v)
{
    nu_u32_t  i;
    nu_bool_t sign  = NU_FALSE;
    nu_i32_t  value = 0;
    /* parse integer part */
    for (i = 0; i < n; ++i)
    {
        nu_char_t c = s[i];
        switch (c)
        {
            case '-':
                if (i != 0)
                {
                    return NU_ERROR_INVALID_STATE;
                }
                sign = NU_TRUE;
                break;
            default:
                if (c > '9' || c < '0')
                {
                    return NU_ERROR_INVALID_STATE;
                }
                value = value * 10 + ((nu_u32_t)c - (nu_u32_t)'0');
                break;
        }
    }
    if (sign)
    {
        value = -value;
    }
    *v = value;
    return NU_ERROR_NONE;
}

#endif

#endif
