
# The name of your application
TARGET = harbour-smssyncd

CONFIG += console \
#    sailfishapp \
    link_pkgconfig \
    c++11

QT += core network dbus
PKGCONFIG += \
    TelepathyQt5 \
    Qt5Contacts

HEADERS += \
    src/mqttdispatcher.h \
    src/smslistener.h \
    src/message.h \
    src/contact.h

SOURCES += src/daemon.cpp \
    src/mqttdispatcher.cpp \
    src/smslistener.cpp \
    src/message.cpp \
    src/contact.cpp

include(qmqtt/qmqtt.pri)
INCLUDEPATH += $$PWD/qmqtt

OTHER_FILES += \
    harbour-smssyncd.conf \
    harbour-smssyncd.service

INSTALLS += target conf systemd
target.path = /usr/bin

conf.files = $${TARGET}.conf
conf.path = /etc/xdg/harbour-smssync

systemd.files = $${TARGET}.service
systemd.path = /usr/lib/systemd/user
