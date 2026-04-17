#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

// Main program
int main() {
    int fd;
    struct winsize ws;

    // Open the terminal device for reading and writing
    fd = open("/dev/tty", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /dev/tty");
        return 1;
    }

    // Use ioctl to get window size
    if (ioctl(fd, TIOCGWINSZ, &ws) == -1) {
        perror("ioctl TIOCGWINSZ failed");
        close(fd);
        return 1;
    }

    // Print the window size
    printf("Rows: %d, Columns: %d", ws.ws_row, ws.ws_col);

    // Close the device
    close(fd);
    return 0;
}
