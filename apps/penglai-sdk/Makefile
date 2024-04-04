NAME = penglai-sdk
export PENGLAI_SDK=$(abspath ./repo)

.PHONY: repo clean
REPO_PATH = repo
$(REPO_PATH):
	-mkdir -p $@
	-git clone https://github.com/ICT-TEE/penglai-sdk-for-nemu.git $@
	-$(MAKE) -C $(REPO_PATH)
	-mkdir $(RISCV_ROOTFS_HOME)/rootfsimg/build 
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/host
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/prime
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/mem
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/shm_creater
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/shm_mapper
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/res.txt
	ln -sf $(abspath repo/demo/host/host) $(RISCV_ROOTFS_HOME)/rootfsimg/build/host
	ln -sf $(abspath repo/demo/prime/prime) $(RISCV_ROOTFS_HOME)/rootfsimg/build/prime
	ln -sf $(abspath repo/demo/mem/mem) $(RISCV_ROOTFS_HOME)/rootfsimg/build/mem
	ln -sf $(abspath repo/demo/shm/shm_creater) $(RISCV_ROOTFS_HOME)/rootfsimg/build/shm_creater
	ln -sf $(abspath repo/demo/shm/shm_mapper) $(RISCV_ROOTFS_HOME)/rootfsimg/build/shm_mapper
	ln -sf $(abspath repo/demo/host/res.txt) $(RISCV_ROOTFS_HOME)/rootfsimg/build/res.txt
	


clean: 
	$(MAKE) -C $(REPO_PATH) clean