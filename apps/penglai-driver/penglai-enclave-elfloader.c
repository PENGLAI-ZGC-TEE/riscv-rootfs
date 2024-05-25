#include "penglai-enclave-elfloader.h"
#include "penglai-enclave-ioctl.h"

int penglai_enclave_load_NOBITS_section(enclave_mem_t* enclave_mem, void * elf_sect_addr, int elf_sect_size)
{
	vaddr_t addr;
	vaddr_t enclave_new_page;
	int size;
	for(addr = (vaddr_t)elf_sect_addr; addr < (vaddr_t)elf_sect_addr + elf_sect_size; addr += RISCV_PGSIZE)
	{
		enclave_new_page = enclave_alloc_page(enclave_mem, addr, ENCLAVE_USER_PAGE);
		if (addr + RISCV_PGSIZE >(vaddr_t) elf_sect_addr + elf_sect_size)
			size = elf_sect_size % RISCV_PGSIZE;
		else
			size = RISCV_PGSIZE;
		memset((void *) enclave_new_page, 0, size);
	}
	return 0;
}

/* elf_prog_infile_addr @ content in elf file
   elf_prog_addr @ virtual addr for program begin addr
   elf_prog_size @ size of prog segment
   */
int penglai_enclave_load_program(enclave_mem_t* enclave_mem, vaddr_t elf_prog_infile_addr, void * elf_prog_addr, int elf_prog_size)
{
	vaddr_t addr;
	vaddr_t enclave_new_page;
	int size;
	int r;
	for(addr =  (vaddr_t)elf_prog_addr; addr <  (vaddr_t)elf_prog_addr + elf_prog_size; addr += RISCV_PGSIZE)
	{

		enclave_new_page = enclave_alloc_page(enclave_mem, addr, ENCLAVE_USER_PAGE);
		if (addr + RISCV_PGSIZE > (vaddr_t)elf_prog_addr + elf_prog_size)
			size = elf_prog_size % RISCV_PGSIZE;
		else
			size = RISCV_PGSIZE;
		r = copy_from_user((void* )enclave_new_page, (void *)(elf_prog_infile_addr + addr - (vaddr_t)elf_prog_addr), size);
	}
	return 0;
}

/* ptr @ user pointer
   hdr @ kernel pointer
   */
int penglai_enclave_loadelf(enclave_mem_t*enclave_mem, void* __user elf_ptr, unsigned long size, vaddr_t * elf_entry_point)
{
	struct  elfhdr elf_hdr;
	struct elf_phdr elf_prog_hdr;
	struct elf_shdr elf_sect_hdr;
	int i,  elf_prog_size;
	vaddr_t elf_sect_ptr, elf_prog_ptr, elf_prog_addr, elf_prog_infile_addr;
	if(copy_from_user(&elf_hdr, elf_ptr, sizeof(struct elfhdr)) != 0)
	{
		printk("KERNEL MODULE:  elf_hdr copy_from_user failed\n");
		return -1;
	}
	*elf_entry_point = elf_hdr.e_entry;
	elf_sect_ptr = (vaddr_t) elf_ptr + elf_hdr.e_shoff;

	/* Loader section */
	for (i = 0; i < elf_hdr.e_shnum;i++)
	{
		if (copy_from_user(&elf_sect_hdr,(void *)elf_sect_ptr,sizeof(struct elf_shdr)))
		{
			printk("KERNEL MODULE: elf_sect_hdr copy_from_user failed\n");
			elf_sect_ptr += sizeof(struct elf_shdr);
			continue;
		}
		if (elf_sect_hdr.sh_addr == 0)
		{
			elf_sect_ptr += sizeof(struct elf_shdr);
			continue;
		}

		/* Load NOBITS section */
		if (elf_sect_hdr.sh_type == SHT_NOBITS)
		{
			vaddr_t elf_sect_addr = elf_sect_hdr.sh_addr;
			int elf_sect_size = elf_sect_hdr.sh_size;
			if (penglai_enclave_load_NOBITS_section(enclave_mem,(void *)elf_sect_addr,elf_sect_size) < 0)
			{
				printk("KERNEL MODULE: penglai enclave load NOBITS  section failed\n");
				return -1;
			}
		}
		elf_sect_ptr += sizeof(struct elf_shdr);
	}

	/* Load program segment */
	elf_prog_ptr = (vaddr_t) elf_ptr + elf_hdr.e_phoff;

	for(i = 0; i < elf_hdr.e_phnum;i++)
	{
		if (copy_from_user(&elf_prog_hdr,(void *)elf_prog_ptr,sizeof(struct elf_phdr)))
		{
			printk("KERNEL MODULE: elf_prog_hdr copy_from_user failed\n");
			elf_prog_ptr += sizeof(struct elf_phdr);
			continue;
		}

		/* Virtual addr for program begin address */
		elf_prog_addr = elf_prog_hdr.p_vaddr;
		elf_prog_size = elf_prog_hdr.p_filesz;
		elf_prog_infile_addr = (vaddr_t) elf_ptr + elf_prog_hdr.p_offset;
		if (penglai_enclave_load_program(enclave_mem, elf_prog_infile_addr, (void *)elf_prog_addr, elf_prog_size) < 0)
		{
			printk("KERNEL MODULE: penglai enclave load program failed\n");
			return -1;
		}
		printk("[Penglai Driver@%s] elf_prog_addr:0x%lx elf_prog_size:0x%x, infile_addr:0x%lx", __func__,
				elf_prog_addr, elf_prog_size, elf_prog_infile_addr);
		elf_prog_ptr += sizeof(struct elf_phdr);
	}
	return 0;
}

int penglai_enclave_elfmemsize(void* __user elf_ptr, int* size)
{
	struct elfhdr elf_hdr;
	struct elf_phdr elf_prog_hdr;
	struct elf_shdr elf_sect_hdr;
	int i, elf_prog_size;
	vaddr_t elf_sect_ptr, elf_prog_ptr;
	if(copy_from_user(&elf_hdr, elf_ptr, sizeof(struct elfhdr)) != 0)
	{
		printk("[Penglai Driver@%s] elf_hdr copy_from_user failed\n", __func__);
		return -1;
	}
	elf_sect_ptr = (vaddr_t) elf_ptr + elf_hdr.e_shoff;

	for (i = 0; i < elf_hdr.e_shnum;i++)
	{
		if (copy_from_user(&elf_sect_hdr,(void *)elf_sect_ptr,sizeof(struct elf_shdr)))
		{
			printk("[Penglai Driver@%s] elf_sect_hdr copy_from_user failed\n", __func__);
			elf_sect_ptr += sizeof(struct elf_shdr);
			return -1;
		}
		if (elf_sect_hdr.sh_addr == 0)
		{
			elf_sect_ptr += sizeof(struct elf_shdr);
			continue;
		}

		// Calculate the size of the NOBITS section
		if (elf_sect_hdr.sh_type == SHT_NOBITS)
		{
			int elf_sect_size = elf_sect_hdr.sh_size;
			*size = *size + elf_sect_size;
		}
		elf_sect_ptr += sizeof(struct elf_shdr);
	}

	// Calculate the size of the PROGBITS segment
	elf_prog_ptr = (vaddr_t) elf_ptr + elf_hdr.e_phoff;

	for(i = 0; i < elf_hdr.e_phnum;i++)
	{
		if (copy_from_user(&elf_prog_hdr,(void *)elf_prog_ptr,sizeof(struct elf_phdr)))
		{
			printk("[Penglai Driver@%s] elf_prog_hdr copy_from_user failed\n", __func__);
			elf_prog_ptr += sizeof(struct elf_phdr);
			return -1;
		}

		// Virtual addr for program begin address
		elf_prog_size = elf_prog_hdr.p_filesz;
		*size = *size + elf_prog_size;
		elf_prog_ptr += sizeof(struct elf_phdr);
	}
	return 0;
}

int penglai_enclave_eapp_preprare(enclave_mem_t* enclave_mem,  void* __user elf_ptr, unsigned long size, vaddr_t * elf_entry_point, vaddr_t stack_ptr, int stack_size)
{
	vaddr_t addr;

	/* Init stack */
	for(addr = stack_ptr - stack_size; addr < stack_ptr; addr += RISCV_PGSIZE)
	{
		enclave_alloc_page(enclave_mem, addr, ENCLAVE_STACK_PAGE);
	}

	/* Load elf file */
	if(penglai_enclave_loadelf(enclave_mem, elf_ptr, size, elf_entry_point) < 0)
	{
		printk("KERNEL MODULE: penglai enclave loadelf failed\n");
	}

	return 0;
}

int fdi_enclave_eapp_preprare(void* __user elf_ptr, struct penglai_enclave_sbi_param * enclave_sbi_param)
{
#define align8up(addr) 		 ((addr+0x7) & ~(0x7)) 
#define align8down(addr) 	 (addr & ~(0x7))
	struct elfhdr elf_hdr;
	struct elf_shdr elf_sect_hdr;
	int i;
	vaddr_t elf_sect_ptr, elf_secstrs_shdr, elf_secstrs_ptr, sector_name_ptr;
	char * ksec_name_buffer = NULL;
	if(copy_from_user(&elf_hdr, elf_ptr, sizeof(struct elfhdr)) != 0)
	{
		printk("[Penglai Driver@%s] elf_hdr copy_from_user failed\n", __func__);
		return -1;
	}
	elf_sect_ptr = (vaddr_t) elf_ptr + elf_hdr.e_shoff;
	// void * elf_sec_str = (vaddr_t) elf_ptr + elf_hdr.e_shoff;

	// Read sector table's str addr
	elf_secstrs_shdr = elf_sect_ptr + elf_hdr.e_shstrndx * sizeof(struct elf_shdr);
	if (copy_from_user(&elf_sect_hdr, (void *)(elf_secstrs_shdr), sizeof(struct elf_shdr)))
	{
		printk("[Penglai Driver@%s] elf_sect_hdr copy_from_user failed\n", __func__);	
		return -1;	
	}
	elf_secstrs_ptr = (vaddr_t)elf_ptr + elf_sect_hdr.sh_offset;

	// Alloc a name buffer to store sector name
	ksec_name_buffer = (char *)kmalloc(MAX_SETTOR_NAME_LENGTH, GFP_KERNEL);

	if (!ksec_name_buffer)
	{
		printk("[Penglai Driver@%s] ksec_name_buffer alloc failed\n", __func__);	
		return -1;			
	}

	// Clear fdi enable
	enclave_sbi_param->fdi_enable = 0;
	for (i = 0; i < elf_hdr.e_shnum;i++)
	{
		if (copy_from_user(&elf_sect_hdr,(void *)elf_sect_ptr,sizeof(struct elf_shdr)))
		{
			printk("[Penglai Driver@%s] elf_sect_hdr copy_from_user failed\n", __func__);
			elf_sect_ptr += sizeof(struct elf_shdr);
			return -1;
		}
		
		sector_name_ptr = elf_secstrs_ptr + elf_sect_hdr.sh_name;

		if (strncpy_from_user(ksec_name_buffer, (void *)sector_name_ptr, MAX_SETTOR_NAME_LENGTH) < 0) {
			printk("[Penglai Driver@%s] sector_name_ptr strncpy_from_user: 0x%lx failed\n", __func__, sector_name_ptr);
			return -1;
		}


		if (!strcmp(ksec_name_buffer, ".text"))
		{
			enclave_sbi_param->maintext_start = elf_sect_hdr.sh_addr;
			enclave_sbi_param->maintext_end = align8up(elf_sect_hdr.sh_addr + elf_sect_hdr.sh_size);
			printk("[Penglai Driver@%s] Find text section, start: 0x%lx, end: 0x%lx\n", __func__, \
								enclave_sbi_param->maintext_start, \
									enclave_sbi_param->maintext_end);

		}

		if (!strcmp(ksec_name_buffer, ".ulibtext"))
		{
			enclave_sbi_param->fdi_enable = 1;
			printk("[Penglai Driver@%s] Find ulibtext section, enable FDI !\n", __func__);
		}

		elf_sect_ptr += sizeof(struct elf_shdr);
	}

	kfree(ksec_name_buffer);

	return 0;
}

int map_untrusted_mem(enclave_mem_t* enclave_mem, vaddr_t vaddr, paddr_t paddr, unsigned long size)
{
	vaddr_t addr = vaddr;

	for (; addr < vaddr + size; addr+=RISCV_PGSIZE) {
		map_va2pa(enclave_mem, addr, paddr, ENCLAVE_UNTRUSTED_PAGE);
		paddr += RISCV_PGSIZE;
	}
	return 0;
}

int map_kbuffer(enclave_mem_t* enclave_mem, vaddr_t vaddr, paddr_t paddr, unsigned long size)
{
	vaddr_t addr = vaddr;

	for (; addr < vaddr + size; addr += RISCV_PGSIZE) {
		map_va2pa(enclave_mem, addr, paddr, ENCLAVE_KBUFFER_PAGE);
		paddr += RISCV_PGSIZE;
	}
	return 0;
}
