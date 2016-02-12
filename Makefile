CFLAGS=`pkg-config epoxy sdl2 --cflags` -g -Wall
LIBS=`pkg-config epoxy sdl2 --libs`

all : halls

halls : halls.o
	$(CC) -o $@ $^ $(LIBS)

.c.o :
	$(CC) -c -o $@ $< $(CFLAGS)

clean :
	rm -f *.o halls

.PHONY : clean all
