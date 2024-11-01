TEMPLATE = subdirs
 
SUBDIRS = joystickplugin customwidgetsplugin

install_customwidgetsplugin.path = ../../bin
install_customwidgetsplugin.files = customwidgetsplugin/customwidgets.dll

install_joystickplugin.path = ../../bin/plugins/joystick/plugins
install_joystickplugin.files = joystickplugin/joystickplugin.dll

INSTALLS += install_customwidgetsplugin

CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++14
