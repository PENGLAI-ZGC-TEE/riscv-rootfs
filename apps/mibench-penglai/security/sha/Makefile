# By default, the code is compiled for a "big endian" machine.
# To compile on a "little endian" machine set the LITTLE_ENDIAN flag.
# To make smaller object code, but run a little slower, don't use UNROLL_LOOPS.
# To use NIST's modified SHA of 7/11/94, define USE_MODIFIED_SHA

APP = sha
APP_C_SRCS = sha.c sha_driver.c
EXTRA_CLEAN = $(APP).dump
include $(PENGLAI_SDK)/app.mk
