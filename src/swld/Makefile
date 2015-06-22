include ${PWD}/Makefile.include

CC=${SWLD_CC}

CFLAGS += ${CFLAGS_SWLD}

OBJECTS = my_rc4_prng.o my_tapi_handle_generator_plugin.o tapi.o tapi_message_interface.o tapp.o tapp_manifest_pack.o tapp_toc.o tapp_toc_shared.o

ARFLAGS ?= rs

libskswld.a: ${OBJECTS}
	@${AR} ${ARFLAGS} $@ $^

clean:
	rm -f *.o *.a

