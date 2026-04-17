#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>

#define DEVICE_FILE "/dev/mydevice" // Replace with your device file
#define MY_IOCTL_MAGIC 0xF1

// Define a custom ioctl command
#define MY_IOCTL_GET_VALUE _IOR(MY_IOCTL_MAGIC, 1, int)

int main() {
    int fd;
    int value;
    
    // Open the device file
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }
    
    // Perform ioctl to get some value from device
    if (ioctl(fd, MY_IOCTL_GET_VALUE, &value) == -1) {
        perror("ioctl failed");
        close(fd);
        return 1;
    }
    
    printf("Value from device: %d", value);
    
    close(fd);
    return 0;
}
