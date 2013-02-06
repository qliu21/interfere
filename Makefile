CC=cc
CFLAGS=-c
LDFLAGS=
SOURCES=interf.c test_ost_speed.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=interf test_ost_speed

all: interf test_ost_speed
interf: interf.c
	${CC} -g -c -DH5_USE_16_API interf.c
	${CC} -g -o interf interf.o
test_ost_speed: test_ost_speed.c
	${CC} -g -c -DH5_USE_16_API test_ost_speed.c
	${CC} -g -o test_ost_speed test_ost_speed.o
clean:
	rm -f *.o core.* *~ *err *out
	rm -f interf test_ost_speed

