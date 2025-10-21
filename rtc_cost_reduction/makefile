# Makefile for GCC g++ ###########################
all: clean library models planets testing

testing: lib
	make -C testing -f makefile.gcc

library: lib
	make -C src -f makefile.gcc

models: lib
	make -C model -f makefile.gcc

planets: lib models
	make -C planet -f makefile.gcc

lib:
	mkdir $lib

clean:
	make -C src -f makefile.gcc clean
	make -C model -f makefile.gcc clean
	make -C planet -f makefile.gcc clean
	make -C testing -f makefile.gcc clean
