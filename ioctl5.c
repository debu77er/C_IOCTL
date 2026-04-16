#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>

// Define your ioctl command here.
// Normally, these are generated via _IO, _IOR, _IOW, or _IOWR macros.
// For example, let's define a dummy command:
#define IOCTL_MY_COMMAND 0x1234

// Path to the device file
#define DEVICE_FILE "/dev/usbmydevice"

int main() {
    int fd;
    int ret;
    unsigned int command_data = 0; // Replace with actual data type and value as needed

    // Open the device file
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Send an ioctl command
    ret = ioctl(fd, IOCTL_MY_COMMAND, &command_data);
    if (ret < 0) {
        perror("ioctl failed");
        close(fd);
        return 1;
    }

    printf("ioctl command sent successfully.\n");

    // Close the device file
    close(fd);
    return 0;
}