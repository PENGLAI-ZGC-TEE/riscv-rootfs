NAME = busybox
BUSYBOX_VERSION = 1.32.0
include $(RISCV_ROOTFS_HOME)/Makefile.app

.PHONY: repo
REPO_PATH = repo
$(REPO_PATH):
	-mkdir -p $@
ifeq ($(wildcard ./busybox-$(BUSYBOX_VERSION).tar.bz2),)
	wget https://busybox.net/downloads/busybox-$(BUSYBOX_VERSION).tar.bz2 
endif
ifeq ($(wildcard ./repo/INSTALL),)
	tar xvf busybox-$(BUSYBOX_VERSION).tar.bz2 --directory=repo
	mv ./repo/busybox-$(BUSYBOX_VERSION)/.[^.]* ./repo/busybox-$(BUSYBOX_VERSION)/* ./repo/
	rm -rf ./repo/busybox-$(BUSYBOX_VERSION)/
endif
	cp config $@/.config
	-$(MAKE) -C $(REPO_PATH)
	-mkdir $(RISCV_ROOTFS_HOME)/rootfsimg/build
	-touch $(RISCV_ROOTFS_HOME)/rootfsimg/build/busybox
	ln -sf $(abspath $(REPO_PATH)/busybox) $(RISCV_ROOTFS_HOME)/rootfsimg/build/busybox

$(APP): | $(REPO_PATH)
	$(MAKE) -C $(REPO_PATH)
	ln -sf $(abspath $(REPO_PATH)/busybox) $@
