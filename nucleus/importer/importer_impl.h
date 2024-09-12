#ifndef NU_IMPORTER_IMPL_H
#define NU_IMPORTER_IMPL_H

#include <nucleus/internal.h>

#include <nucleus/importer/json_impl.h>
#include <nucleus/importer/package_impl.h>
#include <nucleus/importer/model_impl.h>
#include <nucleus/importer/texture_impl.h>

static void
nu__importer_init (void)
{
#ifdef NU_BUILD_IMPORTER_CGLTF
    nu__model_gltf_loader_init();
#endif
    _ctx.importer.pcg_state = NU__IMPORTER_PCG_SEED;
}
static void
nu__importer_free (void)
{
#ifdef NU_BUILD_IMPORTER_CGLTF
    nu__model_gltf_loader_free();
#endif
}

#endif
