#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <errno.h>

#define DEVICE_PATH "/dev/input/event0"  // Change as needed

// Function to list input devices and find keyboard device
void list_input_devices() {
    int fd;
    char name[256] = "Unknown";

    printf("Listing input devices:\n");
    for (int i = 0; i < 32; i++) {
        char dev_path[64];
        snprintf(dev_path, sizeof(dev_path), "/dev/input/event%d", i);
        fd = open(dev_path, O_RDONLY);
        if (fd == -1) {
            continue; // Cannot open device, skip
        }

        // Get device name
        if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0) {
            printf("Device %s: %s", dev_path, name);
            // Check if device is a keyboard
            unsigned long evbit[EV_MAX / sizeof(long) + 1];
            memset(evbit, 0, sizeof(evbit));
            if (ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), evbit) >= 0) {
                if (evbit[EV_KEY / sizeof(long)] & (1 << (KEY_A / sizeof(long)))) {
                    printf("Possible keyboard device: %s", dev_path);
                }
            }
        }
        close(fd);
    }
}

// Function to open a device and return file descriptor
int open_input_device(const char *device_path) {
    int fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        perror("Failed to open device");
        exit(EXIT_FAILURE);
    }
    return fd;
}

// Function to check if device is a keyboard
int is_keyboard_device(int fd) {
    unsigned long evbit[(EV_MAX / sizeof(long)) + 1];
    memset(evbit, 0, sizeof(evbit));
    if (ioctl(fd, EVIOCGBIT(0, sizeof(evbit)), evbit) < 0) {
        perror("EVIOCGBIT");
        return 0;
    }
    // Check if device supports EV_KEY
    if (!(evbit[EV_KEY / sizeof(long)] & (1 << (KEY_A / sizeof(long))))) {
        return 0; // Not a keyboard
    }

    // Additional checks can be added here
    return 1;
}

// Main function to read and process keyboard events
int main(int argc, char *argv[]) {
    const char *device_path = DEVICE_PATH;

    // Optional: list devices to identify keyboard device
    list_input_devices();

    // Allow user to specify device path
    if (argc > 1) {
        device_path = argv[1];
    }

    int fd = open_input_device(device_path);
    if (!is_keyboard_device(fd)) {
        fprintf(stderr, "Device %s does not appear to be a keyboard.\n", device_path);
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening for keyboard events on %s. Press Ctrl+C to exit.\n", device_path);

    struct input_event ev;
    ssize_t n;

    while (1) {
        n = read(fd, &ev, sizeof(ev));
        if (n == -1) {
            perror("read");
            break;
        } else if (n != sizeof(ev)) {
            fprintf(stderr, "Incomplete read");
            continue;
        }

        // Process key events
        if (ev.type == EV_KEY) {
            const char *state_str = (ev.value == 0) ? "RELEASED" :
                                    (ev.value == 1) ? "PRESSED" :
                                    (ev.value == 2) ? "HELD" : "UNKNOWN";

            printf("Key 0x%02x (%d) %s at time %ld.%06ld",
                   ev.code, ev.code, state_str,
                   ev.time.tv_sec, ev.time.tv_usec);
        }
    }

    close(fd);
    return 0;
}
