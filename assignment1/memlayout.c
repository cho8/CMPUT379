#include "memlayout.h"

// EXTERNALLY DEFINED
//extern PAGE_SIZE;

// Remove this before submission!!
//const unsigned int MIN_PAGESIZE=2, MAX_PAGESIZE=65536, PAGE_SIZE=0x80; // some test page size


sigjmp_buf env;

void sigseg_handler (int sig_id) {
//  printf("===SEGFAULT!\n");
  siglongjmp(env, 1);   // Seg_fault due to illegal op
}

int get_mem_layout (struct memregion *regions, unsigned int size) {


  int r_count = 0;    // region count
  int jmp_var=0;

  char* curr_addr;
  char readbuf = 0;                  // buffer for reading;
  
  int old_mode = -1;
  int curr_mode = -1;
  
  // sigaction + handler
  struct sigaction act, old_act;
  act.sa_handler = sigseg_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags=0;
  
  sigaction(SIGSEGV, &act, &old_act);
  
  regions[r_count].from = 0x0;

  // loop through all memory addresses
  unsigned int i;
  for (i=0; i<0xfffffffe; i+=PAGE_SIZE) {
    
    curr_addr = (char*)(long)i; // current address

    // Determine the mode
    jmp_var = sigsetjmp(env,1);
    if (jmp_var==0) {
      // attempt reading
      curr_mode=MEM_RO;
      readbuf = *curr_addr;   // attempt reading

      // attempt writing
      curr_mode=MEM_RW;
      *curr_addr = readbuf;

    } else {
      // something blew up
      if (curr_mode == MEM_RO) {
    	// failed to write

    	curr_mode = MEM_NO;
      } else if (curr_mode == MEM_RW) {
    	// failed to write after read
        curr_mode=MEM_RO;
      }
    }

    
    // compare with region
    // new region if diff mode from before
    if (curr_mode != old_mode) {
      
      if (old_mode == regions[r_count].mode) {
        // This is a new moded area
        if (r_count < size) {
          // Check if we can store entries
          // commit the region to array
          regions[r_count].to = (void*)(long)(i-0x1);
          r_count+=1;
          regions[r_count].from = (void*)(long)i;
        }
      }
    // else curr==old, same region

    }
    old_mode = curr_mode;

    //sanity check
    if ( (i+PAGE_SIZE)<i) {
	if (r_count<=size) {
    		regions[r_count].to=(void*)0xffffffff;
    		regions[r_count].mode=curr_mode;
	}
    	break;
    }
  }
  
  // reset sigaction to previous
  sigaction(SIGSEGV, &old_act, 0);
  
  
  return r_count+1; //return index+1
}


int get_mem_diff (struct memregion *regions, unsigned int howmany, struct memregion *thediff, unsigned int diffsize){
  
  // This function takes the 'old' memregion, the current 'new' region and
  // returns what you would have to put on top of 'old' to get 'new'


  int diff_count = 0;
  int c_old = 0;
  int c_curr = 0;
  
  int last_old_mode = -1;
  int last_curr_mode = -1;
  int curr_mode, old_mode;
  
  int jmp_var = 0;
  char readbuf = 0;
  char* curr_addr = (char*)0x0;

  // sigaction + handler
  struct sigaction act, old_act;
  act.sa_handler = sigseg_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags=0;

  sigaction(SIGSEGV, &act, &old_act);


  unsigned int i;
  for (i=0; i<0xffffffff ; i+=PAGE_SIZE) {
    
	curr_addr = (char*)(long)i; // current address

	 // find the region that i is currently in
    if (i > (unsigned int )regions[c_old].to) {
      c_old++;
    }
    
//    printf("current region: %-10p - %-10p       address %#x\n", regions[c_old].from, regions[c_old].to, i);

    // Determine the current mode
    old_mode = regions[c_old].mode;
    jmp_var = sigsetjmp(env,1);

    if (jmp_var==0) {
      // attempt reading
//      printf("Attempting reading\n");
      curr_mode=MEM_RO;
      readbuf = *curr_addr;   // attempt reading

      // attempt writing
      curr_mode=MEM_RW;
//      printf("Read success... Attempting Writing\n");
      *curr_addr = readbuf;
//      printf("====RW\n");

    } else {
      if (curr_mode == MEM_RO) {
    	// Reading failed
    	curr_mode = MEM_NO;
//    	printf("====NO\n");
      } else if (curr_mode == MEM_RW) {
    	// Reading failed (and cannot not read)
        curr_mode=MEM_RO;
//        printf("====RO\n");
      }
    }
//    printf("old_mode %d curr_mode %d\n", old_mode, curr_mode);


    // if current i mode and old i mode are the same modes
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
        // still in the same diff_region
        // update to pointer
        // might be redundant here
        thediff[diff_count].to =(void*)(long)(i-0x1);
      }
    }
    // update last modes
    last_curr_mode=curr_mode;
    last_old_mode=old_mode;

    //sanity check
    if (i<0xffffffff && i+PAGE_SIZE<i) {
      break;
    }
  }

  sigaction(SIGSEGV, &old_act, 0);

  return diff_count;
}














