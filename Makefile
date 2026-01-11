default:
	
	/usr/bin/time -f "%e" gcc -o game src/main.c src/glad/glad.c -lm -lSDL3
