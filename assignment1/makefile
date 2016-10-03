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
	$(CC) $(MOD1) -o mem_mod1 $(CFLAGS)
mod2 : 
	$(CC) $(MOD2) -o mem_mod2 $(CFLAGS)
mod3 :
	$(CC) $(MOD3) -o mem_mod3 $(CFLAGS)
clean :
	rm -rf *.o mem_mod3 mem_mod2 mem_mod1 
