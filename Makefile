all: chip8

run: chip8
	#./chip8 roms/glitchGhost.ch8
	./chip8 roms/br8kout.ch8
	#./chip8 roms/octojam1title.ch8
	#./chip8 roms/snake.ch8
	#./chip8 roms/test_opcode.ch8
	#./chip8 roms/octoachip8story.ch8
	#./chip8 roms/supersquare.ch8
	#./chip8 roms/1dcell.ch8

chip8: main.o cpu.o memory.o
	g++ -o chip8 main.o cpu.o memory.o -lncurses

main.o: main.cc
	g++ -c main.cc

cpu.o: cpu.cc
	g++ -c cpu.cc

memory.o: memory.cc
	g++ -c memory.cc

clean:
	rm -f chip8
	rm -f *.o
