CC=gcc
CFLAGS=-m32
MOD1 = mem_mod1.c memlayout.c
MOD2 = mem_mod2.c memlayout.c
MOD3 = mem_mod3.c memlayout.c

all: mod1 mod2 mod3
.phony : all	
test :
	$(CC) test_memlayout.c memlayout.c -o test $(CFLAGS)
mod1 :
	$(CC) $(MOD1) -o mod1 $(CFLAGS)
mod2 : 
	$(CC) $(MOD2) -o mod2 $(CFLAGS)
mod3 :
	$(CC) $(MOD3) -o mod3 $(CFLAGS)
clean :
	rm -rf *.o mod3 mod2 mod1 test
