#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include "memlayout.h"

// EXTERNALLY DEFINED
//extern PAGE_SIZE;

// Remove this before submission!!
const unsigned int MIN_PAGESIZE=2, MAX_PAGESIZE=65536, PAGE_SIZE=0x800; // some test page size


static jmp_buf env;

void sigseg_handler (int sig_id) {
  siglongjmp(env, 0);   // Seg_fault due to illegal read
}



int get_mem_layout (struct memregion *regions, unsigned int size) {

  struct memregion curr_region;      // current region
  unsigned int r_count = 0;          // region count   // start off address pointer at 0x0
  char *curr_addr = (char *)0x0;
  char readbuf = 0;                  // buffer for reading;
  
  char current_mode = 0;
  
  int read = 0;
  int write = 0;
  
  
  curr_region.from = 0x0;
  curr_region.mode = -1;
  
  // sigaction + handler
  struct sigaction act, old_act;
  act.sa_handler = sigseg_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags=0;
  
  sigaction(SIGSEGV, &act, &old_act);
  
  // loop through all memory addresses
  int i;
  for (i=0; i<0xffffffff; i+=PAGE_SIZE) {
    // check position in regions array
    // return if at end
    if (r_count == size) {
      return size;
    }
    
    *curr_addr = i; // current address
    
    // Determine the mode
    readbuf = (char)curr_addr[0];   // attempt reading
   
    if (sigsetjmp(env, 1)==0) {     // not readable
      current_mode=MEM_NO;
      
    } else {                        // attempt writing
      curr_addr[0] = 0;
      
      if (sigsetjmp(env, 1)==0) {   // not writeable
        current_mode=MEM_RO;
      } else {
        current_mode=MEM_RW;
      }
    }
    
    // compare with region
    // new region if diff mode
    if (current_mode != curr_region.mode) {
      
      // save the end of the previous region and commit to struct
      curr_region.to = (void*)(curr_addr - 0x1);
      regions[r_count] = curr_region;
      
      // set beginning of new region to the current address where new mode occurs
      curr_region.from = (void*) curr_addr;
      curr_region.mode = current_mode;
      r_count +=1;
    }
    // extend the "to" pointer to the addr we're on and update existing region
    curr_region.to = curr_addr;
    regions[r_count] = curr_region;
  }
  
  // reset sigaction to previous
  sigaction(SIGSEGV, &old_act, 0);
  
  
  return r_count;
}


//int get_mem_diff (struct memregion *regions, unsigned int howmany,
//  struct memregion *thediff, unsigned int diffsize){
//  
//  // get a new snapshot of memory
//  struct memregion *curr_layout;
//  get_mem_layout(curr_layout, howmany);
//  
//  int diff_counter=0;
//  int curr_i = 0;    //current index in regions struct
//  int old_i = 0;     //current index in curr_layout
//  
//  struct memregion curr_region = curr_layout[curr_i];
//  struct memrgeion old_region = regions[old_i];
//  
//  int i;
//  for (i=0; i<0xffffffff; i+=PAGE_SIZE) {
//    //
//    if (curr_region.from == curr_region.from && curr_region.to == old_region.to) {
//      continue;
//    }
//    
//  }
//  return 0;
//}














