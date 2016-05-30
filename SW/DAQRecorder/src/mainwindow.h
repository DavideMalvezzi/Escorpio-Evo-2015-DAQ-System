#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSqlDatabase>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include "connectors/gsminterface.h"
#include "recorder/recordingsession.h"
#include "recorder/recordingsessionviews.h"
#include "recorder/view/viewlogconsole.h"
#include "analyzer/analyzersession.h"
#include "analyzer/analyzersessionviews.h"
#include "dialog/packetsettings.h"
#include "dialog/chartviewsettings.h"
#include "dialog/dashsettingsdialog.h"
#include "dialog/gpssettings.h"
#include "dialog/teamsettingsdialog.h"
#include "dialog/vehiclesettings.h"
#include "dialog/tracksettings.h"
#include "dialog/gsmsettings.h"
#include "dialog/aboutdialog.h"
#include "widget/databasecreator.h"



namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    //Database
    static QSqlDatabase database;

    //Views
    static MainWindow *mainWindow;
    static LogConsole *logConsole;
    static RecordingSessionViews *recordingViews;
    static AnalyzerSessionViews *analyzerViews;

    //Dialogs
    /*
    static PacketSettingsDialog *packetSettingsDialog;
    static ChartViewSettings *chartsSettingsDialog;
    static DashSettingsDialog *dashSettingsDialog;
    static GPSSettings *gpsSettingsDialog;
    static TeamSettings *teamSettingsDialog;
    static VehicleSettings *vehicleSettingsDialog;
    static TrackSettings *trackSettingsDialog;
    static GSMSettings *gsmSettingsDialog;
    static AboutDialog *aboutDialog;
    */

    //Session
    static RecordingSession *recordingSession;
    static AnalyzerSession *analyzerSession;
    static GSMInterface *gsm;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    //Serial Settings
    void refreshTrackAndVehicle();
    void on_buttonRefreshPort_clicked();
    void on_buttonOpenCom_clicked();

    //Database access
    void loadDB(QString name);
    void raceTreeItemClicked(QTreeWidgetItem*);

    //File Menu
    void on_actionApri_triggered();
    void on_actionExportCSV_triggered();
    void on_actionEsci_triggered();

    //Recorder Menu
    void closeRecorder();

    //Settings Menu
    void on_actionChannelsSettings_triggered();
    void on_actionChartSettings_triggered();
    void on_actionWidgetSettings_triggered();
    void on_actionGPSSettings_triggered();
    void on_actionVehicleSettings_triggered();
    void on_actionTrackSettings_triggered();
    void on_actionGSM_triggered();

    void on_actionTeam_triggered();

    void on_actionNuovo_triggered();

    void on_actionInformazioni_triggered();

    void on_actionAvvia_registrazione_triggered();

    void on_actionRiprendi_registrazione_triggered();

    void on_buttonOpemAnalyzer_clicked();

signals:
    void msgFromSerial(QByteArray);

private:
    Ui::MainWindow *ui;
    QByteArray rxBuffer;
};



#endif // MAINWINDOW_H
