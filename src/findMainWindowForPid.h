#ifndef STOCK_KEYS_FINDMAINWINDOWFORPID_H
#define STOCK_KEYS_FINDMAINWINDOWFORPID_H

#include <X11/Xlib.h>

Window findMainWindowForPid(Display *display, pid_t targetPid);

#endif //STOCK_KEYS_FINDMAINWINDOWFORPID_H
