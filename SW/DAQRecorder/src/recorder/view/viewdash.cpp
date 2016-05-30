#include "viewdash.h"
#include "ui_viewdash.h"
#include "../../mainwindow.h"
#include "../../dialog/dashsettingsdialog.h"

DashView::DashView(QWidget *parent) : QWidget(parent),ui(new Ui::DashView){
    ui->setupUi(this);
    repaintTimer = new QTimer;
    repaintTimer->setInterval(200);
    repaintTimer->setSingleShot(true);
    connect(repaintTimer, SIGNAL(timeout()), ui->painter, SLOT(repaint()));
}

void DashView::setup(){
    QList<DashWidget*> *widgets = ui->painter->getWidgetsList();
    while(widgets->count()){
        delete widgets->takeAt(0);
    }

    QSqlRelationalTableModel *sqlModel = new QSqlRelationalTableModel(this, MainWindow::database);
    sqlModel->setTable("WidgetCruscotto");
    sqlModel->select();
    QSqlRecord record;

    for(int i=0; i<sqlModel->rowCount(); i++){
        record = sqlModel->record(i);
        ui->painter->addWidget(createWidget(
                                   i,
                                   record.value(1).toInt(),
                                   record.value(2).toInt(),
                                   record.value(3).toString()
                                   ),false);
    }
    delete sqlModel;
    ui->painter->setDisabled(true);
    lastPacketIndex = 0;
}

DashWidget* DashView::createWidget(int index, int canID, int type, QString config){
    DashWidget *widget;
    QJsonDocument jsonResponse = QJsonDocument::fromJson(config.toUtf8());
    QJsonObject obj = jsonResponse.object();
    switch (type) {
        case LED:
            widget = new DashLed(index, canID, obj);
            return widget;
        case LABEL:
            widget = new DashLabel(index, canID, obj);
            widget->select();
            return widget;
        case DIGIT:
            widget = new DashSegment(index, canID, obj);
            widget->select();
            return widget;
        case ANGOLMETER:
            widget = new DashSpeedometer(index, canID, obj);
            widget->select();
            return widget;
        case BAR:
            widget = new DashBar(index, canID, obj);
            return widget;
        case THERMOMETER:
            widget = new DashTermo(index, canID, obj);
            widget->select();
            return widget;
        case ALARM:
            widget = new DashAlarm(index, canID, obj);
            widget->select();
            return widget;
    }
    return new DashWidget();
}

void DashView::onChannelUpdate(int ID, unsigned int packetIndex, QVariant value){
    QList<DashWidget*> *widgets = ui->painter->getWidgetsList();
    if(packetIndex>=lastPacketIndex){ //Evito di aggiornare se il paccheto è stato inviato dopo
        lastPacketIndex = packetIndex;
        for(int i=0; i<widgets->count(); i++){
            if(widgets->at(i)->getCanID() == ID){
                widgets->at(i)->updateValue(value);

                if(!repaintTimer->isActive()){
                    repaintTimer->start();
                }

            }
        }
    }else{
        qDebug()<<"DASH_VIEW: Old packet "<<lastPacketIndex;
    }
}

DashView::~DashView(){
    qDebug()<<"DASH_VIEW: Deleting";
    delete ui;
}
