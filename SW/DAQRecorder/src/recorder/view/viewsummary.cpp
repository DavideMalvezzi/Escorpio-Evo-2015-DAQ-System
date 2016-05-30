#include "viewsummary.h"
#include "ui_viewsummary.h"
#include "../../mainwindow.h"
#include "../recordingsession.h"
#include "../recordingsessionviews.h"


SummaryView::SummaryView(QWidget *parent) : QWidget(parent), ui(new Ui::SummaryView){
    ui->setupUi(this);
/*
    callSum = new CustomSubWin(ui->mdiArea);
    callSum->setWindowTitle("Chiamate");
    callSum->setWindowIcon(QIcon(":/images/icons/startcall.png"));
    ui->mdiArea->addSubWindow(callSum);

    statsSum = new CustomSubWin(ui->mdiArea);
    statsSum->setWindowTitle("Statistiche");
    statsSum->setWindowIcon(QIcon(":/images/icons/bars.png"));
    ui->mdiArea->addSubWindow(statsSum);
*/
    timeSum = new CustomSubWin(ui->mdiArea);
    timeSum->setWindowTitle("Tempi");
    timeSum->setWindowIcon(QIcon(":/images/icons/timetable.png"));
    ui->mdiArea->addSubWindow(timeSum);

    gpsSum = new CustomSubWin(ui->mdiArea);
    gpsSum->setWindowTitle("GPS");
    gpsSum->setWindowIcon(QIcon(":/images/icons/gps.png"));
    ui->mdiArea->addSubWindow(gpsSum);

    dashSum = new CustomSubWin(ui->mdiArea);
    dashSum->setWindowTitle("Cruscotto");
    dashSum->setWindowIcon(QIcon(":/images/icons/dashboard.png"));
    ui->mdiArea->addSubWindow(dashSum);

    chartSum = new CustomSubWin(ui->mdiArea);
    chartSum->setWindowTitle("Grafici");
    chartSum->setWindowIcon(QIcon(":/images/icons/chart.png"));
    ui->mdiArea->addSubWindow(chartSum);

    ui->mdiArea->setActivationOrder(QMdiArea::CreationOrder);
    ui->mdiArea->tileSubWindows();

}

void SummaryView::prepare(){
    setup();

    if(gpsSum->widget()){
        delete gpsSum->widget();
    }
    GPSView *gps = new GPSView();
    gps->setup(RecordingSession::trackCod);
    gps->setZoom(16);
    connect(MainWindow::recordingSession, SIGNAL(channelUpdate(int,unsigned int,QVariant)),
            gps, SLOT(onChannelUpdate(int,unsigned int,QVariant)));
    gpsSum->setWidget(gps);

}

void SummaryView::resizeEvent(QResizeEvent*evt){
    Q_UNUSED(evt)
    /*
    chartSum->setGeometry(0,0,ui->mdiArea->width()/2,ui->mdiArea->height());
    dashSum->setGeometry(ui->mdiArea->width()/2,0,ui->mdiArea->width()/2,ui->mdiArea->height()*0.55f);
    gpsSum->setGeometry(ui->mdiArea->width()/2,ui->mdiArea->height()*0.55f,ui->mdiArea->width()*0.25f,ui->mdiArea->height()*0.45);
    timeSum->setGeometry(ui->mdiArea->width()*3/4,ui->mdiArea->height()*0.55f,ui->mdiArea->width()*0.25f,ui->mdiArea->height()*0.45);
*/
    ui->mdiArea->tileSubWindows();
}


void SummaryView::setup(){
    chartSum->layout()->addWidget(RecordingSessionViews::chartView);
    dashSum->layout()->addWidget(RecordingSessionViews::dashView);
    timeSum->layout()->addWidget(RecordingSessionViews::utilitiesView->getTimeTable());
    //statsSum->layout()->addWidget(RecordingSessionViews::utilitiesView->getStatsTable());
    //callSum->layout()->addWidget(RecordingSessionViews::utilitiesView->getCallManager());
}


SummaryView::~SummaryView(){
    qDebug()<<"SUMMARY_VIEW: Deleting";
    delete ui;
}
