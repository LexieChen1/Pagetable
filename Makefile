CC = clang
CFLAGS = -Wall -Wextra 
LFLAGS = -Wall

# Object file
OBJS = mlpt.o 

# default target: static library 
all: libmlpt.a

libmlpt.a: $(OBJS)
	ar rcs $@ $^

mlpt.o: mlpt.c mlpt.h config.h
	$(CC) $(CFLAGS) -c mlpt.c -o mlpt.o 

test_pagetable: test_pagetable.c libmlpt.a
	$(CC) $(CFLAGS) test_pagetable.c -L. -lmlpt -o test_pagetable

clean:
	rm -f *.o *.a test_pagetable
