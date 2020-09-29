all: build/libslsolve.a build/libsldraw.a

build/libslsolve.a: $(patsubst src/%.c,src/%.o,$(wildcard src/*.c)) src/acvector/src/acvector.o
	mkdir -p build
	$(AR) rcs $@ $^

build/libsldraw.a: $(patsubst src/draw/%.c,src/draw/%.o,$(wildcard src/draw/*.c)) src/acvector/src/acvector.o
	mkdir -p build
	$(AR) rcs $@ $^

src/acvector/src/acvector.o: src/acvector/src/acvector.c src/acvector/src/acvector.h src/acvector/makefile
	make -C src/acvector

%.o: %.c
	$(CC) -o $@ -c -O3 -DNDEBUG -Wall -Isrc/acvector/src/ -Isrc/ -I./ $<

clean:
	make -C src/acvector clean
	rm -rf build
	rm -f src/*.o
	rm -f src/draw/*.o

.PHONY: all clean
