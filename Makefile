all: chip8

run: chip8
	./chip8 roms/glitchGhost.ch8
	#./chip8 roms/br8kout.ch8

chip8: main.o cpu.o memory.o screen.o
	g++ -o chip8 main.o cpu.o memory.o screen.o -lSDL2 -lSDL2main -pthread

main.o: main.cc
	g++ -c main.cc

cpu.o: cpu.cc
	g++ -c cpu.cc

memory.o: memory.cc
	g++ -c memory.cc

screen.o: screen.cc
	g++ -c screen.cc

clean:
	rm -f chip8
	rm -f *.o
