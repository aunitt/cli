

all:
	scons

arm:
	scons arm

tdd:
	scons tdd

run: all
	build/tdd

test: tdd
	valgrind build/tdd

clean:
	scons -c
	rm -rf build tags
	rm -rf build_arm tags

# FIN
