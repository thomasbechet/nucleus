#ifndef NU_IMPORTER_H
#define NU_IMPORTER_H

#include <nucleus/graphics/graphics.h>
#include <nucleus/asset/asset.h>
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

static jsmntok_t *nu__json_parse(const nu_char_t *json,
                                 nu_size_t        json_size,
                                 nu_size_t       *size,
                                 nu_size_t       *count);
static nu_bool_t  nu__json_eq(const nu_char_t *json,
                              jsmntok_t       *tok,
                              const nu_char_t *s);
static void       nu__json_value(const nu_char_t *json,
                                 jsmntok_t       *tok,
                                 nu_char_t       *s,
                                 nu_size_t        n);
static jsmntok_t *nu__json_skip(jsmntok_t *token);
static jsmntok_t *nu__json_object_member(const nu_char_t *json,
                                         jsmntok_t       *obj,
                                         const nu_char_t *name);

static nu_byte_t *nu__bytes_load_filename(const nu_char_t *filename,
                                          nu_size_t       *size);

static void nu__importer_init(void);
static void nu__importer_free(void);

#endif
