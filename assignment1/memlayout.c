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
      r_count++;
      // check if we're out of bounds
      if (r_count == size) {
        return r_count;
      }
     }
    // extend the "to" pointer to the addr we're on and update existing region
    curr_region.to = curr_addr;
    regions[r_count] = curr_region;
  }
  
  // reset sigaction to previous
  sigaction(SIGSEGV, &old_act, 0);
  
  
  return r_count;
}

int get_mem_diff (struct memregion *regions, unsigned int howmany,
  struct memregion *thediff, unsigned int diffsize){
  
  // This function takes the 'old' memregion, the current 'new' region and
  // returns what you would have to put on top of 'old' to get 'new'

  struct memregion *curr_layout;
  get_mem_layout(curr_layout, howmany);
  
  struct memregion diff_region;
  
  int diff_count = 0;
  int c_old = 0;
  int c_curr = 0;
  
  int last_old_mode = regions[0].mode;
  int last_curr_mode = curr_layout[0].mode;
  int curr_mode, old_mode;
  
  int i;
  for (i=0; i<0xffffffff ; i+=PAGE_SIZE) {
    
    // if we're at the end of thediff, return size of thediff
    if (diff_count == diffsize) {
      return diffsize;
    }

    // find the region that i is currently in
    if (i > (int )curr_layout[c_curr].to) {
      c_curr++;
    }
    if (i > (int )regions[c_old].to) {
      c_old++;
    }
    
    // find the mode of the region i is in
    old_mode = regions[c_old].mode;
    curr_mode = regions[c_curr].mode;
    
    
    
    // if current i mode and old i mode are the same modes
    if (curr_mode == old_mode) {
      if (last_old_mode == last_curr_mode) {
        // No change in mode
        continue;
      } else {
        // regions now the same mode, but were previously different,
        // which means an end to diff_region
        diff_region.to = (void*)(long)(i-0x1);
        thediff[diff_count] = diff_region;
        diff_count++;
      }

    // if curr_mode != old_mode
    } else {
      if (last_old_mode == last_curr_mode) {
        // start a diff_region
        diff_region.from = (void*)(long)i;
        diff_region.mode = curr_mode;
      }
      if (last_curr_mode == curr_mode) {
        // still in the same diff_region
        // update to pointer
        // might be redundant here
        diff_region.to=(void*)(long)(i-0x1);
      }
    }
    // update last modes
    last_curr_mode=curr_mode;
    last_old_mode=old_mode;
  }

  return diff_count;
}














