CC = gcc
CFLAGS = -Wall -Werror -Wno-unused-parameter

V = @

all: libaldl.so

libaldl.o: src/libaldl.c
	@echo + cc src/libaldl.c
	$(V)$(CC) $(CFLAGS) -c src/libaldl.c

libaldl.so: libaldl.o
	@echo + linking libaldl
	$(V)$(CC) $(CFLAGS) -shared -lftdi -o $@ libaldl.o

C1500:  libaldl.so example/C1500.c
	@echo + cc example/C1500.c
	$(V)$(CC) $(CFLAGS) -L.. -laldl -o $@ example/C1500.c

clean:
	@echo + clean
	$(V)rm -rf *.o libaldl.so

install:
	@echo + install libaldl.so to /usr/lib/libaldl.so
	$(V)cp libaldl.so /usr/lib/libaldl.so
	@echo + install libaldl.h to /usr/include/libaldl.so
	$(V)cp inc/libaldl.h /usr/include/libaldl.h

