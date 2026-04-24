#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/vt.h>      /* For VT_GETSTATE, VT_ACTIVATE */
#include <linux/kd.h>      /* For KDGETMODE, KDSETMODE, KD_TEXT, KD_GRAPHICS */
#include <termios.h>       /* For tcgetattr, tcsetattr */
#include <sys/select.h>    /* For select() - input timeout */

#define TTY_PATH "/dev/tty"

/* Get terminal size without ncurses */
static void get_terminal_size(int fd, int *rows, int *cols) {
    struct winsize ws;
    
    if (ioctl(fd, TIOCGWINSZ, &ws) == 0) {
        *rows = ws.ws_row;
        *cols = ws.ws_col;
    } else {
        /* Fallback to environment or defaults */
        char *r = getenv("LINES");
        char *c = getenv("COLUMNS");
        *rows = r ? atoi(r) : 24;
        *cols = c ? atoi(c) : 80;
    }
}

/* Get current virtual terminal number */
static int get_vt_number(int fd) {
    struct vt_stat vt;
    
    if (ioctl(fd, VT_GETSTATE, &vt) == 0) {
        return vt.v_active;
    }
    return -1;
}

/* Check if we're in text or graphics mode */
static int get_display_mode(int fd) {
    int mode;
    
    if (ioctl(fd, KDGETMODE, &mode) == 0) {
        return mode;
    }
    return -1;
}

/* Set console to text mode (restore after graphics) */
static int set_text_mode(int fd) {
    return ioctl(fd, KDSETMODE, KD_TEXT);
}

/* Set console to graphics mode (demonstration only) */
static int set_graphics_mode(int fd) {
    return ioctl(fd, KDSETMODE, KD_GRAPHICS);
}

/* Clear screen using ANSI escape codes (no external library) */
static void clear_screen(void) {
    write(STDOUT_FILENO, "\033[2J\033[H", 7);
}

/* Move cursor to position */
static void gotoxy(int x, int y) {
    char buf[16];
    int len = snprintf(buf, sizeof(buf), "\033[%d;%dH", y, x);
    write(STDOUT_FILENO, buf, len);
}

/* Wait for a key press with timeout (returns key or -1) */
static int wait_key(int timeout_ms) {
    fd_set set;
    struct timeval tv;
    char ch;
    int ret;
    
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    
    ret = select(STDIN_FILENO + 1, &set, NULL, NULL, &tv);
    if (ret > 0) {
        if (read(STDIN_FILENO, &ch, 1) == 1) {
            return (unsigned char)ch;
        }
    }
    return -1;
}

/* Make stdin non-blocking */
static void set_nonblocking(int enable) {
    struct termios t;
    
    tcgetattr(STDIN_FILENO, &t);
    
    if (enable) {
        t.c_lflag &= ~(ICANON | ECHO);
        t.c_cc[VMIN] = 0;
        t.c_cc[VTIME] = 0;
    } else {
        t.c_lflag |= (ICANON | ECHO);
        t.c_cc[VMIN] = 1;
        t.c_cc[VTIME] = 0;
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int main(int argc, char *argv[]) {
    int tty_fd;
    int rows, cols;
    int vt_num;
    int mode;
    int choice;
    int running = 1;
    char input[32];
    
    /* Open the current terminal */
    tty_fd = open(TTY_PATH, O_RDWR);
    if (tty_fd < 0) {
        perror("Failed to open terminal");
        return 1;
    }
    
    /* Get terminal information */
    get_terminal_size(tty_fd, &rows, &cols);
    vt_num = get_vt_number(tty_fd);
    mode = get_display_mode(tty_fd);
    
    set_nonblocking(1);
    clear_screen();
    
    while (running) {
        clear_screen();
        
        printf("=== Linux IOCTL Demo ===\n\n");
        printf("Terminal size: %d rows x %d cols\n", rows, cols);
        printf("Active VT: %d\n\n", vt_num);
        
        printf("1. Get terminal size (TIOCGWINSZ)\n");
        printf("2. Check display mode (KDGETMODE)\n");
        printf("3. Flash screen (KDGKBTYPE - keyboard type)\n");
        printf("4. Get VT state (VT_GETSTATE)\n");
        printf("5. Try graphics mode (KDSETMODE) - restore after\n");
        printf("6. Raw ioctl with custom request\n");
        printf("7. Quit\n\n");
        printf("Press a number key: ");
        
        fflush(stdout);
        
        choice = wait_key(5000); /* 5 second timeout */
        
        if (choice == '7' || choice == 'q' || choice == 'Q') {
            running = 0;
            clear_screen();
            gotoxy(1, 1);
            printf("Exiting...\n");
            break;
        }
        
        clear_screen();
        
        switch (choice) {
            case '1': {
                /* TIOCGWINSZ - Get terminal window size */
                struct winsize ws;
                printf("=== TIOCGWINSZ ===\n\n");
                
                if (ioctl(tty_fd, TIOCGWINSZ, &ws) == 0) {
                    printf("Rows:    %d\n", ws.ws_row);
                    printf("Columns: %d\n", ws.ws_col);
                    printf("X pixels: %d\n", ws.ws_xpixel);
                    printf("Y pixels: %d\n", ws.ws_ypixel);
                } else {
                    perror("TIOCGWINSZ failed");
                }
                break;
            }
            
            case '2': {
                /* KDGETMODE - Get current display mode */
                int kd_mode;
                printf("=== KDGETMODE ===\n\n");
                
                if (ioctl(tty_fd, KDGETMODE, &kd_mode) == 0) {
                    printf("Current display mode: ");
                    switch (kd_mode) {
                        case KD_TEXT:     printf("KD_TEXT (text mode)\n"); break;
                        case KD_GRAPHICS: printf("KD_GRAPHICS (graphics mode)\n"); break;
                        default:          printf("Unknown (%d)\n", kd_mode); break;
                    }
                } else {
                    perror("KDGETMODE failed");
                }
                break;
            }
            
            case '3': {
                /* KDGKBTYPE - Get keyboard type */
                char kb_type;
                printf("=== KDGKBTYPE ===\n\n");
                
                if (ioctl(tty_fd, KDGKBTYPE, &kb_type) == 0) {
                    printf("Keyboard type: ");
                    switch (kb_type) {
                        case KB_84:    printf("KB_84 (old)\n"); break;
                        case KB_101:   printf("KB_101 (standard)\n"); break;
                        case KB_OTHER: printf("KB_OTHER\n"); break;
                        default:       printf("Unknown (0x%02x)\n", (unsigned char)kb_type); break;
                    }
                } else {
                    perror("KDGKBTYPE failed");
                }
                break;
            }
            
            case '4': {
                /* VT_GETSTATE - Get VT state */
                struct vt_stat vs;
                printf("=== VT_GETSTATE ===\n\n");
                
                if (ioctl(tty_fd, VT_GETSTATE, &vs) == 0) {
                    printf("Active VT:    %u\n", vs.v_active);
                    printf("Signal mask:  0x%04x\n", vs.v_signal);
                    printf("State mask:   0x%04x\n", vs.v_state);
                } else {
                    perror("VT_GETSTATE failed");
                }
                break;
            }
            
            case '5': {
                /* KDSETMODE - Temporarily set graphics mode, then restore */
                int current_mode;
                printf("=== KDSETMODE Test ===\n\n");
                
                /* Get current mode first */
                if (ioctl(tty_fd, KDGETMODE, &current_mode) != 0) {
                    perror("Failed to get current mode");
                    break;
                }
                
                printf("Setting graphics mode (KD_GRAPHICS)...\n");
                printf("You won't see output until we restore text mode.\n");
                sleep(1);
                
                if (ioctl(tty_fd, KDSETMODE, KD_GRAPHICS) == 0) {
                    printf("Now in graphics mode. Restoring in 2 seconds...\n");
                    sleep(2);
                    
                    if (ioctl(tty_fd, KDSETMODE, current_mode) == 0) {
                        printf("Restored to original mode.\n");
                    } else {
                        perror("Failed to restore mode");
                    }
                } else {
                    perror("KDSETMODE to graphics failed");
                    /* Try restoring anyway */
                    ioctl(tty_fd, KDSETMODE, current_mode);
                }
                break;
            }
            
            case '6': {
                /* Raw ioctl - user enters a request number */
                unsigned long request;
                int value;
                
                printf("=== Raw IOCTL ===\n\n");
                printf("Enter ioctl request number (hex, e.g., 0x5413 for TIOCGWINSZ): ");
                fflush(stdout);
                
                set_nonblocking(0); /* Back to line-buffered for scanf */
                
                if (fgets(input, sizeof(input), stdin)) {
                    if (sscanf(input, "%lx", &request) == 1) {
                        printf("Calling ioctl(fd, 0x%04lx, &value)...\n", request);
                        value = 0;
                        
                        if (ioctl(tty_fd, request, &value) == 0) {
                            printf("Success! Return value: %d (0x%04x)\n", value, value);
                        } else {
                            perror("ioctl failed");
                        }
                    } else {
                        printf("Invalid input\n");
                    }
                }
                
                set_nonblocking(1); /* Back to non-blocking */
                break;
            }
            
            default:
                if (choice >= 0) {
                    printf("Invalid option: '%c' (0x%02x)\n", choice, choice);
                } else {
                    printf("Timeout - no input received\n");
                }
                break;
        }
        
        if (running) {
            printf("\nPress any key to continue...");
            fflush(stdout);
            wait_key(10000); /* Wait up to 10 seconds */
        }
    }
    
    /* Restore terminal settings */
    set_nonblocking(0);
    close(tty_fd);
    
    return 0;
}