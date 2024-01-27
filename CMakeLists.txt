cmake_minimum_required(VERSION 3.22)
project(stock_keys)

set(CMAKE_CXX_STANDARD 17)

find_package(PkgConfig REQUIRED)
find_package(X11 REQUIRED)
pkg_check_modules(LIBEVDEV REQUIRED libevdev)

add_executable(stock_keys
        src/main.cpp
        src/findPid.cpp
        src/findMainWindowForPid.cpp
        src/findTargetOfMatchingSymlink.cpp
        src/sendKey.cpp
        src/findPid.h
        src/findMainWindowForPid.h
        src/findTargetOfMatchingSymlink.h
        src/sendKey.h
)

target_include_directories(stock_keys PRIVATE ${LIBEVDEV_INCLUDE_DIRS})
target_include_directories(stock_keys PRIVATE ${X11_INCLUDE_DIR})
target_include_directories(stock_keys PRIVATE ${X11_Xtst_INCLUDE_PATH})

target_link_libraries(stock_keys ${X11_LIBRARIES} ${X11_Xtst_LIB} ${LIBEVDEV_LIBRARIES})
target_compile_options(stock_keys PRIVATE ${LIBEVDEV_CFLAGS_OTHER})
