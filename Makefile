CC=cc
CFLAGS=-c
LDFLAGS=
SOURCES=interf.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=interf

all: interf
interf: interf.c
	${CC} -g -c -DH5_USE_16_API interf.c
	${CC} -g -o interf interf.o

clean:
	rm -f *.o core.* *~ *err *out
	rm -f interf

