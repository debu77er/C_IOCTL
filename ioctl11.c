#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Define a struct for terminal window size if not already available
// Usually, winsize is defined in <sys/ioctl.h> on Linux
#include <sys/ioctl.h>

int main() {
    int fd;
    struct winsize ws;

    // Open the terminal device for reading and writing
    fd = open("/dev/tty", O_RDWR);
    if (fd == -1) {
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
    printf("Rows: %d", ws.ws_row);
    printf("Columns: %d", ws.ws_col);

    close(fd);
    return 0;
}
