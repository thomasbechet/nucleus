#ifndef NU_ASSET_MANAGER_IMPL_H
#define NU_ASSET_MANAGER_IMPL_H

#include <nucleus/asset/manager.h>

nu_error_t
nu_asset_manager_init (nu_allocator_t *alloc, nu_asset_manager_t *manager)
{
    manager->_allocator = alloc;
    return NU_ERROR_NONE;
}
void
nu_asset_manager_free (nu_asset_manager_t *manager)
{
}

#endif
