default:
ifeq ($(OS),Windows_NT)
	gcc -o game.exe src/main.c src/glad/glad.c -lm -lSDL3
else
	/usr/bin/time -f "%e" gcc -o game src/main.c src/glad/glad.c -lm -lSDL3
endif