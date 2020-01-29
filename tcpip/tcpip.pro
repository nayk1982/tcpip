QT       += core widgets gui network

include( $${PWD}/../../_nayk/nayk.pri )

QMAKE_CXXFLAGS += -Wno-deprecated-copy

# Sources and Headers ==========================================================

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Resources ====================================================================

win32:RC_FILE = main.rc

# Output dir ===================================================================

CONFIG(release, debug|release) {
    win32: DESTDIR = $${PWD}/../_distrib/win_$${QMAKE_HOST.arch}
    else: unix:!android: DESTDIR  = $${PWD}/../_distrib/linux_$${QMAKE_HOST.arch}/bin
}
