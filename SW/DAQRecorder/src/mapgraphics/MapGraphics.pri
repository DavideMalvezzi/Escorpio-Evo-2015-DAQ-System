#-------------------------------------------------
#
# Project created by QtCreator 2012-03-03T10:50:47
#
#-------------------------------------------------

QT       += network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES += MAPGRAPHICS_LIBRARY

SOURCES += $$PWD/MapGraphicsScene.cpp \
    $$PWD/MapGraphicsObject.cpp \
    $$PWD/MapGraphicsView.cpp \
    $$PWD/guts/PrivateQGraphicsScene.cpp \
    $$PWD/guts/PrivateQGraphicsObject.cpp \
    $$PWD/guts/Conversions.cpp \
    $$PWD/MapTileSource.cpp \
    $$PWD/tileSources/GridTileSource.cpp \
    $$PWD/guts/MapTileGraphicsObject.cpp \
    $$PWD/guts/PrivateQGraphicsView.cpp \
    $$PWD/tileSources/OSMTileSource.cpp \
    $$PWD/guts/MapGraphicsNetwork.cpp \
    $$PWD/tileSources/CompositeTileSource.cpp \
    $$PWD/guts/MapTileLayerListModel.cpp \
    $$PWD/guts/MapTileSourceDelegate.cpp \
    $$PWD/guts/CompositeTileSourceConfigurationWidget.cpp \
    $$PWD/CircleObject.cpp \
    $$PWD/guts/PrivateQGraphicsInfoSource.cpp \
    $$PWD/PolygonObject.cpp \
    $$PWD/Position.cpp \
    $$PWD/LineObject.cpp

HEADERS += $$PWD/MapGraphicsScene.h\
    $$PWD/MapGraphics_global.h \
    $$PWD/MapGraphicsObject.h \
    $$PWD/MapGraphicsView.h \
    $$PWD/guts/PrivateQGraphicsScene.h \
    $$PWD/guts/PrivateQGraphicsObject.h \
    $$PWD/guts/Conversions.h \
    $$PWD/MapTileSource.h \
    $$PWD/tileSources/GridTileSource.h \
    $$PWD/guts/MapTileGraphicsObject.h \
    $$PWD/guts/PrivateQGraphicsView.h \
    $$PWD/tileSources/OSMTileSource.h \
    $$PWD/guts/MapGraphicsNetwork.h \
    $$PWD/tileSources/CompositeTileSource.h \
    $$PWD/guts/MapTileLayerListModel.h \
    $$PWD/guts/MapTileSourceDelegate.h \
    $$PWD/guts/CompositeTileSourceConfigurationWidget.h \
    $$PWD/CircleObject.h \
    $$PWD/guts/PrivateQGraphicsInfoSource.h \
    $$PWD/PolygonObject.h \
    $$PWD/Position.h \
    $$PWD/LineObject.h


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

FORMS += $$PWD/guts/CompositeTileSourceConfigurationWidget.ui

RESOURCES += \
    $$PWD/resources.qrc
