#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

// Replace these with your actual device node and ioctl commands
#define USB_DEVICE_PATH "/dev/usb_sniffer"

// Define ioctl commands (these must match the driver definitions)
#define USB_SNIFFER_START _IO('U', 1)
#define USB_SNIFFER_STOP  _IO('U', 2)
#define USB_SNIFFER_GET_DATA _IOR('U', 3, struct usb_sniffer_data)

// Example data structure for received data
struct usb_sniffer_data {
    size_t length;
    unsigned char data[1024];  // Adjust size as needed
};

int main(int argc, char *argv[]) {
    int fd;
    int ret;
    struct usb_sniffer_data sniff_data;

    // Open the device
    fd = open(USB_DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Start sniffing
    ret = ioctl(fd, USB_SNIFFER_START);
    if (ret < 0) {
        perror("ioctl START failed");
        close(fd);
        return 1;
    }
    printf("Sniffing started.\n");

    // Loop to read data
    for (int i = 0; i < 10; ++i) { // Read 10 packets for example
        memset(&sniff_data, 0, sizeof(sniff_data));
        ret = ioctl(fd, USB_SNIFFER_GET_DATA, &sniff_data);
        if (ret < 0) {
            perror("ioctl GET_DATA failed");
            break;
        }
        printf("Received data (%zu bytes): ", sniff_data.length);
        for (size_t j = 0; j < sniff_data.length; ++j) {
            printf("%02X ", sniff_data.data[j]);
        }
        printf("\n");
        sleep(1); // Wait a bit before next read
    }

    // Stop sniffing
    ret = ioctl(fd, USB_SNIFFER_STOP);
    if (ret < 0) {
        perror("ioctl STOP failed");
    } else {
        printf("Sniffing stopped.\n");
    }

    close(fd);
    return 0;
}
