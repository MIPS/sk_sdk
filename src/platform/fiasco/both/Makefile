include ${PWD}/Makefile.include

OBJECTS =   sysv_log.o sysv_common.o

ARFLAGS ?= rs

.PHONY: libs

libs:
	@rm -f *.o
	@CC=${NWLD_CC} PWD=${PWD} CFLAGS="${CFLAGS} ${CFLAGS_NWLD} -DSK_NWLD" make libskboth_platform_nwld.a
	@rm -f *.o
	@CC=${SWLD_CC} PWD=${PWD} CFLAGS="${CFLAGS} ${CFLAGS_SWLD}" make libskboth_platform_swld.a

libskboth_platform_swld.a: ${OBJECTS}
	@${AR} ${ARFLAGS} $@ $^

libskboth_platform_nwld.a: ${OBJECTS}
	@${AR} ${ARFLAGS} $@ $^

clean:
	rm -f *.o *.a

