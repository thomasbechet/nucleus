#define NU_IMPLEMENTATION
#include <nucleus/nucleus.h>

void
init (void)
{
    nuext_import_package(NU_STR("../../../assets/pkg2.json"), NU_UID("import"));
    // nu_seria_t seria = nu_seria_create();
    // nu_seria_open_file(seria, NU_SERIA_WRITE, NU_STR("pkg.bin"));
    // nu_resource_save_group(NU_UID("import"), seria);
    // nu_seria_close(seria);
}

void
nu_app (void)
{
    nu_app_init_callback(init);
}
