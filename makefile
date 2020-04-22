all: storylines draw

storylines:
	make -C src/

draw:
	make -C src/draw

clean:
	make -C src/ clean
	make -C src/draw clean

.PHONY: all storylines draw clean
