none :
	@echo "Usage: make [clean | <process>]"

quicksort : clean simulator
	gcc -c quicksort.c
	gcc simulator.o quicksort.o -o simulation

heapsort : clean simulator
	gcc -c heapsort.c
	gcc simulator.o heapsort.o -o simulation

example : clean simulator
	gcc -c example.c
	gcc simulator.o example.o -o simulation

simulator :
	gcc -c simulator.c

clean :
	rm -f *.o simulation
