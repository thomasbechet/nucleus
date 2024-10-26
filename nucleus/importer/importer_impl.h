#ifndef NU_IMPORTER_IMPL_H
#define NU_IMPORTER_IMPL_H

#include <nucleus/internal.h>

#include <nucleus/importer/package_impl.h>
#include <nucleus/importer/model_impl.h>
#include <nucleus/importer/image_impl.h>
#include <nucleus/importer/cubemap_impl.h>

static nu_byte_t *
nu__bytes_load_filename (nu_str_t filename, nu_size_t *size)
{
    char fn[256];
    nu_str_to_cstr(filename, fn, 256);
    FILE *f = fopen(fn, "rb");
    if (!f)
    {
        NU_ERROR("failed to open file %s", filename);
        return NU_NULL;
    }
    fseek(f, 0, SEEK_END);
    nu_size_t fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    nu_byte_t *bytes = (nu_byte_t *)nu_alloc(fsize);
    fread(bytes, fsize, 1, f);
    *size = fsize;
    return bytes;
}

static void
nu__importer_init (void)
{
#ifdef NU_BUILD_CGLTF
    nu__model_gltf_loader_init();
#endif
}
static void
nu__importer_free (void)
{
#ifdef NU_BUILD_CGLTF
    nu__model_gltf_loader_free();
#endif
}

#endif
