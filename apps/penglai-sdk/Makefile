NAME = penglai-sdk
export PENGLAI_SDK=$(abspath ./repo)

.PHONY: repo clean
REPO_PATH = repo
$(REPO_PATH):
	-mkdir -p $@
	-git clone -b dev-pxy https://github.com/ICT-TEE/penglai-sdk-for-nemu.git $@
	-$(MAKE) -C $(REPO_PATH)
	-mkdir $(RISCV_ROOTFS_HOME)/rootfsimg/build 
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/host
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/prime
	ln -sf $(abspath repo/demo/host/host) $(RISCV_ROOTFS_HOME)/rootfsimg/build/host
	ln -sf $(abspath repo/demo/prime/prime) $(RISCV_ROOTFS_HOME)/rootfsimg/build/prime


clean: 
	$(MAKE) -C $(REPO_PATH) clean