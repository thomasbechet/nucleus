#ifndef NU_ASSET_TEXTURE_H
#define NU_ASSET_TEXTURE_H

#include <nucleus/asset/manager.h>

typedef struct
{
    nu_texture_handle_t handle;
    nu_texture_info_t   info;
    nu_image_t          image;
    nu_bool_t           has_image;
} nu_texture_t;

NU_DEFINE_ASSET(nu_asset_texture_t, nu_texture_t, void);

#endif
