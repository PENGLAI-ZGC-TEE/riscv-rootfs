NAME = busybox
include $(RISCV_ROOTFS_HOME)/Makefile.app

.PHONY: repo
REPO_PATH = repo
$(REPO_PATH):
	-mkdir -p $@
	-git clone --depth 1 -b 1_32_stable https://git.busybox.net/busybox $@
	cp config $@/.config
	-$(MAKE) -C $(REPO_PATH)
	-mkdir $(RISCV_ROOTFS_HOME)/rootfsimg/build
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/busybox
	ln -sf $(abspath $(REPO_PATH)/busybox) $(RISCV_ROOTFS_HOME)/rootfsimg/build/busybox

$(APP): | $(REPO_PATH)
	$(MAKE) -C $(REPO_PATH)
	ln -sf $(abspath $(REPO_PATH)/busybox) $@
