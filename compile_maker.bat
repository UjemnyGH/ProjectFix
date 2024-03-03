@echo off
echo Compiling SerialIO.exe
gcc -DSIZE=8 -Ofast -m64 -std=c2x -Wall -Wextra -Wpedantic -Werror -o PixelMaker8.exe make_char.c gl.c -L vendor\lib -I vendor\include -lopengl32 -lglfw3 -lm -lpthread -lgdi32 -luser32 -lkernel32
gcc -DSIZE=16 -Ofast -m64 -std=c2x -Wall -Wextra -Wpedantic -Werror -o PixelMaker16.exe make_char.c gl.c -L vendor\lib -I vendor\include -lopengl32 -lglfw3 -lm -lpthread -lgdi32 -luser32 -lkernel32
gcc -DSIZE=32 -Ofast -m64 -std=c2x -Wall -Wextra -Wpedantic -Werror -o PixelMaker32.exe make_char.c gl.c -L vendor\lib -I vendor\include -lopengl32 -lglfw3 -lm -lpthread -lgdi32 -luser32 -lkernel32
gcc -DSIZE=64 -Ofast -m64 -std=c2x -Wall -Wextra -Wpedantic -Werror -o PixelMaker64.exe make_char.c gl.c -L vendor\lib -I vendor\include -lopengl32 -lglfw3 -lm -lpthread -lgdi32 -luser32 -lkernel32
echo SerialIO.exe compiled succesfully