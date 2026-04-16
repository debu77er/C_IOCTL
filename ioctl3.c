#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <string.h>
#include <errno.h>

//write ioctl c linux mouse app without any external library

int main() {
    const char *device = "/dev/input/mice"; // or "/dev/input/eventX"
    int fd = open(device, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open input device");
        return 1;
    }

    // Optionally, get device info
    struct input_id id;
    if (ioctl(fd, EVIOCGID, &id) == -1) {
        perror("ioctl EVIOCGID");
        // Not fatal, continue
    } else {
        printf("Device ID: bustype=%u vendor=0x%04x product=0x%04x version=0x%04x",
               id.bustype, id.vendor, id.product, id.version);
    }

    printf("Listening for mouse events. Press Ctrl+C to exit.\n");

    unsigned char data[3]; // For relative movement: buttons + x + y

    while (1) {
        ssize_t n = read(fd, data, sizeof(data));
        if (n == -1) {
            perror("read");
            break;
        } else if (n != sizeof(data)) {
            fprintf(stderr, "Unexpected read size: %zd", n);
            continue;
        }

        // Parse button states
        int buttons = data[0];
        int left_button = buttons & 0x1;
        int right_button = (buttons & 0x2) >> 1;
        int middle_button = (buttons & 0x4) >> 2;

        // Movement
        int dx = (char)data[1]; // signed
        int dy = (char)data[2]; // signed

        printf("Buttons: [L:%d M:%d R:%d] | Movement: dx=%d, dy=%d",
               left_button, middle_button, right_button, dx, dy);
    }

    close(fd);
    return 0;
}
