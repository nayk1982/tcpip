#===============================================================================
#
# Projects Settings - nayk library
#
#===============================================================================

CONFIG  += c++14

DEFINES += QT_DEPRECATED_WARNINGS

lessThan( QT_MAJOR_VERSION, 6 ):lessThan( QT_MINOR_VERSION, 14 ) {
    QMAKE_CXXFLAGS += -Wno-deprecated-copy
}

# Includes =====================================================================

INCLUDEPATH *= \
    $${PWD}/include

# Sources and Headers ==========================================================

SOURCES *= \
    $${PWD}/sources/app_core.cpp \
    $${PWD}/sources/log.cpp \
    $${PWD}/sources/convert.cpp \
    $${PWD}/sources/file_sys.cpp \
    $${PWD}/sources/system_utils.cpp \
    $${PWD}/sources/geo.cpp \
    $${PWD}/sources/http_server.cpp \
    $${PWD}/sources/simple_uart.cpp \
    $${PWD}/sources/lpt_port.cpp \

HEADERS *= \
    $${PWD}/include/app_core.h \
    $${PWD}/include/log.h \
    $${PWD}/include/convert.h \
    $${PWD}/include/file_sys.h \
    $${PWD}/include/system_utils.h \
    $${PWD}/include/geo.h \
    $${PWD}/include/http_const.h \
    $${PWD}/include/http_server.h \
    $${PWD}/include/abstract_port.h \
    $${PWD}/include/simple_uart.h \
    $${PWD}/include/lpt_port.h

contains(QT, quick) | contains(QT, widgets) {

    SOURCES *= \
        $${PWD}/sources/gui_app_core.cpp

    HEADERS *= \
        $${PWD}/include/gui_app_core.h \
        $${PWD}/include/images_const.h
}

contains(QT, widgets) {

    SOURCES *= \
        $${PWD}/sources/gui_utils.cpp \
        $${PWD}/sources/dialog_log.cpp \
        $${PWD}/sources/ex_controls.cpp \
        $${PWD}/sources/graph.cpp \
        $${PWD}/sources/highlighter.cpp

    HEADERS *= \
        $${PWD}/include/gui_utils.h \
        $${PWD}/include/dialog_log.h \
        $${PWD}/include/ex_controls.h \
        $${PWD}/include/graph.h \
        $${PWD}/include/highlighter.h
}

contains(QT, quick) {

} else {

    SOURCES *= \
        $${PWD}/sources/console.cpp

    HEADERS *= \
        $${PWD}/include/console.h
}

contains(QT, network) {

    SOURCES *= \
        $${PWD}/sources/network_client.cpp \
        $${PWD}/sources/http_client.cpp \
        $${PWD}/sources/telegram.cpp

    HEADERS *= \
        $${PWD}/include/network_client.h \
        $${PWD}/include/http_client.h \
        $${PWD}/include/telegram.h
}

contains(QT, serialport) {

    SOURCES *= \
        $${PWD}/sources/com_port.cpp

    HEADERS *= \
        $${PWD}/include/com_port.h
}

contains(QT, sql) {

    SOURCES *= \
        $${PWD}/sources/database_client.cpp

    HEADERS *= \
        $${PWD}/include/database_client.h
}

contains(CONFIG, hardware) {

    SOURCES *= \
        $${PWD}/sources/hardware_utils.cpp

    HEADERS *= \
        $${PWD}/include/hardware_utils.h

    win32:LIBS += -lKernel32 -lPsapi
}

# Resources files ==============================================================

contains(QT, widgets) {

    RESOURCES *= \
        $${PWD}/resources/icons.qrc \
        $${PWD}/resources/led_images.qrc
}

contains(QT, quick) {

    RESOURCES *= \
        $${PWD}/qml/qml_resources.qrc

    # Additional import path used to resolve QML modules in Qt Creator's code model
    QML_IMPORT_PATH *= \
        $${PWD}/qml/

    # Additional import path used to resolve QML modules just for Qt Quick Designer
    QML_DESIGNER_IMPORT_PATH *= \
        $${PWD}/qml/
}

# Version and Build date =======================================================

win32: BUILD_DATE = '$(shell echo %DATE:~6,4%-%DATE:~3,2%-%DATE:~0,2%)'
else:  BUILD_DATE = '$(shell date +%Y-%m-%d)'

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_BUILD_DATE=\\\"$$BUILD_DATE\\\"

# Output dir ===================================================================

CONFIG(release, debug|release) {
    win32: DESTDIR = $${_PRO_FILE_PWD_}/../_distrib/win_$${QMAKE_HOST.arch}
    else: unix:!android: DESTDIR  = $${_PRO_FILE_PWD_}/../_distrib/linux_$${QMAKE_HOST.arch}/bin
}

CONFIG(debug, debug|release) {
    win32: DESTDIR = $${_PRO_FILE_PWD_}/../_distrib/win_$${QMAKE_HOST.arch}_debug
    else: unix:!android: DESTDIR  = $${_PRO_FILE_PWD_}/../_distrib/linux_$${QMAKE_HOST.arch}_debug/bin
}

# translations =================================================================

win32: TRANSLATIONS_DIR = $$absolute_path( $${DESTDIR}/translations )
else:  TRANSLATIONS_DIR = $$absolute_path( $${DESTDIR}/../translations )

!exists( $${TRANSLATIONS_DIR} ) {

    translations_dir.commands = $${QMAKE_MKDIR} $$shell_path( $${TRANSLATIONS_DIR} )

    QMAKE_EXTRA_TARGETS += \
        translations_dir

    PRE_TARGETDEPS += \
        translations_dir
}

TRANSLATIONS += \
    $${PWD}/resources/translations/nayk_common_ru.ts

nayk_common_tr.commands = lrelease $${PWD}/resources/translations/nayk_common_ru.ts -qm $${TRANSLATIONS_DIR}/nayk_common_ru.qm

POST_TARGETDEPS += \
    nayk_common_tr

QMAKE_EXTRA_TARGETS += \
    nayk_common_tr

contains(QT, widgets) {

    TRANSLATIONS += \
        $${PWD}/resources/translations/nayk_widget_ru.ts

    nayk_widget_tr.commands = lrelease $${PWD}/resources/translations/nayk_widget_ru.ts -qm $${TRANSLATIONS_DIR}/nayk_widget_ru.qm

    POST_TARGETDEPS += \
        nayk_widget_tr

    QMAKE_EXTRA_TARGETS += \
        nayk_widget_tr
}

contains(QT, quick) {

    TRANSLATIONS += \
        $${PWD}/resources/translations/nayk_qml_ru.ts

    nayk_qml_tr.commands = lrelease $${PWD}/resources/translations/nayk_qml_ru.ts -qm $${TRANSLATIONS_DIR}/nayk_qml_ru.qm

    POST_TARGETDEPS += \
        nayk_qml_tr

    QMAKE_EXTRA_TARGETS += \
        nayk_qml_tr
}
