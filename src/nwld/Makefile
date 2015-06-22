include ${PWD}/Makefile.include

CC=${NWLD_CC}
CFLAGS += ${CFLAGS_NWLD}

OBJECTS = sk_drv_nwld_tapi.o sk_teec_api.o

ARFLAGS ?= rs

libsknwld.a: ${OBJECTS}
	@${AR} ${ARFLAGS} $@ $^

clean:
	rm -f *.o *.a

