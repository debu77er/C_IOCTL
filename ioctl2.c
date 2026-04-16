#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <errno.h>
#include <string.h>

int main() {
    const char *device = "/dev/input/event0"; // Change this to your keyboard device
    int fd = open(device, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open input device");
        return 1;
    }

    printf("Listening for keyboard events on %s...\n", device);

    struct input_event ev;

    while (1) {
        ssize_t n = read(fd, &ev, sizeof(ev));
        if (n < (ssize_t)sizeof(ev)) {
            perror("Failed to read input event");
            break;
        }

        // We're interested in key events
        if (ev.type == EV_KEY) {
            const char *key_state = (ev.value) ? "Pressed" : "Released";

            // To get key name, we can print the key code
            printf("Key code: %u, State: %s", ev.code, key_state);
        }
    }

    close(fd);
    return 0;
}

/*      // ioctl() equivalent in Windows is DeviceIoControl()
        // Use a Windows compiler like MSVC. For example, with Visual Studio Developer Command Prompt:
        // ioctl() is a systemcall


       //   cl /W4 /D_UNICODE keyboard_listener.c user32.lib


#include <windows.h>
#include <stdio.h>

// Global variable to store the window handle
HWND hwnd = NULL;

// Forward declaration of the window procedure
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Entry point of the program
int main() {
    // Register window class
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "KeyboardListenerClass";

    if (!RegisterClass(&wc)) {
        printf("Failed to register window class");
        return 1;
    }

    // Create a message-only window
    hwnd = CreateWindow(wc.lpszClassName, "Keyboard Listener", 0,
                        0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);
    if (!hwnd) {
        printf("Failed to create window");
        return 1;
    }

    // Register for raw input from keyboard devices
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01; // Generic desktop controls
    rid.usUsage = 0x06;     // Keyboard
    rid.dwFlags = RIDEV_INPUTSINK; // Receive input even when not focused
    rid.hwndTarget = hwnd;

    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
        printf("Failed to register raw input device");
        return 1;
    }

    printf("Listening for keyboard events...\n");

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

// Window procedure to handle messages
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INPUT: {
            UINT dwSize = 0;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
            BYTE *rawData = malloc(dwSize);
            if (rawData == NULL) {
                printf("Memory allocation failed");
                break;
            }
            if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, rawData, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
                printf("GetRawInputData does not return expected size!\n");
            } else {
                RAWINPUT *raw = (RAWINPUT*)rawData;
                if (raw->header.dwType == RIM_TYPEKEYBOARD) {
                    RAWKEYBOARD rk = raw->data.keyboard;

                    // Key down
                    if (rk.Message == WM_KEYDOWN || rk.Message == WM_SYSKEYDOWN) {
                        printf("Key pressed: ScanCode=%u, VirtualKey=%u", rk.MakeCode, rk.VKey);
                    }
                    // Key up
                    else if (rk.Message == WM_KEYUP || rk.Message == WM_SYSKEYUP) {
                        printf("Key released: ScanCode=%u, VirtualKey=%u", rk.MakeCode, rk.VKey);
                    }
                }
            }
            free(rawData);
            break;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


*/