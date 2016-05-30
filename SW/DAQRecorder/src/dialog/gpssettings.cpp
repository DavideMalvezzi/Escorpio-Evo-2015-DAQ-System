#include "gpssettings.h"
#include "ui_gpssettings.h"
#include "../mainwindow.h"

int *GPSSettings::channels = new int[SPECIAL_CHANNELS];

int GPSSettings::getSpecialChannel(int channel){
    return channels[channel];
}

GPSSettings::GPSSettings(QWidget *parent) : QDialog(parent), ui(new Ui::GPSSettings){
    ui->setupUi(this);
    sqlModel = new QSqlTableModel(this, MainWindow::database);
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sqlModel->setTable("CanaleSpeciale");
    sqlModel->select();
    loadSpecialChannels();
    connect(ui->OKButton, SIGNAL(clicked()), this, SLOT(save()));
}

int GPSSettings::exec(){
    QSqlQuery q(MainWindow::database);
    q.exec("SELECT CanID, Nome FROM Canale WHERE (CodTipoDato = 1 OR CodTipoDato = 2) AND (CodTipoIO = 1 OR CodTipoIO = 3)");

    ui->comboTime->clear();
    ui->comboSpace->clear();
    ui->comboLat->clear();
    ui->comboLon->clear();
    ui->comboLap->clear();
    ui->comboWaypoint->clear();

    while(q.next()){
        ui->comboTime->addItem(q.value(1).toString(), q.value(0).toInt());
        ui->comboSpace->addItem(q.value(1).toString(), q.value(0).toInt());
        ui->comboLat->addItem(q.value(1).toString(), q.value(0).toInt());
        ui->comboLon->addItem(q.value(1).toString(), q.value(0).toInt());
        ui->comboLap->addItem(q.value(1).toString(), q.value(0).toInt());
        ui->comboWaypoint->addItem(q.value(1).toString(), q.value(0).toInt());
    }

    ui->comboTime->insertItem(0, "Nessuno", -1);
    ui->comboSpace->insertItem(0, "Nessuno", -1);
    ui->comboLat->insertItem(0, "Nessuno", -1);
    ui->comboLon->insertItem(0, "Nessuno", -1);
    ui->comboLap->insertItem(0, "Nessuno", -1);
    ui->comboWaypoint->insertItem(0, "Nessuno", -1);

    loadSpecialChannels();

    ui->comboTime->setCurrentIndex(ui->comboTime->findData(channels[TIME_CHANNEL]));
    ui->comboSpace->setCurrentIndex(ui->comboSpace->findData(channels[SPACE_CHANNEL]));
    ui->comboLat->setCurrentIndex(ui->comboLat->findData(channels[LAT_CHANNEL]));
    ui->comboLon->setCurrentIndex(ui->comboLon->findData(channels[LON_CHANNEL]));
    ui->comboLap->setCurrentIndex(ui->comboLap->findData(channels[LAP_CHANNEL]));
    ui->comboWaypoint->setCurrentIndex(ui->comboWaypoint->findData(channels[WAYPOINT_CHANNEL]));

    return QDialog::exec();
}

void GPSSettings::save(){
    QSqlQuery q(MainWindow::database);
    channels[LAT_CHANNEL] = ui->comboLat->currentData().toInt();
    channels[LON_CHANNEL] = ui->comboLon->currentData().toInt();
    channels[LAP_CHANNEL] = ui->comboLap->currentData().toInt();
    channels[SPACE_CHANNEL] = ui->comboSpace->currentData().toInt();
    channels[TIME_CHANNEL] = ui->comboTime->currentData().toInt();
    channels[WAYPOINT_CHANNEL] = ui->comboWaypoint->currentData().toInt();


    for(int i=0; i<SPECIAL_CHANNELS; i++){
        sqlModel->setData(sqlModel->index(i,2), channels[i]);
    }

    q.exec("PRAGMA foreign_keys=OFF;");
    sqlModel->database().transaction();
    sqlModel->submitAll();
    sqlModel->database().commit();
    q.exec("PRAGMA foreign_keys=ON;");
    accept();
}

void GPSSettings::loadSpecialChannels(){
     int i=0;
    QSqlQuery q(MainWindow::database);
    q.exec("SELECT CodCanale FROM CanaleSpeciale");
    while(q.next()){
        channels[i++] = q.value(0).toInt();
    }
}

GPSSettings::~GPSSettings(){
    delete ui;
}
