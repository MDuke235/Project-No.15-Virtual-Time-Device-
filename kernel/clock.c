// kernel/clock.c
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

  // 1. user_dst   = Is the destination in user space?
  // 2. dst        = User destination address
  // 3. &time_copy = Kernel source address
  // 4. sizeof...  = Length
  if(either_copyout(user_dst, dst, &time_copy, sizeof(uint64)) < 0)
    return -1;
  
  return sizeof(uint64);
}

// (2) Device Ops: Write sets the time (Programmable)
int clockwrite(int user_src, uint64 src, int n) {
  uint64 new_time;

  if(n < sizeof(uint64)) return -1;
  
  // CORRECTED CALL:
  // 1. &new_time = Kernel destination address
  // 2. user_src   = Is the source in user space? (1 yes, 0 no)
  // 3. src        = User source address
  // 4. sizeof...  = Length
  if(either_copyin(&new_time, user_src, src, sizeof(uint64)) < 0)
    return -1;

  acquire(&clock_lock);
  virtual_time = new_time;
  release(&clock_lock);

  return sizeof(uint64);
}