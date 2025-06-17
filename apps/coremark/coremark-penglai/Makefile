# NAME = coremark
# SRCS = $(shell find -L ./src/ -name "*.c")
# ifdef ITERATIONS
# CFLAGS += -DITERATIONS=$(ITERATIONS)
# NAME = coremark-$(ITERATIONS)-iteration
# endif

# include $(AM_HOME)/Makefile.app

APP = coremark
APP_C_SRCS = $(shell find -L $(abspath .) -name "*.c")
# CFLAGS += -I$(abspath ./include)
EXTRA_CLEAN = $(APP).dump
include $(PENGLAI_SDK)/app.mk
