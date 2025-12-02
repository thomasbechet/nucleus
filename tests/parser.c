#define NU_IMPL
#include <nucleus/lang.h>

static nu_char_t *
load_file (const nu_char_t *file)
{
    nu_char_t *buffer = NULL;
    FILE      *f      = fopen(file, "rb");
    long       length;
    if (f)
    {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = malloc(length + 1);
        if (buffer)
        {
            fread(buffer, 1, length, f);
        }
        buffer[length] = '\0';
        fclose(f);
    }
    return buffer;
}

int
main (int argc, char *argv[])
{
    nu_vm_info_t           vm_info;
    nu_vm_t                vm;
    nulang_status_t        status;
    nulang_compiler_info_t info;
    nulang_compiler_t      compiler;
    nu_char_t             *source;

    NU_ASSERT(argc == 2);
    source = load_file(argv[1]);
    NU_ASSERT(source);

    nu_allocator_api_stdlib(&vm_info.allocator);
    nu_cartridge_api_default(&vm_info.cartridge);
    nu_vm_init(&vm_info, &vm);
    {
        nu_error_t error;
        error = nu_vm_create_archetype(vm, "player", NU_NULL, 0, NU_NULL);
        NU_ASSERT(error == NU_ERROR_NONE);
    }

    /* nulang_print_tokens(source); */
    nulang_compiler_info_default(vm, &info);
    status = nulang_compiler_init(&info, &compiler);
    NU_ASSERT(status == NULANG_SUCCESS);
    status = nulang_compile(&compiler, source);
    nulang_print_status(&compiler);
    NU_ASSERT(status == NULANG_SUCCESS);
    nulang_print_ast(&compiler);

    nulang_compiler_free(&compiler);
    return 0;
}
