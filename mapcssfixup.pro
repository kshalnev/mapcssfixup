TARGET = mapcssfixup

CONFIG += console
CONFIG -= app_bundle
CONFIG += warn_on c++11

TEMPLATE = app

SOURCES += \
    main.cpp \
    mapcss_parse.cpp \
    common.cpp

HEADERS += \
    mapcss_parse.h \
    common.hpp
