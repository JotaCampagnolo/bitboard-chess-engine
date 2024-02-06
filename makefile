all:
	gcc -Ofast engine.c -o engine
	x86_64-w64-mingw32-gcc -Ofast engine.c -o engine

debug:
	gcc engine.c -o engine
	x86_64-w64-mingw32-gcc engine.c -o engine