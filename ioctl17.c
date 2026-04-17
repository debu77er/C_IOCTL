#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <string.h>
#include <stdlib.h>

// Function to print framebuffer info
void print_fb_info(struct fb_var_screeninfo vinfo, struct fb_fix_screeninfo finfo) {
    printf("Resolution: %dx%d", vinfo.xres, vinfo.yres);
    printf("Bits Per Pixel: %d", vinfo.bits_per_pixel);
    printf("Line Length: %d bytes", finfo.line_length);
    printf("Framebuffer Length: %ld bytes", finfo.smem_len);
}

// Function to fill the screen with a color
void fill_screen(char *fbp, struct fb_var_screeninfo vinfo, unsigned int color) {
    int x, y;
    for (y = 0; y < vinfo.yres; y++) {
        for (x = 0; x < vinfo.xres; x++) {
            long location = (x + y * vinfo.xres) * vinfo.bits_per_pixel / 8;
            // Assuming 32bpp (ARGB)
            *((unsigned int *)(fbp + location)) = color;
        }
    }
}

int main() {
    const char *fb_path = "/dev/fb0";
    int fd = open(fb_path, O_RDWR);
    if (fd == -1) {
        perror("Error opening framebuffer device");
        return 1;
    }

    // Retrieve fixed screen info
    struct fb_fix_screeninfo finfo;
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed screen info");
        close(fd);
        return 1;
    }

    // Retrieve variable screen info
    struct fb_var_screeninfo vinfo;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable screen info");
        close(fd);
        return 1;
    }

    print_fb_info(vinfo, finfo);

    // Map framebuffer to memory
    long screensize = finfo.smem_len;
    char *fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fbp == MAP_FAILED) {
        perror("Failed to mmap framebuffer");
        close(fd);
        return 1;
    }

    // Example: Fill screen with black
    fill_screen(fbp, vinfo, 0x00000000);
    sleep(1);

    // Example: Fill screen with red
    fill_screen(fbp, vinfo, 0x00FF0000);
    sleep(1);

    // Example: Fill screen with green
    fill_screen(fbp, vinfo, 0x0000FF00);
    sleep(1);

    // Example: Fill screen with blue
    fill_screen(fbp, vinfo, 0x000000FF);
    sleep(1);

    // Cleanup
    munmap(fbp, screensize);
    close(fd);
    return 0;
}
