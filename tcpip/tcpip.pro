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

RESOURCES += \
    main.qrc

win32:RC_FILE = main.rc

# Output dir ===================================================================

CONFIG(release, debug|release) {
    win32: DESTDIR = $${PWD}/../_distrib/win_$${QMAKE_HOST.arch}
    else: unix:!android: DESTDIR  = $${PWD}/../_distrib/linux_$${QMAKE_HOST.arch}/bin
}

# translations =================================================================

#TRANSLATIONS += $${PWD}/translations/main_ru.ts
main_tr.commands = lrelease $${PWD}/translations/main_ru.ts -qm $${PWD}/translations/main_ru.qm
nayk_common_tr.commands = lrelease $${PWD}/../../_nayk/resources/translations/nayk_common_ru.ts -qm $${PWD}/translations/nayk_common_ru.qm
nayk_widget_tr.commands = lrelease $${PWD}/../../_nayk/resources/translations/nayk_widget_ru.ts -qm $${PWD}/translations/nayk_widget_ru.qm

PRE_TARGETDEPS += \
    main_tr \
    nayk_common_tr \
    nayk_widget_tr

QMAKE_EXTRA_TARGETS += \
    main_tr \
    nayk_common_tr \
    nayk_widget_tr
