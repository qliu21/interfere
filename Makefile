CC=cc
CFLAGS=-c
LDFLAGS=
SOURCES=interf.c test_ost_speed.c test_ost_speed_100ost.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=interf test_ost_speed test_ost_speed_100ost

all: interf test_ost_speed test_ost_speed_100ost
interf: interf.c
	${CC} -g -c -DH5_USE_16_API interf.c
	${CC} -g -o interf interf.o
test_ost_speed: test_ost_speed.c
	${CC} -g -c -DH5_USE_16_API test_ost_speed.c
	${CC} -g -o test_ost_speed test_ost_speed.o
test_ost_speed_100ost: test_ost_speed_100ost.c
	${CC} -g -c -DH5_USE_16_API test_ost_speed_100ost.c
	${CC} -g -o test_ost_speed_100ost test_ost_speed_100ost.o
clean:
	rm -f *.o core.* *~ *err *out
	rm -f interf test_ost_speed test_ost_speed_100ost

