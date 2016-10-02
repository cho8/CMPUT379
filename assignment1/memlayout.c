#include "memlayout.h"

// EXTERNALLY DEFINED
//extern PAGE_SIZE;

sigjmp_buf env;

void sigseg_handler (int sig_id) {
  siglongjmp(env, 1);   // Seg_fault due to illegal op
}

int get_mem_layout (struct memregion *regions, unsigned int size) {

  int r_count = 0;    // region count
  int jmp_var = 0;

  char* curr_addr;	  // address for read/write ops
  char readbuf = 0;   // buffer to read from address
  
  int old_mode = -1;
  int curr_mode = -1;
  
  // sigaction + handler
  struct sigaction act, old_act;
  act.sa_handler = sigseg_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags=0;
  
  sigaction(SIGSEGV, &act, &old_act);
  
  // loop through all memory addresses
  unsigned int i;
  for (i=0; i<0xfffffffe; i+=PAGE_SIZE) {
    
    curr_addr = (char*)(long)i; // current address

    // Determine the mode
    jmp_var = sigsetjmp(env,1);
    if (jmp_var==0) {
      // attempt reading
      curr_mode=MEM_RO;
      readbuf = *curr_addr;

      // attempt writing
      curr_mode=MEM_RW;
      *curr_addr = readbuf;

    } else {
      // program segfault
      if (curr_mode == MEM_RO) {
    	curr_mode = MEM_NO;		// tried to read but failed
      } else if (curr_mode == MEM_RW) {
        curr_mode=MEM_RO;		// failed to write after read but failed
      }
    }

    // base case
    if (i==0) {
    	old_mode=curr_mode;
    	regions[r_count].mode=curr_mode;
    	regions[r_count].from = (void*)(long) 0;;
    }
    
    // compare modes
    if (curr_mode != old_mode) {
           
        // This is a new mode area
        if (r_count < size) {
          // Check if we can store entries and commit to regions
          regions[r_count].to = (void*)(long)(i-0x1);
          regions[r_count].mode = old_mode;
          // start a "new" region
          r_count+=1;
          regions[r_count].from = (void*)(long)i;
      } else {
    	  // can't store anymore
    	  r_count+=1;
      }
    // else curr==old, same region

    }
    old_mode = curr_mode;

    //sanity check for iterating over last address
    if ( (i+PAGE_SIZE)<i) {
    	if (r_count<=size) {
    		regions[r_count].to=(void*)0xffffffff;
    		regions[r_count].mode=curr_mode;
    	}
    	break;
    }
  }
  
  // reset previous sigaction
  sigaction(SIGSEGV, &old_act, 0);

  return r_count+1; //return actual number of regions
}


int get_mem_diff (struct memregion *regions, unsigned int howmany, struct memregion *thediff, unsigned int diffsize){
  
  // This function takes the 'old' memregion, the current 'new' region and
  // returns what you would have to put on top of 'old' to get 'new'

  int diff_count = 0;		// thediff index
  int old_i = 0;			// regions index
  
  int last_old_mode = -1;
  int last_curr_mode = -1;
  int curr_mode, old_mode;
  
  int jmp_var = 0;
  char readbuf = 0;
  char* curr_addr;

  // sigaction + handler
  struct sigaction act, old_act;
  act.sa_handler = sigseg_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags=0;

  sigaction(SIGSEGV, &act, &old_act);

  // scan addresses
  unsigned int i;
  for (i=0; i<0xffffffff ; i+=PAGE_SIZE) {
    curr_addr = (char*)(long)i; // current address

	// Determine the old mode at i
    if (i > (unsigned int )regions[old_i].to) {
      old_i++;
    }
    
    // Determine the current mode at i
    old_mode = regions[old_i].mode;
    jmp_var = sigsetjmp(env,1);

    if (jmp_var==0) {
      // attempt reading
      curr_mode=MEM_RO;
      readbuf = *curr_addr;   // attempt reading

      // attempt writing
      curr_mode=MEM_RW;
      *curr_addr = readbuf;;

    } else {
      if (curr_mode == MEM_RO) {
    	curr_mode = MEM_NO;		// Reading failed
      } else if (curr_mode == MEM_RW) {
        curr_mode=MEM_RO;		// Writing failed (and cannot not read)
      }
    }


    // if current i mode and old i mode are the same
    if (curr_mode == old_mode) {
      if (last_old_mode != last_curr_mode){
        // regions now the same mode, but were previously different,
        // which means an end to diff_region
        thediff[diff_count].to = (void*)(long)(i-0x1);
        diff_count++;
      }

    // if curr_mode != old_mode
    } else {
      if (last_old_mode == last_curr_mode) {
        // start a diff_region
        thediff[diff_count].from = (void*)(long)i;
        thediff[diff_count].mode = curr_mode;
      } 
      if (last_curr_mode == curr_mode) {
      	thediff[diff_count].to=(void*)(long)i;
      } else if (last_old_mode == last_curr_mode) {
      	thediff[diff_count].from = (void*)(long)i;
      	thediff[diff_count].mode = curr_mode;
      } else if (last_curr_mode != curr_mode) {
        // suddenly a very different region
        thediff[diff_count].to =(void*)(long)(i-0x1);
        diff_count++;
        thediff[diff_count].from=(void*)(long)i;
        thediff[diff_count].mode=curr_mode;
      }
    }
    // update last modes
    last_curr_mode=curr_mode;
    last_old_mode=old_mode;

    //sanity check
    if ((i+PAGE_SIZE)<i) {
      break;
    }
  }

  sigaction(SIGSEGV, &old_act, 0);

  return diff_count;
}














