NAME = penglai-sdk
export PENGLAI_SDK=$(abspath ./repo)

.PHONY: repo clean
REPO_PATH = repo
$(REPO_PATH):
	-mkdir -p $@
	-git clone -b zgc-lab-demo git@github.com:PENGLAI-ZGC-TEE/penglai-sdk-for-nemu.git $@
	-$(MAKE) -C $(REPO_PATH)
	-mkdir $(RISCV_ROOTFS_HOME)/rootfsimg/build 
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/host_malware
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/fdi_attack_origin
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/fdi_attack_defend
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/input.bin
	ln -sf $(abspath repo/demo/zgc-lab-demo/host_malware/host_malware) $(RISCV_ROOTFS_HOME)/rootfsimg/build/host_malware
	ln -sf $(abspath repo/demo/zgc-lab-demo/host_malware/input.bin) $(RISCV_ROOTFS_HOME)/rootfsimg/build/input.bin
	ln -sf $(abspath repo/demo/zgc-lab-demo/fdi_attack_origin/fdi_attack_origin) $(RISCV_ROOTFS_HOME)/rootfsimg/build/fdi_attack_origin
	ln -sf $(abspath repo/demo/zgc-lab-demo/fdi_attack_defend/fdi_attack_defend) $(RISCV_ROOTFS_HOME)/rootfsimg/build/fdi_attack_defend

clean: 
	$(MAKE) -C $(REPO_PATH) clean
