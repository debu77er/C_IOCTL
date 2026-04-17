#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>

// Define constants if not available
#ifndef TIOCGWINSZ
#define TIOCGWINSZ 0x5413
#endif

#ifndef TIOCSWINSZ
#define TIOCSWINSZ 0x5414
#endif

int main() {
    int fd;
    struct termios term_opts;

    // Open the terminal device
    fd = open("/dev/tty", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /dev/tty");
        return 1;
    }

    // Use ioctl to get terminal attributes
    if (ioctl(fd, TCGETS, &term_opts) == -1) {
        perror("ioctl TCGETS failed");
        close(fd);
        return 1;
    }

    printf("Original ECHO flag: %d", (term_opts.c_lflag & ECHO) != 0);

    // Toggle ECHO flag
    if (term_opts.c_lflag & ECHO)
        term_opts.c_lflag &= ~ECHO;  // Disable echo
    else
        term_opts.c_lflag |= ECHO;   // Enable echo

    // Apply the modified terminal attributes
    if (ioctl(fd, TCSETS, &term_opts) == -1) {
        perror("ioctl TCSETS failed");
        close(fd);
        return 1;
    }

    printf("Toggled ECHO flag. Type something, it will%s be echoed.\n", (term_opts.c_lflag & ECHO) ? "" : " not");

    // Read input from user
    char buffer[100];
    printf("Enter something: ");
    fflush(stdout);
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        printf("You typed: %s", buffer);
    }

    // Restore original terminal settings before exit
    // Get current settings
    if (ioctl(fd, TCGETS, &term_opts) == -1) {
        perror("ioctl TCGETS failed");
        close(fd);
        return 1;
    }

    // Re-enable ECHO if it was disabled
    term_opts.c_lflag |= ECHO;
    if (ioctl(fd, TCSETS, &term_opts) == -1) {
        perror("ioctl TCSETS restore failed");
        close(fd);
        return 1;
    }

    close(fd);
    return 0;
}
