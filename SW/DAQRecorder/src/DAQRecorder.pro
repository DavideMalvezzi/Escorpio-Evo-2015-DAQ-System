#-------------------------------------------------
#
# Project created by QtCreator 2014-09-08T21:23:32
#
#-------------------------------------------------

QT       += core gui printsupport serialport sql multimedia network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EscorpioDAQRecorder
TEMPLATE = app

#INCLUDEPATH += $$PWD/mapgraphics
#DEPENDPATH += $$PWD/mapgraphics
include($$PWD/mapgraphics/MapGraphics.pri)
RC_FILE = app.rc

SOURCES += main.cpp\
    mainwindow.cpp\
    dialog/packetsettings.cpp \
    dialog/chartviewsettings.cpp \
    dialog/dashsettingsdialog.cpp \
    dialog/gpssettings.cpp \
    dialog/vehiclesettings.cpp \
    dialog/tracksettings.cpp \
    dialog/gsmsettings.cpp \
    recorder/recordingsession.cpp \
    recorder/recordingsessionviews.cpp \
    recorder/view/viewchart.cpp \
    recorder/view/viewdash.cpp \
    recorder/view/viewgps.cpp \
    recorder/view/viewlogconsole.cpp \
    recorder/view/viewsummary.cpp \
    widget/flagobject.cpp \
    widget/chart.cpp \
    widget/customsubwin.cpp \
    widget/qcustomplot.cpp \
    widget/dashwidget.cpp \
    widget/dashpainter.cpp \
    widget/dashled.cpp \
    widget/dashlabel.cpp \
    widget/dashsegment.cpp \
    widget/dashspeedometer.cpp \
    widget/dashalarm.cpp \
    widget/dashbar.cpp \
    widget/dashtermo.cpp \
    connectors/gsminterface.cpp \
    widget/trackplotter.cpp \
    analyzer/analyzersession.cpp \
    analyzer/analyzersessionviews.cpp \
    analyzer/view/chartanalyzer.cpp \
    recorder/view/viewutilities.cpp \
    dialog/teamsettingsdialog.cpp \
    dialog/aboutdialog.cpp \
    widget/databasecreator.cpp \
    analyzer/view/viewchannelspanel.cpp \
    analyzer/channelvalues.cpp

HEADERS  += mainwindow.h\
    viewlogconsole.h \
    recorder/view/viewchart.h \
    recorder/view/viewgps.h \
    recorder/view/viewlogconsole.h \
    recorder/view/viewsummary.h \
    widget/chart.h \
    widget/customsubwin.h \
    widget/qcustomplot.h \
    dialog/packetsettings.h \
    dialog/chartviewsettings.h \
    dialog/vehiclesettings.h \
    dialog/tracksettings.h \
    recorder/recordingsession.h \
    recorder/recordingsessionviews.h \
    dialog/gpssettings.h \
    dialog/gsmsettings.h \
    widget/flagobject.h \
    dialog/dashsettingsdialog.h \
    widget/dashwidget.h \
    widget/dashpainter.h \
    widget/dashled.h \
    widget/dashlabel.h \
    widget/dashsegment.h \
    widget/dashspeedometer.h \
    recorder/view/viewdash.h \
    widget/dashbar.h \
    widget/dashtermo.h \
    connectors/gsminterface.h \
    widget/dashalarm.h \
    widget/trackplotter.h \
    analyzer/analyzersession.h \
    analyzer/analyzersessionviews.h \
    analyzer/view/chartanalyzer.h \
    recorder/view/viewutilities.h \
    dialog/teamsettingsdialog.h \
    dialog/aboutdialog.h \
    widget/databasecreator.h \
    analyzer/view/viewchannelspanel.h \
    analyzer/channelvalues.h

FORMS    += mainwindow.ui \
    recorder/view/viewchart.ui \
    recorder/view/viewgps.ui \
    recorder/view/viewsummary.ui \
    recorder/view/viewlogconsole.ui \
    dialog/packetsettings.ui \
    dialog/chartviewsettings.ui \
    dialog/vehiclesettings.ui \
    dialog/tracksettings.ui \
    recorder/recordingsessionviews.ui \
    dialog/gpssettings.ui \
    dialog/gsmsettings.ui \
    dialog/dashsettingsdialog.ui \
    recorder/view/viewdash.ui \
    analyzer/analyzersessionviews.ui \
    analyzer/view/chartanalyzer.ui \
    recorder/view/viewutilities.ui \
    dialog/teamsettingsdialog.ui \
    analyzer/view/viewchannelspanel.ui

RESOURCES += Resource.qrc
CONFIG += console


