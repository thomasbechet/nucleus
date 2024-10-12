#ifndef NU_APP_H
#define NU_APP_H

#include <nucleus/core/api.h>

typedef struct
{
    nu_app_callback_t init;
    nu_app_callback_t free;
    nu_app_callback_t update;
    float             deltatime;
} nu__app_t;

#endif
