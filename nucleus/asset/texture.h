#ifndef NU_ASSET_TEXTURE_H
#define NU_ASSET_TEXTURE_H

#include <nucleus/asset/manager.h>
#include <nucleus/graphics.h>

typedef struct
{
    nu_texture_t      handle;
    nu_texture_info_t info;
    nu_bool_t         has_image;
} nu_texture_asset_t;

#endif
