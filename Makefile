
.PHONY: all clean test

all: build/nesemu

build/nesemu: src/*.c include/*.h
	gcc src/*.c -g -o build/nesemu -Og

clean:
	rm build/nesemu

test: all
	build/nesemu misc/roms/nestest.nes
