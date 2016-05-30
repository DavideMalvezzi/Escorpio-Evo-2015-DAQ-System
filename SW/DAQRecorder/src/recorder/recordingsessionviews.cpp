#include "recordingsessionviews.h"
#include "ui_recordingsessionviews.h"
#include "recordingsession.h"

SummaryView *RecordingSessionViews::summaryView = Q_NULLPTR;
ChartView *RecordingSessionViews::chartView = Q_NULLPTR;
DashView *RecordingSessionViews::dashView = Q_NULLPTR;
GPSView *RecordingSessionViews::gpsView = Q_NULLPTR;
UtilitiesView *RecordingSessionViews::utilitiesView = Q_NULLPTR;

RecordingSessionViews::RecordingSessionViews(QWidget *parent) : QWidget(parent),ui(new Ui::RecordingSessionViews){
    ui->setupUi(this);
    this->summaryView = ui->summaryView;
    this->chartView = ui->chartView;
    this->dashView = ui->dashView;
    this->gpsView = ui->gpsView;
    this->utilitiesView = ui->utilitiesView;

    connect(ui->sessionTabs, SIGNAL(currentChanged(int)), this, SLOT(reassignWidget(int)));
}

void RecordingSessionViews::setup(){
    ui->sessionTabs->setCurrentIndex(0);
    chartView->setup();
    dashView->setup();
    gpsView->setup(RecordingSession::trackCod);
    utilitiesView->setup(RecordingSession::trackCod);

    summaryView->prepare();
}




void RecordingSessionViews::reassignWidget(int tab){
    switch (tab) {
        case 0:
            summaryView->setup();
            break;
        case 1:
            ui->chartsTab->layout()->addWidget(chartView);
            break;
        case 2:
            ui->dashTab->layout()->addWidget(dashView);
            break;
        case 4:
            ui->utilitiesView->resetWidgets();
            break;
    }
}


RecordingSessionViews::~RecordingSessionViews(){
    qDebug()<<"RECORDING_SESSION_VIEWS: Deleting";
    delete ui;
}
