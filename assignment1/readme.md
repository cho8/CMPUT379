# CMPUT379
Fall 2016
Assignment 1
Christina Ho
Cristian Dumitrescu

mem_mod1
  The regions printed out in mem_mod1 belong to the heap space. The operation performed to change stack space. sbrk() alters the termination location of a process data. In our instance, the process data end location is issued two page sizes later. The stack space is RW as it is allocated for variables

mem_mod2
  The regions produced by mem_mod2 are allocated by malloc, which manages the memory space on the heap. In our instance, the memory space of certain RW blocks are expanded.
  
mem_mod3
  This driver uses mprotect() to change the read-write access privileges on a region of memory. By selecting on a specified amount of memory that we altered before the initial memory layout was called, mprotect directly alters the read-write access of the same amount of memory.    
