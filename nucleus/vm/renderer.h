#ifndef NU_RENDERER_H
#define NU_RENDERER_H

#include <nucleus/vm/error.h>

typedef struct
{
    nu_error_t (*load)(void *userdata);
    nu_error_t (*unload)(void *userdata);
    nu_error_t (*draw)(void *userdata);
} nu_renderer_api_t;

typedef struct
{
    void             *userdata;
    nu_renderer_api_t api;
} nu_renderer_info_t;

typedef struct
{
    void             *userdata;
    nu_renderer_api_t api;
} nu__renderer_t;

#endif
