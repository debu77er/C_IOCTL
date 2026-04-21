#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Define a custom ioctl command
// Usually, these are defined in a header shared between kernel and user space
// Here, we create a unique command number using _IOW, _IOR, or _IO
#define MY_IOCTL_MAGIC 'k'
#define MY_IOCTL_CMD _IOW(MY_IOCTL_MAGIC, 1, int)

int main() {
    int fd;
    int value_to_set = 42;
    int result;

    // Open the device file (replace "/dev/mydevice" with your device)
    fd = open("/dev/tty0", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // Send ioctl command to set some value in the driver
    result = ioctl(fd, MY_IOCTL_CMD, &value_to_set);
    if (result < 0) {
        perror("ioctl failed");
        close(fd);
        return EXIT_FAILURE;
    }

    printf("ioctl command sent successfully.\n");

    // Close the device
    close(fd);
    return EXIT_SUCCESS;
}
