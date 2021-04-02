
.PHONY: ctags

all:
	scons

arm:
	scons arm

tdd:
	scons tdd

run: all
	build/tdd

test: tdd
	build/tdd

valgrind: tdd
	valgrind build/tdd

clean:
	scons -c
	rm -rf build tags
	rm -rf build_arm tags

ctags:
	ctags -R --exclude=build --exclude=build_arm

# FIN
