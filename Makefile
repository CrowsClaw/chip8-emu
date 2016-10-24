COMPILER=clang++

OUTPUT=chipemu

SRC=main.cpp chip8.cpp chip8.h

LIBS= -L/usr/local/lib -I/usr/local/include  -lsfml-graphics -lsfml-window -lsfml-system

LINKER=-std=c++11 -stdlib=libc++

default: build

build:
	$(COMPILER) $(LINKER)  $(SRC)  $(LIBS)  

run:
	./$(OUTPUT)

clean:
	rm *.o $(OUTPUT)
