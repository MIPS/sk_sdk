-include ${PWD}/Makefile.include

CFLAGS += ${CFLAGS_NWLD}

-include ${PKG}/incpaths
-include ${PKG}/nwldlibs

CFLAGS += ${ELPSK_INCPATH}
ALL_APPS = hello_nwld

hello_nwld: hello_nwld.o
	@${CC} ${CFLAGS} ${LDFLAGS} $^ ${ELPSK_NWLDLIBS} -o $@ 

ifeq (${TARGET},tcp)
-include ${PKG}/swldlibs
ALL_APPS += hello_swld

hello_swld: tcp_main.o
	@${CC} ${CFLAGS} ${LDFLAGS} $^ ${ELPSK_SWLDLIBS} -o $@ 
endif

.PHONY: all dbfiles
all: dbfiles ${ALL_APPS}

dbfiles:
	@echo ignore
#	@make -C mani

clean:
	rm -f *.o ${ALL_APPS}

