all: storylinesdrawer

storylinesdrawer: $(patsubst %.c,%.o,$(wildcard *.c)) ../acvector/src/acvector.o
	mkdir -p ../../build
	$(AR) rcs ../../build/lib$@.a $^

../acvector/src/acvector.o: ../acvector/src/acvector.c
	make -C ../acvector/

%.o: %.c
	$(CC) -c -O3 -DNDEBUG -Wall -I../acvector/src/ -I../../ -I../ $<

clean:
	make -C ../acvector/ clean
	-rm ../../build/libstorylinesdrawer.a
	-rmdir ../../build/
	-rm *.o

.PHONY: all
