all : mod1 mod2 #mod3
	
test :
	gcc test_memlayout.c memlayout.c -o test
#mod3 :
#	gcc mem_mod3.c memlayout.c -o mem_mod3
mod2 :
	gcc mem_mod2.c memlayout.c -o mem_mod2
mod1 :
	gcc mem_mod1.c memlayout.c -o mem_mod1
clean :
	rm -rf *.o mem_mod3 mem_mod2 mem_mod1