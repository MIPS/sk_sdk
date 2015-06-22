include ${PWD}/Makefile.include
export PWD

OBJECTS = sk_tapi_byte_pack.o sk_tapi_byte_unpack.o sysv_byte_packing.o

ARFLAGS ?= rs

.PHONY: libs

libs:
	@rm -f *.o
	@CC=${NWLD_CC} PWD=${PWD} CFLAGS="${CFLAGS} ${CFLAGS_NWLD}" make libskboth_nwld.a
	@rm -f *.o
	@CC=${SWLD_CC} PWD=${PWD} CFLAGS="${CFLAGS} ${CFLAGS_SWLD}" make libskboth_swld.a

libskboth_nwld.a libskboth_swld.a: ${OBJECTS}
	@${AR} ${ARFLAGS} $@ $^

clean:
	rm -f *.o *.a
