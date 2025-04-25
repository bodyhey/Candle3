lessThan(QT_MAJOR_VERSION, 6) {
    message("Cannot use Qt $${QT_VERSION}")
    error("Use Qt 6.8 or newer")
}
equals(QT_MAJOR_VERSION, 6):lessThan(QT_MINOR_VERSION, 8) {
    message("Cannot use Qt $${QT_VERSION}")
    error("Use Qt 6.8 or newer")
}

QT = core gui opengl serialport uitools network
QT += multimedia multimediawidgets

VERSION=1.0.0.0

# DEFINES += DEBUG_UCNC_COMMUNICATION=1
# DEFINES += DEBUG_GRBL_COMMUNICATION=1
DEFINES += DEBUG_RAW_TCP_COMMUNICATION=1
# QT_DEBUG_PLUGINS=1
# DEFINES += QT_DEBUG_PLUGINS=1
# DEFINES += USE_GLWINDOW

win32: {
    DEFINES += WINDOWS
    # QMAKE_CXXFLAGS_DEBUG += -g3 -pg
    # QMAKE_LFLAGS_DEBUG += -pg -lgmon

    build_nr.commands = python $$PWD/../../scripts/build_inc.py $$PWD
    build_nr.depends = FORCE

    QMAKE_EXTRA_TARGETS += build_nr
    PRE_TARGETDEPS += build_nr

    HEADERS  += build.h
}

unix:!macx {
    DEFINES += UNIX LINUX #GL_GLEXT_PROTOTYPES
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN/libs\'"
}

DEFINES += GLES
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

# QMAKE_CXXFLAGS += /std:c++20
# QMAKE_MSC_VER = 1929
# CMAKE_CXX_STANDARD=20

win32-msvc*: {
    QMAKE_CXXFLAGS += /std:c++20
    CMAKE_CXX_STANDARD=20
}
QMAKE_MSC_VER = 1929
CONFIG += c++20
GCC_COMPILE_FLAGS = -std=c++20
win32-msvc*: {
    # QMAKE_CXXFLAGS += -std=c++20
} else {
    # CONFIG += c++17
    # QMAKE_CXXFLAGS += -std=c++17
}

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
    connection/virtualgrblconnection.cpp \
    connection/virtualucncconnection.cpp \
    drawers/cubedrawer.cpp \
    drawers/cursordrawer.cpp \
    drawers/tablesurfacedrawer.cpp \
    drawers/vertexdataexporter.cpp \
    form_partial/main/partmainconsole.cpp \
    form_partial/main/partmaincontrol.cpp \
    form_partial/main/partmainheightmap.cpp \
    form_partial/main/partmainjog.cpp \
    form_partial/main/partmainoverride.cpp \
    form_partial/main/partmainspindle.cpp \
    form_partial/main/partmainstate.cpp \
    form_partial/main/partmainstatelcd.cpp \
    form_partial/main/partmainvirtualsettings.cpp \
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
    gcode/gcodethreadedloader.cpp \
    heightmap.cpp \
    machine/machineconfiguration.cpp \
    module/camera/camera.cpp \
    # module/camera/qvideoframeconversionhelper.cpp \
    # module/camera/videosurface.cpp \
    # module/camera/viewfinder.cpp \
    module/camera/cameraframeprocessor.cpp \
    module/pendant/pendant.cpp \
    parser/arcproperties.cpp \
    parser/gcodeparser.cpp \
    parser/gcodepreprocessorutils.cpp \
    parser/gcodeviewparser.cpp \
    parser/linesegment.cpp \
    parser/pointsegment.cpp \
    state/state.cpp \
    state/stateconnecting.cpp \
    state/stateerror.cpp \
    state/statehoming.cpp \
    state/stateidle.cpp \
    state/stateinitialization.cpp \
    state/statejogging.cpp \
    state/statejoggingwaitingforidle.cpp \
    state/stateprobing.cpp \
    state/staterunning.cpp \
    tables/gcodetablemodel.cpp \
    tables/heightmaptablemodel.cpp \
    utils.cpp \
    widgets/combobox.cpp \
    widgets/comboboxkey.cpp \
    widgets/glcontainer.cpp \
    widgets/glpalette.cpp \
    widgets/glwidget.cpp \
    drawers/selectiondrawer.cpp \
    scripting/scriptvars.cpp \
    widgets/dropwidget.cpp \
    widgets/qpushbuttonwithmenu.cpp

HEADERS  += frmmain.h \
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
    connection/virtualgrblconnection.h \
    connection/virtualucncconnection.h \
    drawers/cube.h \
    drawers/cubedrawer.h \
    drawers/cursordrawer.h \
    drawers/tablesurfacedrawer.h \
    drawers/vertexdataexporter.h \
    form_partial/main/partmainconsole.h \
    form_partial/main/partmaincontrol.h \
    form_partial/main/partmainheightmap.h \
    form_partial/main/partmainjog.h \
    form_partial/main/partmainoverride.h \
    form_partial/main/partmainspindle.h \
    form_partial/main/partmainstate.h \
    form_partial/main/partmainstatelcd.h \
    form_partial/main/partmainvirtualsettings.h \
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
    gcode/gcodethreadedloader.h \
    globals.h \
    heightmap.h \
    machine/machineconfiguration.h \
    module/camera/camera.h \
    # module/camera/qvideoframeconversionhelper.h \
    # module/camera/videosurface.h \
    # module/camera/viewfinder.h \
    module/camera/cameraframeprocessor.h \
    module/pendant/pendant.h \
    parser/arcproperties.h \
    parser/gcodeparser.h \
    parser/gcodepreprocessorutils.h \
    parser/gcodeviewparser.h \
    parser/linesegment.h \
    parser/pointsegment.h \
    state/state.h \
    state/stateconnecting.h \
    state/stateerror.h \
    state/statehoming.h \
    state/stateidle.h \
    state/stateinitialization.h \
    state/statejogging.h \
    state/statejoggingwaitingforidle.h \
    state/stateprobing.h \
    state/staterunning.h \
    state/states.h \
    tables/gcodetablemodel.h \
    tables/heightmaptablemodel.h \
    utils.h \
    utils/interpolation.h \
    utils/util.h \
    utils/validators.h \
    widgets/combobox.h \
    widgets/comboboxkey.h \
    widgets/glcontainer.h \
    widgets/glpalette.h \
    widgets/glwidget.h \
    drawers/selectiondrawer.h \
    scripting/scriptvars.h \
    widgets/dropwidget.h \
    widgets/qpushbuttonwithmenu.h

FORMS    += frmmain.ui \
    form_partial/main/partmainconsole.ui \
    form_partial/main/partmaincontrol.ui \
    form_partial/main/partmainheightmap.ui \
    form_partial/main/partmainjog.ui \
    form_partial/main/partmainoverride.ui \
    form_partial/main/partmainspindle.ui \
    form_partial/main/partmainstate.ui \
    form_partial/main/partmainstatelcd.ui \
    form_partial/main/partmainvirtualsettings.ui \
    frmdebug.ui \
    frmgrblconfigurator.ui \
    frmsettings.ui \
    frmabout.ui \
    module/camera/camera.ui

DEFINES += _USE_MATH_DEFINES

RESOURCES += \
    fonts.qrc \
    shaders.qrc \
    images.qrc

INCLUDEPATH += ../designerplugins/customwidgetsplugin
INCLUDEPATH += ../vendor/PropertyEditor

include(../vendor/CRC.pri)
include(../vendor/PropertyEditor/PropertyEditor.pri)
include(../vendor/phantomstyle/src/phantom/phantom.pri)


LIBS += -L../designerplugins/customwidgetsplugin -lcustomwidgets

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

# INCLUDEPATH += $$PWD/../vendor/uCNC/build
# INCLUDEPATH += $$PWD/../vendor/uCNC/makefiles/virtual
# DEPENDPATH += ../vendor/uCNC

#LIBS += C:/Projekty/Qt/Candle/src/vendor/build/uCNC-Debug

LIBS += -L../vendor/uCNC -luCNC
LIBS += -L../vendor/grblHal -lgrblHal

DISTFILES += \
    shaders/2dcopy_fragment.glsl \
    shaders/2dcopy_vertex.glsl \
    shaders/base_fragment.glsl \
    shaders/base_vertex.glsl \
    shaders/cube_fragment.glsl \
    shaders/cube_vertex.glsl \
    shaders/gcode_fragment.glsl \
    shaders/gcode_vertex.glsl

