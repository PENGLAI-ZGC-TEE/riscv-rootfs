#include <linux/mm.h>
#include <linux/file.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/miscdevice.h>
#include <linux/dma-mapping.h>
#include "penglai-enclave-driver.h"
#include "penglai-enclave-ioctl.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("enclave_ioctl");
MODULE_AUTHOR("LuXu");
MODULE_VERSION("enclave_ioctl");

static cma_mem_t cma_mem;

static int enclave_mmap(struct file* f,struct vm_area_struct *vma)
{
	return 0;
}

static const struct file_operations enclave_ops = {
	.owner = THIS_MODULE,
	.mmap = enclave_mmap,
	.unlocked_ioctl = penglai_enclave_ioctl
};

struct miscdevice enclave_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "penglai_enclave_dev",
	.fops = &enclave_ops,
	.mode = 0666,
};

//static int enclave_ioctl_init(void)
int enclave_ioctl_init(void)
{
	int ret;
    struct device *cma_dev;
	void *cma_addr;
	struct sbiret sbiret;
	printk("enclave_ioctl_init...\n");

	//register enclave_dev
	ret=misc_register(&enclave_dev);
	if(ret < 0)
	{
		printk("Enclave_driver: register enclave_dev failed!(ret:%d)\n",
				ret);
		goto deregister_device;
	}

	/* [Dd] Should we broadcast some states (e.g., PT_area region) to other harts? */
	/* [LX] sm will broadcast */
	// addr = __get_free_pages(GFP_HIGHUSER, DEFAULT_SECURE_PAGES_ORDER);

	/* [YBC] use contiguous memory allocator */
    cma_dev = enclave_dev.this_device;
    cma_dev->coherent_dma_mask = DMA_BIT_MASK(64); // dma device can access 64-bit address space
    cma_dev->dma_mask = &cma_dev->coherent_dma_mask;

    cma_addr = dma_alloc_coherent(cma_dev, CMA_SIZE, &cma_mem.cma_handle, GFP_KERNEL);
	if(!cma_addr)
	{
		// printk("[Penglai KModule]: can not get free page which order is 0x%x", DEFAULT_SECURE_PAGES_ORDER);
		printk("[Penglai KModule] CMA allocation failed with size %ld\n", CMA_SIZE);
		ret = -1;
		goto deregister_device;
	}
    cma_mem.cma_start_va = cma_addr;
    cma_mem.cma_start_pa = __pa(cma_addr);

#if 1
	// sbiret = SBI_CALL_2(SBI_SM_INIT, __pa(cma_addr), 1 << (DEFAULT_SECURE_PAGES_ORDER + RISCV_PGSHIFT));
	sbiret = SBI_CALL_2(SBI_SM_INIT, __pa(cma_addr), CMA_SIZE);
	ret = sbiret.value;
	//if(ret < 0)
	if(sbiret.error)
	{
		printk("[Penglai KModule]: sbi call mm_init is failed\n");
		goto deregister_device;
	}
#endif
	printk("[Penglai KModule] register_chrdev succeeded!\n");
	return 0;

deregister_device:
	misc_deregister(&enclave_dev);
	return ret;
}

//static void enclave_ioctl_exit(void)
void enclave_ioctl_exit(void)
{
	//unsigned long size, addr, order, count;
	printk("enclave_ioctl_exit...\n");

	// free cma memory
    dma_free_coherent(enclave_dev.this_device, CMA_SIZE, cma_mem.cma_start_va, cma_mem.cma_handle);

	//TODO: free SM memory
	/*while((addr = SBI_CALL_2(SBI_SM_FREE_ENCLAVE_MEM, &size, FREE_MAX_MEMORY)))
	  {
	  order = ilog2(size-1) + 1;
	  count = 0x1 << order;
	  if(count != size)
	  {
	  printk("KERNEL MODULE:  the number of free pages is not exponential times of two\n");
	  return;
	  }
	  free_pages((long unsigned int)__va(addr), order);
	  }*/

	misc_deregister(&enclave_dev);
	return;
}

module_init(enclave_ioctl_init);
module_exit(enclave_ioctl_exit);
