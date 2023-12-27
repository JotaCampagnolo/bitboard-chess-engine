all:
	gcc -oFast engine.c -o engine
	x86_64-w64-mingw32-gcc -oFast engine.c -o engine

debug:
	gcc engine.c -o engine
	x86_64-w64-mingw32-gcc engine.c -o engine