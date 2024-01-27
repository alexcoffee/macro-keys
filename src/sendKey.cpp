#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>

void sendKey(Display *display, Window window, int key) {
    // Focus the target window
    XWindowAttributes originalAttributes;
    XGetWindowAttributes(display, window, &originalAttributes); // Get attributes to check visibility

    Window originalFocus;
    int revertTo;
    XGetInputFocus(display, &originalFocus, &revertTo); // Get original focused window

    if (originalAttributes.map_state == IsViewable) {
        XSetInputFocus(display, window, RevertToParent, CurrentTime);
        XFlush(display);
    }

    // Get keycodes for CTRL and A keys
    KeyCode keycodeCtrl = XKeysymToKeycode(display, XK_Control_L);
    KeyCode keycode = XKeysymToKeycode(display, key);

    // Simulate CTRL+A keystroke
    XTestFakeKeyEvent(display, keycodeCtrl, True, 0);
    XTestFakeKeyEvent(display, keycode, True, 0);
    XTestFakeKeyEvent(display, keycode, False, 0);
    XTestFakeKeyEvent(display, keycodeCtrl, False, 0);

    XFlush(display);

    // Restore original focus
    XSetInputFocus(display, originalFocus, revertTo, CurrentTime);
}
