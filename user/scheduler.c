#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    int fd;
    uint64 current_tick;
    uint64 start_tick;
    uint64 last_printed_tick;
    uint64 reset_value = 0; // Giá trị để reset đồng hồ

    // 1. Open the virtual time device
    fd = open("clock", O_RDWR);
    if(fd < 0){
        printf("Error: cannot open /dev/clock\n");
        exit(1);
    }

    // --- THÊM PHẦN NÀY ĐỂ SỬ DỤNG CLOCKWRITE ---
    // 2. Programmable: Reset clock to 0 to start fresh
    printf("Scheduler: Resetting virtual clock to 0...\n");
    if(write(fd, &reset_value, sizeof(uint64)) != sizeof(uint64)){
        printf("Error writing to clock\n");
        exit(1);
    }
    // -------------------------------------------

    // 3. Get the starting time (Should be 0 or close to 0 now)
    if(read(fd, &start_tick, sizeof(uint64)) < 0){
        printf("Error reading clock\n");
        exit(1);
    }
    
    printf("Scheduler: Starting at tick %d.\n", (int)start_tick);
    printf("Scheduler: Tasks rescheduled. Waiting for ticks...\n");

    last_printed_tick = start_tick;

    // 4. Loop until we decide to stop
    while(1) {
        // Read the clock
        if(read(fd, &current_tick, sizeof(uint64)) < 0){
            printf("Error reading clock\n");
            break;
        }

        // Only process if the time has actually moved forward
        if (current_tick > last_printed_tick) {
            
            // In ra mỗi tick để dễ theo dõi (tùy chọn)
            // printf("Scheduler: Got tick value: %d\n", (int)current_tick);

            // LOGIC FOR ANIMATION A:
            // Starts at Start+5, repeats every 7 ticks
            if ((current_tick > start_tick) && ((current_tick - start_tick - 5) % 7 == 0)) {
                 printf("[Tick %d] --- Animation A Frame ---\n", (int)current_tick);
            }

            // LOGIC FOR ANIMATION B:
            // Starts at Start+10, repeats every 5 ticks
            if ((current_tick > start_tick) && (current_tick >= start_tick + 10) && 
                ((current_tick - start_tick - 10) % 5 == 0)) {
                 printf("[Tick %d] *** Animation B Frame ***\n", (int)current_tick);
            }

            // Update tracker
            last_printed_tick = current_tick;

            // Stop condition: Stop after 50 ticks (tăng lên để xem được nhiều loop hơn)
            if (current_tick >= start_tick + 50) {
                printf("Scheduler: Limit reached. Exiting.\n");
                break;
            }
        }
    }

    close(fd);
    exit(0);
}
