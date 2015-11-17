TARGET = mapcssfixup

CONFIG += console
CONFIG -= app_bundle
CONFIG -= QT
CONFIG += warn_on c++11

TEMPLATE = app

SOURCES += \
    main.cpp \
    mapcss_parse.cpp \
    common.cpp

HEADERS += \
    mapcss_parse.hpp \
    common.hpp
