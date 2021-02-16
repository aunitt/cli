

all: 
	scons 

run: all
	build/tdd

test: all
	valgrind -s build/tdd

clean:
	scons -c
	rm -rf build tags

# FIN
