#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main() {
    int fd;
    uint64 t;

    // 1. Mở file thiết bị
    fd = open("clock", O_RDONLY);
    if(fd < 0){
        printf("Error: cannot open /dev/clock\n");
        exit(1);
    }

    // 2. Đọc dữ liệu thô (8 bytes)
    if(read(fd, &t, sizeof(uint64)) != sizeof(uint64)){
        printf("Error reading clock\n");
        exit(1);
    }

    // 3. In ra màn hình dưới dạng số
    printf("System Uptime: %d ticks\n", (int)t);

    close(fd);
    exit(0);
}