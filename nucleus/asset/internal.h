#ifndef NU_ASSET_INTERNAL_H
#define NU_ASSET_INTERNAL_H

#include <nucleus/asset/asset.h>
#include <nucleus/graphics/graphics.h>

typedef struct
{
    nu_bool_t       used;
    nu_asset_type_t type;
    nu_u32_t        hash;
    nu_asset_data_t data;
    nu_char_t       name[NU_ASSET_NAME_SIZE];
    nu_bundle_t     bundle;
    nu_size_t       refcount;
} nu__asset_entry_t;

typedef nu_pool(nu__asset_entry_t) nu__asset_entry_pool_t;

typedef struct
{
    nu__asset_entry_pool_t entries;
    nu_bundle_t            active_bundle;
} nu__asset_t;

static nu_error_t nu__asset_init(void);
static nu_error_t nu__asset_free(void);

#endif
