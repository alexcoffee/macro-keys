#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <libevdev/libevdev.h>
#include <csignal>
#include <string>
#include <regex>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "findPid.h"
#include "findMainWindowForPid.h"
#include "findTargetOfMatchingSymlink.h"
#include "sendKey.h"


volatile bool isRunning = true;

void signalHandler(int signum) {
    std::cout << "Received signal " << signum << "\n";
    isRunning = false;
}

int main(int argc, char *argv[]) {
    int fd;
    int rc;
    libevdev *dev = nullptr;
    struct input_event ev{};
    bool ctrlPressed = false;

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    pid_t targetPid = findPid((argc > 1) ? argv[1] : "WebullDesktop");

    if (!targetPid) {
        return 1;
    }

    Display *display = XOpenDisplay(nullptr);
    if (!display) {
        return 1;
    }

    // find target window
    Window mainWindow = findMainWindowForPid(display, targetPid);

    if (!mainWindow) {
        std::cout << "Target program window not found." << std::endl;
        return 1;
    }

    // find HID input device
    std::string device = findTargetOfMatchingSymlink("/dev/input/by-id", std::regex("usb-Raspberry_Pi_Pico_.*-event-kbd"));

    if (device.empty()) {
        std::cerr << "Could not find Raspberry Pi Pico HID device." << std::endl;
        return 1;
    }

    // open HID input device
    fd = open(device.c_str(), O_RDONLY | O_NONBLOCK); // async polling to allow clean up

    rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        std::cerr << "Failed to initialize libevdev (" << strerror(-rc) << ")" << std::endl;
        return 1;
    }

    rc = libevdev_grab(dev, LIBEVDEV_GRAB);
    if (rc < 0) {
        std::cerr << "Failed to grab libevdev (" << strerror(-rc) << ")" << std::endl;
        return 1;
    }

    while (isRunning) {
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

        if (rc == 0) {
            if (ev.type == EV_KEY) {
                if (ev.code == KEY_LEFTCTRL || ev.code == KEY_RIGHTCTRL) {
                    ctrlPressed = ev.value; // value is 1 if key is pressed, 0 if released
                }
                if (ctrlPressed && ev.code == KEY_A && ev.value == 1) {
                    sendKey(display, mainWindow, XK_A);
                    std::cout << "CTRL+A Pressed" << std::endl;
                }
                if (ctrlPressed && ev.code == KEY_B && ev.value == 1) {
                    sendKey(display, mainWindow, XK_B);
                    std::cout << "CTRL+B Pressed" << std::endl;
                }
            }
        } else if (rc == -EAGAIN) {
            usleep(1000); // Short sleep to prevent busy-waiting
        } else {
            std::cerr << "next_event returned error " << rc << std::endl;
        }
    }

    // cleanup
    XCloseDisplay(display);
    libevdev_grab(dev, LIBEVDEV_UNGRAB);
    libevdev_free(dev);
    close(fd);

    return 0;
}
