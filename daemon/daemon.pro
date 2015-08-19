
# The name of your application
TARGET = harbour-smssyncd

CONFIG += console \
#    sailfishapp \
    link_pkgconfig \
    c++11

QT += core network dbus
PKGCONFIG += TelepathyQt5

HEADERS += \
    src/mqttdispatcher.h \
    src/smslistener.h

SOURCES += src/daemon.cpp \
    src/mqttdispatcher.cpp \
    src/smslistener.cpp

include(qmqtt/qmqtt.pri)
INCLUDEPATH += $$PWD/qmqtt

OTHER_FILES += \
    harbour-smssyncd.conf

INSTALLS += target conf
target.path = /usr/bin

conf.files = $${TARGET}.conf
conf.path = /etc/xdg/harbour-smssync
