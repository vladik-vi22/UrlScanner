TEMPLATE = app

CONFIG += c++14
CONFIG += CONSOLE

QT += qml quick widgets
QT += network

HEADERS += \
    incl/url_scanner.hpp

SOURCES += src/main.cpp \
    src/url_scanner.cpp

RESOURCES += qml.qrc

