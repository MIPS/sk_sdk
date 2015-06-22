include ${PWD}/Makefile.include

CC=${NWLD_CC}
CFLAGS += ${CFLAGS_NWLD}

OBJECTS = sk_drv_nwld_comm.o  sk_drv_nwld_memory_config.o  sk_drv_nwld_platform.o  sk_drv_nwld_tapi_ioctl.o
ARFLAGS ?= rs

libsknwld_platform.a: ${OBJECTS}
	@${AR} ${ARFLAGS} $@ $^

clean:
	rm -f *.o *.a
