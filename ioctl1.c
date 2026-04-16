#include <stdio.h>
#include <fcntl.h>      // open
#include <unistd.h>     // close
#include <sys/ioctl.h>  // ioctl
#include <errno.h>      // errno
#include <string.h>     // strerror

// Define your own IOCTL command number
// Usually, _IO, _IOW, _IOR, _IOWR macros are used, but since we want to avoid external headers,
// we'll define the command number manually.
// For demonstration, assume 0x1234 is our command.
#define MY_IOCTL_COMMAND 0x1234

// Data structure for ioctl data (if needed)
struct my_ioctl_data {
    int value;
    char message[256];
};

int main() {
    const char *device_path = "/dev/mydevice"; // Replace with your device path
    int fd = open(device_path, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Prepare data for ioctl
    struct my_ioctl_data data;
    memset(&data, 0, sizeof(data));
    data.value = 42; // Example value
    strncpy(data.message, "Hello, ioctl!", sizeof(data.message) - 1);

    // Perform ioctl call
    int ret = ioctl(fd, MY_IOCTL_COMMAND, &data);
    if (ret < 0) {
        perror("ioctl failed");
        close(fd);
        return 1;
    }

    // If the ioctl modifies data, print the result
    printf("ioctl succeeded. message: %s, value: %d", data.message, data.value);

    close(fd);
    return 0;
}
