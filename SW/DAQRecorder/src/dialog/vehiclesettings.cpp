#include "vehiclesettings.h"
#include "ui_vehiclesettings.h"
#include "../mainwindow.h"

#include <QSqlRecord>
#include <QSqlError>

VehicleSettings::VehicleSettings(QWidget *parent) : QDialog(parent), ui(new Ui::VehicleSettings){
    ui->setupUi(this);
    ui->vehicleImgPath->setVisible(false);

    connect(ui->vehicleCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(selectVehicle()));
    connect(ui->vehicleModelButton, SIGNAL(clicked()), this, SLOT(selectVehicleModel()));

    connect(ui->vehicleImg, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(selectVehicleImg()));

    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addVehicle()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeVehicle()));
    connect(ui->OKButton, SIGNAL(clicked()), this, SLOT(accept()));

    QSqlQueryModel *vehicleNames = new QSqlQueryModel();
    vehicleNames->setQuery("SELECT IDVeicolo, Nome FROM Veicolo", MainWindow::database);
    ui->vehicleCombo->setModel(vehicleNames);
    ui->vehicleCombo->setModelColumn(1);
    reloadVehicleList();
}

int VehicleSettings::exec(){
    if(ui->vehicleCombo->count()>0){
        ui->vehicleCombo->setCurrentIndex(0);
    }
    return QDialog::exec();
}

void VehicleSettings::reloadVehicleList(){
    QSqlQueryModel *vehicleNames = (QSqlQueryModel*)ui->vehicleCombo->model();
    vehicleNames->setQuery("SELECT IDVeicolo, Nome FROM Veicolo", MainWindow::database);
}

void VehicleSettings::selectVehicle(){
    QSqlQueryModel *vehicleNames = (QSqlQueryModel*)ui->vehicleCombo->model();
    int index = ui->vehicleCombo->currentIndex();
    int id = vehicleNames->data(vehicleNames->index(index, 0)).toInt();

    QSqlQuery q(MainWindow::database);
    q.prepare("SELECT * FROM Veicolo WHERE IDVeicolo=:id");
    q.bindValue(":id", id);
    q.exec();q.next();
    QSqlRecord result = q.record();

    ui->vehicleName->setText(result.value(1).toString());
    ui->vehicleModel->setText(result.value(2).toString());
    ui->vehicleDesc->setText(result.value(4).toString());
    QPixmap img(result.value(3).toString());
    ui->vehicleImgPath->setText(result.value(3).toString());
    if(!img.isNull()){
        ui->vehicleImg->setPixmap(img.scaled(ui->vehicleImg->width(),ui->vehicleImg->height(), Qt::KeepAspectRatio));
        ui->vehicleImg->setText("");
    }else{
        ui->vehicleImg->setPixmap(QPixmap());
        ui->vehicleImg->setText("Nessuna immagine selezionata\nClick destro per modificare");
    }
}

void VehicleSettings::addVehicle(){
    QSqlQuery q(MainWindow::database);
    q.exec("INSERT INTO Veicolo(Nome) VALUES('Nuovo Veicolo')");
    reloadVehicleList();
    ui->vehicleCombo->setCurrentIndex(ui->vehicleCombo->count()-1);
}

void VehicleSettings::removeVehicle(){
    QSqlQueryModel *vehicleNames = (QSqlQueryModel*)ui->vehicleCombo->model();
    int index = ui->vehicleCombo->currentIndex();
    int id = vehicleNames->data(vehicleNames->index(index, 0)).toInt();

    QSqlQuery q(MainWindow::database);
    q.prepare("SELECT * FROM Gara WHERE CodVeicolo=:id LIMIT 1");
    q.bindValue(":id", id);
    q.exec();
    if(q.next()){
        QMessageBox::critical(this, "Errore", "Impossibile eliminare il veicolo poichè già in uso!");
    }else{
        q.prepare("DELETE FROM Veicolo WHERE IDVeicolo=:id");
        q.bindValue(":id", id);
        q.exec();
        reloadVehicleList();
        ui->vehicleCombo->setCurrentIndex(0);
    }
}

void VehicleSettings::save(){
    QSqlQueryModel *vehicleNames = (QSqlQueryModel*)ui->vehicleCombo->model();
    int index = ui->vehicleCombo->currentIndex();
    int id = vehicleNames->data(vehicleNames->index(index, 0)).toInt();

    QSqlQuery q(MainWindow::database);
    q.prepare("UPDATE Veicolo SET Nome=:nome, Modello=:mod, Foto=:img, Descrizione=:desc WHERE IDVeicolo=:id");
    q.bindValue(":id", id);
    q.bindValue(":nome", ui->vehicleName->text());
    q.bindValue(":mod", ui->vehicleModel->text());
    q.bindValue(":img", ui->vehicleImgPath->text());
    q.bindValue(":desc", ui->vehicleDesc->toPlainText());
    q.exec();
    if(q.lastError().isValid()){
        QMessageBox::critical(this, "Errore", QString("Impossibile salvare!\n%1").arg(q.lastError().text()));
    }

    reloadVehicleList();
    ui->vehicleCombo->setCurrentIndex(index);
}

void VehicleSettings::selectVehicleImg(){
    QFileDialog d(this);
    d.setFileMode(QFileDialog::ExistingFile);
    d.setNameFilter("JPG Files (*.jpg *.jpeg);;PNG Files (*.png)");
    if(d.exec() == 1){
        ui->vehicleImgPath->setText(d.selectedFiles().first());
        QPixmap img(ui->vehicleImgPath->text());
        if(!img.isNull()){
            ui->vehicleImg->setPixmap(img.scaled(ui->vehicleImg->width(),ui->vehicleImg->height(), Qt::KeepAspectRatio));
            ui->vehicleImg->setText("");
        }else{
            ui->vehicleImg->setPixmap(QPixmap());
            ui->vehicleImg->setText("Nessuna immagine selezionata\nClick destro per modificare");
        }
    }
}

void VehicleSettings::selectVehicleModel(){
    QFileDialog d(this);
    d.setFileMode(QFileDialog::ExistingFile);
    if(d.exec() == 1){
        ui->vehicleModel->setText(d.selectedFiles().first());
    }
}


VehicleSettings::~VehicleSettings(){
    delete ui;
}
