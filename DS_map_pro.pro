QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    mapview.cpp \
    searchengine.cpp \
    router.cpp \
    tilemanager.cpp

HEADERS += \
    mainwindow.h \
    mapview.h \
    searchengine.h \
    router.h \
    datatypes.h \
    tilemanager.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
