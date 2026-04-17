#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

// Define custom ioctl commands
#define DEVICE_FILE "/dev/mydevice"

// Magic number for ioctl commands
#define MYDEVICE_MAGIC 'k'

// Command numbers
#define MYDEVICE_GET_STATUS _IOR(MYDEVICE_MAGIC, 1, struct device_status)
#define MYDEVICE_SET_CONFIG _IOW(MYDEVICE_MAGIC, 2, struct device_config)
#define MYDEVICE_RESET _IO(MYDEVICE_MAGIC, 3)
#define MYDEVICE_READ_DATA _IOR(MYDEVICE_MAGIC, 4, char[256])
#define MYDEVICE_WRITE_DATA _IOW(MYDEVICE_MAGIC, 5, char[256])

// Structures for ioctl data exchange

// Device status structure
struct device_status {
    int power_on;       // 1 if device is powered on, 0 otherwise
    int error_code;     // Last error code
    char message[64];   // Status message
};

// Device configuration structure
struct device_config {
    int mode;           // Operational mode
    int speed;          // Speed setting
    int threshold;      // Threshold value
};

// Function to check if device file exists
int check_device_exists(const char *device_path) {
    struct stat st;
    return (stat(device_path, &st) == 0);
}

// Function to perform ioctl call with error handling
int perform_ioctl(int fd, unsigned long request, void *arg) {
    int ret = ioctl(fd, request, arg);
    if (ret == -1) {
        perror("ioctl failed");
    }
    return ret;
}

int main() {
    int fd;

    // Check if device file exists
    if (!check_device_exists(DEVICE_FILE)) {
        fprintf(stderr, "Device file %s does not exist.\n", DEVICE_FILE);
        return 1;
    }

    // Open device file
    fd = open(DEVICE_FILE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }
    printf("Device %s opened successfully.\n", DEVICE_FILE);

    // 1. Get device status
    struct device_status status;
    printf("Getting device status...\n");
    if (perform_ioctl(fd, MYDEVICE_GET_STATUS, &status) == 0) {
        printf("Device Status:\n");
        printf("  Power On: %s", status.power_on ? "Yes" : "No");
        printf("  Error Code: %d", status.error_code);
        printf("  Message: %s", status.message);
    }

    // 2. Set device configuration
    struct device_config config;
    config.mode = 2;       // Example mode
    config.speed = 1000;   // Example speed
    config.threshold = 50; // Example threshold

    printf("Setting device configuration...\n");
    if (perform_ioctl(fd, MYDEVICE_SET_CONFIG, &config) == 0) {
        printf("Configuration set successfully.\n");
    }

    // 3. Read data from device
    char data_buffer[256] = {0};
    printf("Reading data from device...\n");
    if (perform_ioctl(fd, MYDEVICE_READ_DATA, data_buffer) == 0) {
        printf("Data received: %s", data_buffer);
    }

    // 4. Write data to device
    const char *write_message = "Hello, device!";
    strncpy(data_buffer, write_message, sizeof(data_buffer) - 1);
    printf("Writing data to device: %s", data_buffer);
    if (perform_ioctl(fd, MYDEVICE_WRITE_DATA, data_buffer) == 0) {
        printf("Data written successfully.\n");
    }

    // 5. Reset device
    printf("Resetting device...\n");
    if (perform_ioctl(fd, MYDEVICE_RESET, NULL) == 0) {
        printf("Device reset successfully.\n");
    }

    // Close device
    close(fd);
    printf("Device closed.\n");
    return 0;
}
