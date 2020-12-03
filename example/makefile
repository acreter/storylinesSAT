OBJS:=$(patsubst src/%.c,src/%.o,$(wildcard src/*.c))

all: build/sl-minisat build/sl-cadical

build/sl-cadical: cadical slsolve sldraw $(OBJS)
	mkdir -p build
	g++ -o build/sl-cadical $(OBJS) -Lsrc/cadical/build/ -L../build/ -lsldraw -lplot -lm -lslsolve -lcadical
	
build/sl-minisat: minisat slsolve sldraw $(OBJS)
	mkdir -p build
	g++ -o build/sl-minisat $(OBJS) -Lsrc/minisat/build/release/lib/ -L../build -lsldraw -lplot -lm -lslsolve -lminisat

minisat:
	make -C src/minisat

cadical: src/cadical/makefile
	make -C src/cadical

src/cadical/makefile: src/cadical/configure
	(cd ./src/cadical && ./configure)

slsolve:
	make -C .. build/libslsolve.a

sldraw:
	make -C .. build/libsldraw.a

%.o: %.c
	$(CC) -o $@ -c -O3 -DNDEBUG -DSLDRAW -Wall -I.. $<

clean:
	rm -rf build/
	rm -f src/*.o
	rm -f *.svg

purge: clean
	make -C src/cadical clean
	make -C src/minisat clean

.PHONY: all clean minisat cadical slsolve sldraw purge
