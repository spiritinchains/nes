
# not super 'robust' but it gets the job done

.PHONY: run clean

build/nesemu: src/*.c include/*.h
	gcc -Iinclude src/*.c -o build/nesemu

run: build/nesemu
	build/nesemu

clean:
	rm build/nesemu
