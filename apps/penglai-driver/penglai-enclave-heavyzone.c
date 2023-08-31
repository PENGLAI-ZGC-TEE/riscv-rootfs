#include "penglai-enclave-heavyzone.h"
#include "penglai-enclave-ioctl.h"
#include "dbg.h"

int penglai_heavy_enclave_create(struct file * filep, unsigned long args)
{
    check(args != NULL, "Invailid user args.");
    struct penglai_enclave_user_param* usr_param = (struct penglai_enclave_user_param *)args;

    void *elf_ptr = (void *)usr_parm->elf_ptr;
    int elf_size = 0;

    if(penglai_enclave_elfmemsize(elf_ptr, &elf_size) < 0)
    {
        log_err("Calculate elf_size failed.\n");
        goto error;
    }


    return 0;
error:
    return -1;
}
