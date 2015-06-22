include Makefile.tools

ifndef TARGET
install lib demos fail:
	@echo You must define a TARGET platform to build for:
	@for f in src/platform/*; do printf "   %15s -- " `basename $$f`; head -n1 <  $$f/desc; done
else

PWD=$(shell pwd)
export PWD

CFLAGS += \
-I${PWD}/src/swld/include -I${PWD}/src/nwld/include -I${PWD}/src/both/include \
-I${PWD}/src/platform/${TARGET}/nwld/include -I${PWD}/src/platform/${TARGET}/swld/include -I${PWD}/src/platform/${TARGET}/both/include

CFLAGS += -W -Wall -g3

export CFLAGS

.PHONY: demos lib install clean
lib:
	@rm -rf libs
	@mkdir -p libs
	@make -C src/nwld
	@make -C src/swld
	@make -C src/both
	@make -C src/platform/${TARGET}/nwld
	@make -C src/platform/${TARGET}/both
	@make -C src/platform/${TARGET}/swld
	@cp src/nwld/*.a src/both/*.a src/swld/*.a  src/platform/${TARGET}/swld/*.a src/platform/${TARGET}/nwld/*.a src/platform/${TARGET}/both/*.a libs/

IDIR ?= ${PWD}/pkg/

install: lib
	@rm -rf pkg
	@mkdir -p ${IDIR}/lib/../include/swld/../nwld/../both/../platform/swld/../nwld/../both
	@cp libs/*.a ${IDIR}/lib/
	@cp src/swld/include/* ${IDIR}/include/swld
	@cp src/nwld/include/* ${IDIR}/include/nwld
	@cp src/both/include/* ${IDIR}/include/both
	@cp src/platform/${TARGET}/swld/include/* ${IDIR}/include/platform/swld
	@cp src/platform/${TARGET}/nwld/include/* ${IDIR}/include/platform/nwld
	@cp src/platform/${TARGET}/both/include/* ${IDIR}/include/platform/both
	@echo ELPSK_INCPATH="-I${IDIR}/include/swld -I${IDIR}/include/nwld -I${IDIR}/include/both -I${IDIR}/include/platform/swld -I${IDIR}/include/platform/nwld -I${IDIR}/include/platform/both" > ${IDIR}/incpaths
	@echo ELPSK_SWLDLIBS1="${IDIR}/lib/libskswld.a ${IDIR}/lib/libskswld_platform.a ${IDIR}/lib/libskboth_swld.a ${IDIR}/lib/libskboth_platform_swld.a" > ${IDIR}/swldlibs
	@echo ELPSK_SWLDLIBS='$${ELPSK_SWLDLIBS1} $${ELPSK_SWLDLIBS1}' >> ${IDIR}/swldlibs
	@echo ELPSK_NWLDLIBS1="${IDIR}/lib/libsknwld.a ${IDIR}/lib/libsknwld_platform.a ${IDIR}/lib/libskboth_nwld.a ${IDIR}/lib/libskboth_platform_nwld.a" > ${IDIR}/nwldlibs
	@echo ELPSK_NWLDLIBS='$${ELPSK_NWLDLIBS1} $${ELPSK_NWLDLIBS1}' >> ${IDIR}/nwldlibs

demos: install
	@PKG=${IDIR} make -C src/demos/hello all

tools:
	@make install
	@PKG=${IDIR} make -C src/tools
	@make clean

clean:
	@PKG=${IDIR} make -C src/demos/hello clean
	@make -C src/nwld clean
	@make -C src/swld clean
	@make -C src/both clean
	@make -C src/platform/${TARGET}/nwld clean
	@make -C src/platform/${TARGET}/swld clean
	@make -C src/platform/${TARGET}/both clean
	@rm -rf pkg libs

cleanall: clean
	@PKG=${IDIR} make -C src/tools clean

endif
