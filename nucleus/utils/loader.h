#ifndef NU_LOADER_H
#define NU_LOADER_H

#include <nucleus/graphics/image.h>
#include <nucleus/graphics/model.h>

typedef struct
{
    const void *ptr;
    nu_u16_t    index;
} nu__gltf_model_cache_t;
typedef nu_vec(nu__gltf_model_cache_t) nu__gltf_model_cache_vec_t;

typedef struct
{
    nu__gltf_model_cache_vec_t _cache;
    nu_bool_t                  _has_default_material;
    nu_u32_t                   _default_material;
} nu_gltf_loader_t;

NU_API nu_error_t nu_gltf_loader_init(nu_gltf_loader_t *loader);
NU_API void       nu_gltf_loader_free(nu_gltf_loader_t *loader);

NU_API nu_model_t nuext_model_load_gltf_filename(nu_gltf_loader_t *loader,
                                                 const nu_char_t  *filename);

NU_API nu_image_t nuext_image_load_filename(const nu_char_t *filename);
NU_API nu_image_t nu_image_load_memory(const nu_byte_t *data,
                                       nu_size_t        data_size);

#endif
