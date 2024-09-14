TEMPLATE = subdirs

SUBDIRS =   uCNC \
            PropertyEditor

SOURCES += CRC/src/CRC.cpp \
           CRC/src/CRC8.cpp
INCLUDEPATH += CRC
HEADERS += CRC/src/CRC.h \
           CRC/src/CRC8.h

