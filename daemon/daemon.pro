
# The name of your application
TARGET = harbour-smssyncd

CONFIG += console \
#    sailfishapp \
    link_pkgconfig

QT += core network

HEADERS += \
    src/mqttdispatcher.h

SOURCES += src/daemon.cpp \
    src/mqttdispatcher.cpp

include(qmqtt/qmqtt.pri)
INCLUDEPATH += $$PWD/qmqtt

OTHER_FILES += \
    harbour-smssyncd.conf

INSTALLS += target conf
target.path = /usr/bin

conf.files = $${TARGET}.conf
conf.path = /etc/xdg/harbour-smssync
