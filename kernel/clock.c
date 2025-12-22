#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"

struct spinlock clock_lock;
uint64 virtual_time = 0; // The simulated clock time

void clockinit(void) {
  initlock(&clock_lock, "clock");
  // Register read/write functions for Major Device 2 (CLOCK)
  devsw[CLOCK].read = clockread;
  devsw[CLOCK].write = clockwrite;
}
// Timer Interrupt Hook: Call this from trap.c
void update_virtual_time(void) {  // <--- Changed name
  acquire(&clock_lock);
  virtual_time++; 
  release(&clock_lock);
}

// (2) Device Ops: Read returns current time
int clockread(int user_dst, uint64 dst, int n) {
  uint64 time_copy;

  acquire(&clock_lock);
  time_copy = virtual_time;
  release(&clock_lock);


  if(either_copyout(user_dst, dst, &time_copy, sizeof(uint64)) < 0)
    return -1;
  
  return sizeof(uint64);
}

// (2) Device Ops: Write sets the time (Programmable)
int clockwrite(int user_src, uint64 src, int n) {
  uint64 new_time;

  if(n < sizeof(uint64)) return -1;
  

  if(either_copyin(&new_time, user_src, src, sizeof(uint64)) < 0)
    return -1;

  acquire(&clock_lock);
  virtual_time = new_time;
  release(&clock_lock);

  return sizeof(uint64);
}