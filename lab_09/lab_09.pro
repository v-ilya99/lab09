TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread

QMAKE_CFLAGS_RELEASE = -O2 -MT
QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO += -O2 -MT -Zi
QMAKE_CFLAGS_DEBUG = -Zi -MTd

INCLUDEPATH += D:\boost
LIBS += "-LD:\boost\stage\lib"

INCLUDEPATH += ..\gumbo-parser\src
win32:CONFIG(release, debug|release): LIBS += "-L..\gumbo-parser\visualc\Release" -lgumbo
else:win32:CONFIG(debug, debug|release): LIBS += "-L..\gumbo-parser\visualc\Debug" -lgumbo

INCLUDEPATH += ..\ThreadPool

INCLUDEPATH += ..\uriparser\include
win32:CONFIG(release, debug|release): LIBS += "-L..\uriparser\win32\Release" -luriparser
else:win32:CONFIG(debug, debug|release): LIBS += "-L..\uriparser\win32\Debug" -luriparser


SOURCES += \
        main.cpp \
    c_parser.cpp \
    c_uri.cpp \
    c_requester.cpp \
    c_queue.cpp

HEADERS += \
    c_parser.h \
    c_uri.h \
    c_requester.h \
    c_queue.h

