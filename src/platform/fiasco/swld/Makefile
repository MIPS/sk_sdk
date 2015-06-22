include ${PWD}/Makefile.include

CC=${SWLD_CC}
CFLAGS += ${CFLAGS_SWLD}

OBJECTS = sk_drv_server_comm.o  sk_drv_swld_kernel_interface.o  sk_drv_swld_memory_config_interface.o  sk_drv_swld_secure_storage_interface.o  sk_drv_swld_secure_timer_interface.o  sk_tapp_swld_app_interface.o  swld_vm_fiasco.o
ARFLAGS ?= rs

libskswld_platform.a: ${OBJECTS}
	@${AR} ${ARFLAGS} $@ $^

clean:
	rm -f *.o *.a

