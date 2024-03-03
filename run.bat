@echo on
g++ -static -Wall -Wextra -Wpedantic -Ofast -Os -m64 -std=c++2a -o OrdinaryEngine engine/*.c game/*.cpp -I vendor\include -L vendor\lib -lstdc++ -lopengl32 -lglfw3 -lm -lpthread -lgdi32 -luser32 -lkernel32 -llua54
.\OrdinaryEngine.exe