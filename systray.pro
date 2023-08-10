HEADERS       = window.h \
    SingleApplication.h
SOURCES       = main.cpp \
                SingleApplication.cpp \
                window.cpp
RC_ICONS = icon1.ico
RESOURCES     = systray.qrc

QT += widgets network
requires(qtConfig(combobox))

TARGET = DocTimeMachine
