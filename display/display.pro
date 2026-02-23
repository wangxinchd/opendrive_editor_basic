QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    form_edit/edit_road/form_edit_road.cpp \
    main.cpp \
    display.cpp \
    display_config/display_config.cc \
    display_config/display_config_dialog.cc

HEADERS += \
    display.h \
    form_edit/edit_road/form_edit_road.h \
    form_edit_road.h \
    display_config/display_config.h \
    display_config/display_config_dialog.h

FORMS += \
    display.ui \
    form_edit/edit_road/form_edit_road.ui \
    form_edit_road.ui \
    display_config/display_config_dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
