#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include "memlayout.h"

// EXTERNALLY DEFINED
//extern PAGE_SIZE;

// Remove this before submission!!
const unsigned int MIN_PAGESIZE=2, MAX_PAGESIZE=65536, PAGE_SIZE=0x800 // some test page size


static jmp_buf env;

void sigseg_handler (int sig_id) {
  siglongjmp(env, 0);   // Seg_fault due to illegal read
}

void sigill_handler (int sig_id) {
  siglongjmp(env, 0);  // Seg_illegal due to illegal write
}


int get_mem_layout (struct memregion *regions, unsigned int size) {

  struct memregion curr_region;      // current region
  unsigned int r_count = 0;          // region count
  void *curr_addr = (void *) 0x0;    // start off address pointer at 0x0
  
//  char readbuf = 0;                  // buffer for reading;
  char current_mode = 0;
  
  int read = 0;
  int write = 0;
  
  
  curr_region.from = curr_addr;
  curr_region.mode = -1;
  
  // sigaction + handler
  struct sigaction act, old_act;
  act.sa_handler = sigseg_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags=0
  
  sigaction(SIGSEGV, &act, &old_act)
  
  
  while (curr_addr < 0xffffffff) { // loop through all memory address
      // check position in regions array
      // return if at end
      if (r_count == size) {
        return size;
      }
    
    curr_addr[0];   // attempt reading
   
    if (sigsetjmp(env, 1)==0) {
      read = 0;
      write= 0;
    } else {
    
      read=1;
      curr_addr[0] = 0;       // attempt writing
      if (sigsetjmp(env, 1)==0) {
        write=0;
      } else {
        write=1;
      }
    }
    
    // determine mode & compare with region
    current_mode = 2-(read+write);
    if (current_mode != curr_region.mode) {
      regions[r_count] = curr_region;
      r_count +=1;
    }
    
    curr_addr += PAGE_SIZE; // increment to next base page address
    curr_region.to = curr_addr - 0x1;   // store the last address
  }
  
  // reset sigaction to previous
  sigaction(SIGSEG, &old_act, 0);
  
  
  return r_count;
}

int get_mem_diff (struct memregion *regions, unsigned int howmany,
  struct memregion *thediff, unsigned int diffsize){
  
  // This function takes the 'old' memregion, the current 'new' region and
  // returns what you would have to put on top of 'old' to get 'new'

  struct memregion *curr_layout;
  get_mem_layout(curr_layout, howmany);
  
  struct memregion curr_r;
  struct memregion old_r;
  
  int start_addr;
  int end_addr;
  
  int diff_counter = 0;
  int c_old = 0;
  int c_curr = 0;
  
  void *oldTo, *oldFrom, *newTo, *newFrom;
  
  int i;
  for (int i=0; i<0xffffffff ; i+=PAGE_SIZE) {
    
    if (i > curr_r.to) {
      c_curr++;
    }
    if (i > old_r.to) {
      c_old++;
    }
    curr_r = regions[c_curr];
    old_r = regions[c_old];
    
    if(curr_r.mode == old_r.mode) {
      continue;
    }
    
    if (curr_r.to > old_r.to) {
      start_addr = curr_r.to;
    }
  }
  
  return diff_counter;
}













