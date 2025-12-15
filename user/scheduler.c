#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    int fd;
    uint64 current_tick;
    uint64 start_tick;
    uint64 last_printed_tick;
    
    // 1. Open the virtual time device
    fd = open("clock", O_RDWR);
    if(fd < 0){
        printf("Error: cannot open /dev/clock\n");
        exit(1);
    }

    // 2. Get the starting time (Do NOT reset to 0)
    printf("Scheduler: Getting start time...\n");
    if(read(fd, &start_tick, sizeof(uint64)) < 0){
        printf("Error reading clock\n");
        exit(1);
    }
    
    printf("Scheduler: Starting at tick %d.\n", (int)start_tick);
    printf("Scheduler: Tasks rescheduled. Waiting for ticks...\n");

    last_printed_tick = start_tick;

    // 3. Loop until we decide to stop (e.g., after 20 ticks)
    while(1) {
        // Read the clock
        if(read(fd, &current_tick, sizeof(uint64)) < 0){
            printf("Error reading clock\n");
            break;
        }

        // Only process if the time has actually moved forward
        if (current_tick > last_printed_tick) {
            
            printf("Scheduler: Got tick value: %d\n", (int)current_tick);

            // LOGIC FOR ANIMATION A:
            // Starts at Start+5, repeats every 7 ticks (Offsets: 5, 12, 19...)
            if ((current_tick > start_tick) && ((current_tick - start_tick - 5) % 7 == 0)) {
                 printf("[Tick %d] --- Animation A Frame ---\n", (int)current_tick);
            }

            // LOGIC FOR ANIMATION B:
            // Starts at Start+10, repeats every 5 ticks (Offsets: 10, 15, 20...)
            if ((current_tick > start_tick) && (current_tick >= start_tick + 10) && 
                ((current_tick - start_tick - 10) % 5 == 0)) {
                 printf("[Tick %d] *** Animation B Frame ***\n", (int)current_tick);
            }

            // Update tracker
            last_printed_tick = current_tick;

            // Stop condition: Stop after 20 ticks from start
            if (current_tick >= start_tick + 20) {
                printf("Scheduler: All tasks complete. Exiting.\n");
                break;
            }
        }
    }

    close(fd);
    exit(0);
}