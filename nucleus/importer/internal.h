#ifndef NU_IMPORTER_INTERNAL_H
#define NU_IMPORTER_INTERNAL_H

#include <nucleus/core/core.h>
#include <nucleus/external/jsmn/jsmn.h>

#ifdef NU_BUILD_CGLTF
typedef struct
{
    const void *ptr;
    nu_u16_t    index;
} nu__model_gltf_cache_t;
typedef nu_vec(nu__model_gltf_cache_t) nu__model_gltf_cache_vec_t;

typedef struct
{
    nu__model_gltf_cache_vec_t _cache;
    nu_bool_t                  _has_default_material;
    nu_u32_t                   _default_material;
} nu__model_gltf_loader_t;
#endif

typedef struct
{
#ifdef NU_BUILD_CGLTF
    nu__model_gltf_loader_t model_gltf_loader;
#endif
    int dummy;
} nu__importer_t;

static int        nu__jsoneq(const nu_char_t *json,
                             jsmntok_t       *tok,
                             const nu_char_t *s);
static jsmntok_t *nu__json_load_filename(const nu_char_t *filename,
                                         nu_size_t       *s);
static nu_byte_t *nu__bytes_load_filename(const nu_char_t *filename,
                                          nu_size_t       *size);

static void nu__importer_init(void);
static void nu__importer_free(void);

#endif
