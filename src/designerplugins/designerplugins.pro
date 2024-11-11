TEMPLATE = subdirs
 
SUBDIRS = customwidgetsplugin

install_customwidgetsplugin.path = ../../bin
install_customwidgetsplugin.files = customwidgetsplugin/customwidgets.dll

win32: {
    SUBDIRS += joystickplugin

    install_joystickplugin.path = ../../bin/plugins/joystick/plugins
    install_joystickplugin.files = joystickplugin/joystickplugin.dll

    INSTALLS += install_customwidgetsplugin
}

CONFIG += c++17
QMAKE_CXXFLAGS += -std=c++17
