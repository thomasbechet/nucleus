#ifndef NU_LOADER_H
#define NU_LOADER_H

#include <nucleus/graphics.h>
#include <nucleus/utils/image.h>
#include <nucleus/utils/model.h>

typedef struct
{
    const void *ptr;
    nu_u16_t    index;
} nu__gltf_model_cache_t;
typedef nu_vec(nu__gltf_model_cache_t) nu__gltf_model_cache_vec_t;

typedef struct
{
    nu_allocator_t            *_allocator;
    nu_logger_t               *_logger;
    nu__gltf_model_cache_vec_t _cache;
    nu_bool_t                  _has_default_material;
    nu_u32_t                   _default_material;
} nu_gltf_loader_t;

NU_API nu_error_t nu_gltf_loader_init(nu_allocator_t   *alloc,
                                      nu_logger_t      *logger,
                                      nu_gltf_loader_t *loader);
NU_API void       nu_gltf_loader_free(nu_gltf_loader_t *loader);

NU_API nu_error_t nuext_gltf_load_model_filename(nu_gltf_loader_t *loader,
                                                 const nu_char_t  *filename,
                                                 nu_allocator_t   *alloc,
                                                 nu_renderer_t    *renderer,
                                                 nu_model_t       *model);

NU_API nu_error_t nuext_load_image_filename(const nu_char_t *filename,
                                            nu_allocator_t  *allocator,
                                            nu_image_t      *image);
NU_API nu_error_t nu_load_image_memory(const nu_byte_t *data,
                                       nu_size_t        data_size,
                                       nu_allocator_t  *allocator,
                                       nu_image_t      *image);

#endif
