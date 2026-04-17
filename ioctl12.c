#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>

// Define a magic number for our ioctl commands
#define MY_IOCTL_MAGIC 'k'

// Define a command number
#define MY_IOCTL_GET_VALUE _IOR(MY_IOCTL_MAGIC, 1, int)

// Path to your device file (must exist and be accessible)
#define DEVICE_FILE "/dev/mydevice"

// Main function
int main() {
    int fd;
    int value;

    // Open the device file
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Call ioctl to get a value from device
    if (ioctl(fd, MY_IOCTL_GET_VALUE, &value) == -1) {
        perror("ioctl failed");
        close(fd);
        return 1;
    }

    printf("Received value from device: %d", value);

    close(fd);
    return 0;
}
