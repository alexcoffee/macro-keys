#include <vector>
#include <iostream>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

void printWindowDetails(Display *display, Window window) {
    char *windowName = nullptr;
    XClassHint classHint;

    if (XFetchName(display, window, &windowName)) {
        std::cout << "\nWindow Name: " << windowName << std::endl;
        XFree(windowName);
    }

    if (XGetClassHint(display, window, &classHint)) {
        std::cout << "Application Name: " << classHint.res_name << std::endl;
        std::cout << "Application Class: " << classHint.res_class << std::endl;
        XFree(classHint.res_name);
        XFree(classHint.res_class);
    }

    XWindowAttributes attributes;
    if (XGetWindowAttributes(display, window, &attributes)) {
        std::cout << "Position: (" << attributes.x << ", " << attributes.y << ")" << std::endl;
        std::cout << "Size: " << attributes.width << "x" << attributes.height << std::endl;
    }

    Atom typeAtom = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom actualType;
    int actualFormat;
    unsigned long numItems, bytesAfter;
    unsigned char *data = nullptr;

    if (XGetWindowProperty(display, window, typeAtom, 0, 1, False, AnyPropertyType, &actualType, &actualFormat, &numItems, &bytesAfter, &data) == Success) {
        if (data != nullptr) {
            Atom *atoms = reinterpret_cast<Atom *>(data);
            char *typeName = XGetAtomName(display, atoms[0]);
            std::cout << "Window Type: " << typeName << std::endl;
            XFree(typeName);
            XFree(data);
        }
    }
}

bool getWindowPid(Display *display, Window window, pid_t &pid) {
    Atom pidAtom = XInternAtom(display, "_NET_WM_PID", True);

    if (pidAtom == None) {
        return false;
    }

    Atom actualType;
    int actualFormat;
    unsigned long numItems, bytesAfter;
    unsigned char *propPID = nullptr;

    if (XGetWindowProperty(display, window, pidAtom, 0, 1, False, XA_CARDINAL, &actualType, &actualFormat, &numItems, &bytesAfter, &propPID) != Success) {
        return false;
    }

    if (actualType == XA_CARDINAL && actualFormat == 32 && numItems == 1) {
        pid = *reinterpret_cast<pid_t *>(propPID);
        XFree(propPID);
        return true;
    }

    if (propPID) {
        XFree(propPID);
    }

    return false;
}

void findWindowsByPid(Display *display, Window root, pid_t targetPid, std::vector<Window> &result) {
    Window parent;
    Window *children;
    unsigned int numChildren;

    if (XQueryTree(display, root, &root, &parent, &children, &numChildren)) {
        for (unsigned int i = 0; i < numChildren; ++i) {
            pid_t pid;

            if (getWindowPid(display, children[i], pid) && pid == targetPid) {
                result.push_back(children[i]);
            }

            // Recursively search in child windows
            findWindowsByPid(display, children[i], targetPid, result);
        }

        if (children) {
            XFree(children);
        }
    }
}

bool isMainWindow(Display *display, Window window) {
    XWindowAttributes attr;

    if (!XGetWindowAttributes(display, window, &attr)) {
        return false;
    }

    if (attr.map_state != IsViewable) {
        return false;
    }


    if (attr.width < 200 || attr.height < 200) {
        return false;
    }

    Atom typeAtom = XInternAtom(display, "_NET_WM_WINDOW_TYPE", False);
    Atom actualType;
    int actualFormat;
    unsigned long numItems, bytesAfter;
    unsigned char *data = nullptr;

    if (XGetWindowProperty(display, window, typeAtom, 0, 1, False, AnyPropertyType, &actualType, &actualFormat, &numItems, &bytesAfter, &data) != Success) {
        return false;
    }

    if (!data) {
        return false;
    }

    Atom *type = reinterpret_cast<Atom *>(data);
    Atom normalAtom = XInternAtom(display, "_NET_WM_WINDOW_TYPE_NORMAL", False);
    Atom customAtom = XInternAtom(display, "_KDE_NET_WM_WINDOW_TYPE_OVERRIDE", False);

    bool isMain = (*type == normalAtom) || (*type == customAtom);
    XFree(data);

    return isMain;
}

Window findMainWindowForPid(Display *display, pid_t targetPid) {
    Window root = DefaultRootWindow(display);
    std::vector<Window> windows;
    findWindowsByPid(display, root, targetPid, windows);

    for (Window w: windows) {
        printWindowDetails(display, w);
        if (isMainWindow(display, w)) {
            return w;
        }
    }

    return 0;
}