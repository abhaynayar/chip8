all: chip8

chip8: main.o CPU.o Memory.o
	g++ -o chip8 main.o CPU.o Memory.o

memory.o: Memory.cc
	g++ -c Memory.cc

main.o: main.cc
	g++ -c main.cc

cpu.o: CPU.cc
	g++ -c CPU.cc

#screen.o: Screen.cc
	#g++ Screen.cc -lSDL2main -lSDL2 -o screen

clean:
	rm -f chip8
	rm -f *.o
