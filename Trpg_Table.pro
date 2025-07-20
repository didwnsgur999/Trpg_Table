QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    mycore/Backend.cpp \
    mycore/Info.cpp \
    mycore/chathandler.cpp \
    ui/chatroomui.cpp \
    mycore/clientchat.cpp \
    mycore/customer.cpp \
    ui/lobbymainui.cpp \
    ui/loginui.cpp \
    mycore/main.cpp \
    ui/mainwindow.cpp \
    graphics/mygraphicsitem.cpp \
    graphics/mygraphicsview.cpp \
    mycore/product.cpp \
    ui/roomdisplayui.cpp \
    ui/roomlistui.cpp \
    ui/storeui.cpp

HEADERS += \
    mycore/Backend.h \
    mycore/Info.h \
    mycore/chathandler.h \
    ui/chatroomui.h \
    mycore/clientchat.h \
    mycore/customer.h \
    ui/lobbymainui.h \
    ui/loginui.h \
    ui/mainwindow.h \
    graphics/mygraphicsitem.h \
    graphics/mygraphicsview.h \
    mycore/product.h \
    ui/roomdisplayui.h \
    mycore/roomitem.h \
    ui/roomlistui.h \
    ui/storeui.h

FORMS += \
    ui/chatroomui.ui \
    ui/lobbymainui.ui \
    ui/loginui.ui \
    ui/mainwindow.ui \
    ui/roomdisplayui.ui \
    ui/roomlistui.ui \
    ui/storeui.ui

TRANSLATIONS += \
    Trpg_Table_ko_KR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES += \
    background/tavern.jpg \
    models/ARCHER.png
