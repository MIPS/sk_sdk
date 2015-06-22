-include ${PWD}/Makefile.include
-include ${PKG}/incpaths
-include ${PKG}/swldlibs

CFLAGS += ${ELPSK_INCPATH}

COMMON_OBJ = common_tools.o

all: tapp_build_toc_db tapp_manifest

tapp_build_toc_db: tapp_build_toc_db.o ${COMMON_OBJ}
	@${CC} ${CFLAGS} ${LDFLAGS} $^ ${ELPSK_SWLDLIBS} -o $@ 

tapp_manifest: tapp_manifest.o ${COMMON_OBJ}
	@${CC} ${CFLAGS} ${LDFLAGS} $^ ${ELPSK_SWLDLIBS} -o $@ 

clean:
	rm -f *.o tapp_build_toc_db tapp_manifest
