#ifndef NU_IMPORTER_H
#define NU_IMPORTER_H

#include <nucleus/importer/api.h>
#include <nucleus/graphics/graphics.h>
#include <nucleus/resource/resource.h>
#include <nucleus/importer/json.h>

#define NU__IMPORTER_PCG_SEED 12345
#define NU__IMPORTER_PCG_INCR 12345

#ifdef NU_BUILD_IMPORTER_CGLTF
typedef struct
{
    const void *ptr;
    void       *handle;
} nu__model_gltf_resource_t;

typedef struct
{
    NU_VEC(nu__model_gltf_resource_t) resources;
    nu_material_t default_material;
} nu__model_gltf_loader_t;
#endif

typedef struct
{
#ifdef NU_BUILD_IMPORTER_CGLTF
    nu__model_gltf_loader_t model_gltf_loader;
#endif
    nu_u64_t pcg_state;
} nu__importer_t;

static void nu__importer_init(void);
static void nu__importer_free(void);

#endif
