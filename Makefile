CC=g++
ARGS=-Wall
INCLUDE_PATHS=-Isrc/include
LIB_PATHS=-Lsrc/lib
FILES=*.cpp
INCLUDES=-lmingw32 -lSDL2main -lSDL2 

all: compile run

compile:
	${CC} ${ARGS} -o main ${FILES} ${INCLUDE_PATHS} ${LIB_PATHS} ${INCLUDES}
run:
	main.exe