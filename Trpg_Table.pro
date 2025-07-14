QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Backend.cpp \
    Info.cpp \
    chathandler.cpp \
    chatroomui.cpp \
    clientchat.cpp \
    customer.cpp \
    lobbymainui.cpp \
    loginui.cpp \
    main.cpp \
    mainwindow.cpp \
    product.cpp \
    roomdisplayui.cpp \
    roomlistui.cpp \
    storeui.cpp

HEADERS += \
    Backend.h \
    Info.h \
    chathandler.h \
    chatroomui.h \
    clientchat.h \
    customer.h \
    lobbymainui.h \
    loginui.h \
    mainwindow.h \
    product.h \
    roomdisplayui.h \
    roomlistui.h \
    storeui.h

FORMS += \
    chatroomui.ui \
    lobbymainui.ui \
    loginui.ui \
    mainwindow.ui \
    roomdisplayui.ui \
    roomlistui.ui \
    storeui.ui

TRANSLATIONS += \
    Trpg_Table_ko_KR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
