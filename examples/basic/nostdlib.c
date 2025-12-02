__attribute__((force_align_arg_pointer)) void
_start (void)
{

    /* main body of program: call main(), etc */

    /* exit system call */
    asm("movl $1,%eax;"
        "xorl %ebx,%ebx;"
        "int  $0x80");
    __builtin_unreachable();
}
