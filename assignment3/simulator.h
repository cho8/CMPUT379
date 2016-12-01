#ifndef SIMULATOR_H
#define SIMULATOR_H

// Function declarations
void process();
void init (int psize, int winsize);
void put (unsigned int address, int value);
int get (unsigned int address);
void done ();

#endif
