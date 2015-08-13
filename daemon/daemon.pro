
# The name of your application
TARGET = harbour-smssyncd

CONFIG += console

QT += core dbus

SOURCES += src/daemon.cpp

INSTALLS += target
target.path = /usr/bin
