QT       += core gui widgets
#greaterThan(QT_MAJOR_VERSION, 4): QT +=

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    base.cpp \
    block.cpp \
    conveyer.cpp \
    cutter.cpp \
    dustbin.cpp \
    gamescene.cpp \
    harvestor.cpp \
    main.cpp \
    mine.cpp \
    store.cpp \
    widget.cpp

HEADERS += \
    base.h \
    block.h \
    conveyer.h \
    cutter.h \
    dustbin.h \
    gamescene.h \
    harvestor.h \
    mine.h \
    store.h \
    widget.h

FORMS += \
    gamescene.ui \
    store.ui \
    widget.ui

TRANSLATIONS += \
    myShapez_zh_TW.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
