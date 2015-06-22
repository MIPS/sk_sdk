.PHONY: outdbtoc

outdbtoc:
	@mkdir -p outmani outdbtoc
	@../../../tools/tapp_manifest -m hello.mani -i sk_hello.bin -o outmani
	@../../../tools/tapp_manifest -m olleh.mani -i sk_olleh.bin -o outmani
	@../../../tools/tapp_build_toc_db -i outmani -o outdbtoc
	@cp outdbtoc/*.dat ..

clean:
	rm -f outmani/*
	rm -f outdbtoc/*

