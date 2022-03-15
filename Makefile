
.PHONY: all clean test

all: build/nesemu

build/nesemu: src/*.c include/*.h
	gcc -Iinclude src/*.c -o build/nesemu

clean:
	rm build/nesemu

test: all
	build/nesemu misc/roms/nestest.nes
