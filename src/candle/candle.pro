#-------------------------------------------------
#
# Project created by QtCreator 2014-05-23T17:51:21
#
#-------------------------------------------------

QT       = core gui opengl serialport uitools
equals(QT_MAJOR_VERSION, 5): QT += widgets network

DEFINES += DEBUG_UCNC_COMMUNICATION=1

win32: {
    QT += winextras
    DEFINES += WINDOWS
    QMAKE_CXXFLAGS_DEBUG += -g3 -pg
    QMAKE_LFLAGS_DEBUG += -pg -lgmon

    build_nr.commands = python $$PWD/../../scripts/build_inc.py $$PWD
    build_nr.depends = FORCE

    QMAKE_EXTRA_TARGETS += build_nr
    PRE_TARGETDEPS += build_nr

    HEADERS  += build.h
}

unix:!macx {
    DEFINES += UNIX #GL_GLEXT_PROTOTYPES
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/libs\'"
}

contains(QT_CONFIG, opengles.) {
    warning("GL ES detected. VAO will be disabled.")
    DEFINES += GLES
    INSTALLS += target
    target.path = /home/pi
}

TARGET = GPilot
TEMPLATE = app

RC_ICONS = images/gpilot.ico

DEFINES += sNan=\"65536\"

TRANSLATIONS += translations/candle_en.ts translations/candle_ru.ts translations/candle_es.ts translations/candle_fr.ts translations/candle_pt.ts

CONFIG += c++14
QMAKE_CXXFLAGS += -std=c++14

# don't create both debug and release folders
CONFIG -= debug_and_release

SOURCES += main.cpp\
    communicator.cpp \
    communicator_processing_response.cpp \
    communicator_utils.cpp \
    config/configuration.cpp \
    config/module/configurationconnection.cpp \
    config/module/configurationconsole.cpp \
    config/module/configurationheightmap.cpp \
    config/module/configurationjogging.cpp \
    config/module/configurationmachine.cpp \
    config/module/configurationmodule.cpp \
    config/module/configurationparser.cpp \
    config/module/configurationsender.cpp \
    config/module/configurationui.cpp \
    config/module/configurationvisualizer.cpp \
    config/persistence/ini/inipersister.cpp \
    config/persistence/ini/iniprovider.cpp \
    config/persistence/xml/xmlpersister.cpp \
    config/persistence/xml/xmlprovider.cpp \
    connection/connection.cpp \
    connection/connectionmanager.cpp \
    connection/rawtcpconnection.cpp \
    connection/serialconnection.cpp \
    connection/virtualucncconnection.cpp \
    drawers/cursordrawer.cpp \
    drawers/tablesurfacedrawer.cpp \
    form_partial/main/partmainconsole.cpp \
    form_partial/main/partmaincontrol.cpp \
    form_partial/main/partmainheightmap.cpp \
    form_partial/main/partmainjog.cpp \
    form_partial/main/partmainoverride.cpp \
    form_partial/main/partmainspindle.cpp \
    form_partial/main/partmainstate.cpp \
    frmgrblconfigurator.cpp \
    frmmain.cpp \
    frmsettings.cpp \
    frmabout.cpp \
    drawers/gcodedrawer.cpp \
    drawers/heightmapborderdrawer.cpp \
    drawers/heightmapgriddrawer.cpp \
    drawers/heightmapinterpolationdrawer.cpp \
    drawers/origindrawer.cpp \
    drawers/shaderdrawable.cpp \
    drawers/tooldrawer.cpp \
    drawers/machineboundsdrawer.cpp \
    gcode/gcode.cpp \
    gcode/gcodeexporter.cpp \
    gcode/gcodeloader.cpp \
    heightmap.cpp \
    machine/machineconfiguration.cpp \
    module/camera/camera.cpp \
    module/camera/qvideoframeconversionhelper.cpp \
    module/camera/videosurface.cpp \
    module/camera/viewfinder.cpp \
    module/pendant/pendant.cpp \
    parser/arcproperties.cpp \
    parser/gcodeparser.cpp \
    parser/gcodepreprocessorutils.cpp \
    parser/gcodeviewparse.cpp \
    parser/linesegment.cpp \
    parser/pointsegment.cpp \
    tables/gcodetablemodel.cpp \
    tables/heightmaptablemodel.cpp \
    widgets/combobox.cpp \
    widgets/comboboxkey.cpp \
    widgets/glwidget.cpp \
    drawers/selectiondrawer.cpp \
    scripting/scriptvars.cpp \
    widgets/dropwidget.cpp \
    ../vendor/CRC/src/CrcFastReverse.cpp \
    ../vendor/CRC/src/CRC.cpp \
    ../vendor/CRC/src/CRC8.cpp \
    ../vendor/CRC/src/CRC12.cpp \
    ../vendor/CRC/src/CRC16.cpp \
    ../vendor/CRC/src/CRC32.cpp \
    ../vendor/CRC/src/CRC64.cpp

HEADERS  += frmmain.h \
    Arduino.h \
    communicator.h \
    config/configuration.h \
    config/module/configurationconnection.h \
    config/module/configurationconsole.h \
    config/module/configurationheightmap.h \
    config/module/configurationjogging.h \
    config/module/configurationmachine.h \
    config/module/configurationmodule.h \
    config/module/configurationparser.h \
    config/module/configurationsender.h \
    config/module/configurationui.h \
    config/module/configurationvisualizer.h \
    config/persistence/persister.h \
    config/persistence/provider.h \
    config/persistence/ini/inipersister.h \
    config/persistence/ini/iniprovider.h \
    config/persistence/xml/xmlpersister.h \
    config/persistence/xml/xmlprovider.h \
    config/registry.h \
    connection/connection.h \
    connection/connectionmanager.h \
    connection/rawtcpconnection.h \
    connection/serialconnection.h \
    connection/virtualucncconnection.h \
    drawers/cursordrawer.h \
    drawers/tablesurfacedrawer.h \
    form_partial/main/partmainconsole.h \
    form_partial/main/partmaincontrol.h \
    form_partial/main/partmainheightmap.h \
    form_partial/main/partmainjog.h \
    form_partial/main/partmainoverride.h \
    form_partial/main/partmainspindle.h \
    form_partial/main/partmainstate.h \
    frmgrblconfigurator.h \
    frmsettings.h \
    frmabout.h \
    drawers/gcodedrawer.h \
    drawers/heightmapborderdrawer.h \
    drawers/heightmapgriddrawer.h \
    drawers/heightmapinterpolationdrawer.h \
    drawers/origindrawer.h \
    drawers/shaderdrawable.h \
    drawers/tooldrawer.h \
    drawers/machineboundsdrawer.h \
    gcode/gcode.h \
    gcode/gcodeexporter.h \
    gcode/gcodeloader.h \
    globals.h \
    heightmap.h \
    machine/machineconfiguration.h \
    module/camera/camera.h \
    module/camera/qvideoframeconversionhelper.h \
    module/camera/videosurface.h \
    module/camera/viewfinder.h \
    module/pendant/pendant.h \
    parser/arcproperties.h \
    parser/gcodeparser.h \
    parser/gcodepreprocessorutils.h \
    parser/gcodeviewparse.h \
    parser/linesegment.h \
    parser/pointsegment.h \
    tables/gcodetablemodel.h \
    tables/heightmaptablemodel.h \
    utils/interpolation.h \
    utils/util.h \
    utils/validators.h \
    widgets/combobox.h \
    widgets/comboboxkey.h \
    widgets/glwidget.h \
    drawers/selectiondrawer.h \
    scripting/scriptvars.h \
    widgets/dropwidget.h

FORMS    += frmmain.ui \
    form_partial/main/partmainconsole.ui \
    form_partial/main/partmaincontrol.ui \
    form_partial/main/partmainheightmap.ui \
    form_partial/main/partmainjog.ui \
    form_partial/main/partmainoverride.ui \
    form_partial/main/partmainspindle.ui \
    form_partial/main/partmainstate.ui \
    frmdebug.ui \
    frmgrblconfigurator.ui \
    frmsettings.ui \
    frmabout.ui \
    module/camera/camera.ui

DEFINES += _USE_MATH_DEFINES

RESOURCES += \
    shaders.qrc \
    images.qrc

INCLUDEPATH += ../designerplugins/customwidgetsplugin
INCLUDEPATH += ../vendor/PropertyEditor
INCLUDEPATH += ../vendor/CRC/src

LIBS += -L../designerplugins/customwidgetsplugin -lcustomwidgets
LIBS += -L../vendor/PropertyEditor -lPropertyEditor

# qtPrepareTool(LRELEASE, lrelease)
# for(tsfile, TRANSLATIONS) {
#     qmfile = $$tsfile
#     qmfile ~= s,.ts$,.qm,
#     qmdir = $$dirname(qmfile)
#     !exists($$qmdir) {
#         mkpath($$qmdir)|error("Aborting.")
#     }
#     command = $$LRELEASE -removeidentical $$tsfile -qm $$qmfile
#     system($$command)|error("Failed to run: $$command")
# }

LIBS += -L../vendor/uCNC -luCNC

INCLUDEPATH += $$PWD/../vendor/uCNC/build
INCLUDEPATH += $$PWD/../vendor/uCNC/makefiles/virtual
DEPENDPATH += ../vendor/uCNC

# win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../vendor/uCNC/release/libuCNC.a
# else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../vendor/uCNC/release/uCNC.lib

# win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../vendor/uCNC/release/ -luCNC
# else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../vendor/uCNC/debug/ -luCNC

# INCLUDEPATH += $$PWD/../vendor/uCNC/build
# DEPENDPATH += $$PWD/../vendor/uCNC/build

# win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../vendor/uCNC/release/libuCNC.a
# else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../vendor/uCNC/debug/libuCNC.a
# else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../vendor/uCNC/release/uCNC.lib
# else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../vendor/uCNC/debug/uCNC.lib

win32: LIBS += -luCNC
