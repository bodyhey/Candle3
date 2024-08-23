TEMPLATE = subdirs

SUBDIRS =   uCNC \
            PropertyEditor

SOURCES += CircularBuffer/CircularBuffer.hpp
INCLUDEPATH += CircularBuffer
HEADERS += CircularBuffer/CircularBuffer.h

