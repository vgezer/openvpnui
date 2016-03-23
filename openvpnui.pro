QT += widgets

HEADERS     = \
    configparser.h \
    vpngui.h \
    defines.h
SOURCES     = \
              main.cpp \
    configparser.cpp \
    vpngui.cpp

CONFIG += c++11
win32:RC_ICONS += res/openvpn-gui.ico
# install
