TEMPLATE = subdirs

SUBDIRS =   src/candle \
            src/designerplugins \
            src/vendor

OTHER_FILES += scripts/*.py

# Make plugins translations
# system(qmake src/candleplugins/candleplugins.pro)

install_main.path = bin
install_main.files += $$OUT_PWD/src/candle/GPilot.exe

install_translations.path = bin/translations
install_translations.files = src/candle/translations/*

install_license.path = bin
install_license.files = LICENSE

install_propertyeditor.path = bin
install_propertyeditor.files = src/vendor/PropertyEditor/PropertyEditor.dll

install_ucnc.path = bin
install_ucnc.files = src/vendor/uCNC/uCNC.dll

install_grbl.path = bin
install_grbl.files = src/vendor/grblHal/grblHal.dll

INSTALLS += install_propertyeditor install_ucnc install_grbl

INSTALLS += install_main

# install_translations install_license

# CONFIG += c++17
# QMAKE_CXXFLAGS += -std=c++17


