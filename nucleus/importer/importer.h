#ifndef NU_IMPORTER_H
#define NU_IMPORTER_H

#include <nucleus/importer/api.h>
#include <nucleus/graphics/graphics.h>
#include <nucleus/asset/asset.h>
#include <nucleus/external/jsmn/jsmn.h>

#ifdef NU_BUILD_IMPORTER_CGLTF
typedef struct
{
    const void *ptr;
    nu_u16_t    index;
} nu__model_gltf_cache_t;
typedef NU_VEC(nu__model_gltf_cache_t) nu__model_gltf_cache_vec_t;

typedef struct
{
    nu__model_gltf_cache_vec_t cache;
    nu_bool_t                  has_default_material;
    nu_u32_t                   default_material;
} nu__model_gltf_loader_t;
#endif

typedef struct
{
#ifdef NU_BUILD_IMPORTER_CGLTF
    nu__model_gltf_loader_t model_gltf_loader;
#endif
    int dummy;
} nu__importer_t;

static void nu__importer_init(void);
static void nu__importer_free(void);

#endif
