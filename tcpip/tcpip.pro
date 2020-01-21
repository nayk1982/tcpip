QT       += core widgets gui network

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

QMAKE_CXXFLAGS += -Wno-deprecated-copy

# Sources and Headers ==========================================================

INCLUDEPATH += \
    include \

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Resources ====================================================================

RESOURCES += \
    main.qrc

win32:RC_FILE = main.rc

# Version and Build date =======================================================

win32: BUILD_DATE = '$(shell echo %DATE:~6,4%-%DATE:~3,2%-%DATE:~0,2%)'
else:  BUILD_DATE = '$(shell date +%Y-%m-%d)'
VERSION  = 1.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_BUILD_DATE=\\\"$$BUILD_DATE\\\"

# Output dir ===================================================================

CONFIG(release, debug|release) {
    win32: DESTDIR = $${PWD}/../_distrib/win_$${QMAKE_HOST.arch}
    else: unix:!android: DESTDIR  = $${PWD}/../_distrib/linux_$${QMAKE_HOST.arch}/bin
}
