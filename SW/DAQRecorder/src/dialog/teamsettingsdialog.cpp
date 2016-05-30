#include "teamsettingsdialog.h"
#include "ui_teamsettingsdialog.h"
#include "../mainwindow.h"

TeamSettings::TeamSettings(QWidget *parent) : QDialog(parent), ui(new Ui::TeamSettings){
    ui->setupUi(this);
    sqlModel = new QSqlTableModel(this, MainWindow::database);
    sqlModel->setEditStrategy(QSqlTableModel::OnRowChange);
    sqlModel->setTable("Team");
    sqlModel->select();

    ui->memberTable->setModel(sqlModel);
    ui->memberTable->verticalHeader()->setVisible(false);
    ui->memberTable->setColumnHidden(0,true);
    ui->memberTable->resizeColumnsToContents();
    ui->memberTable->resizeRowsToContents();
    ui->memberTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(sqlModel);
    mapper->addMapping(ui->nameText, 1);
    mapper->addMapping(ui->surnameText, 2);
    mapper->addMapping(ui->roleText, 3);
    mapper->addMapping(ui->telText, 4);

    connect(ui->memberTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));


}

int TeamSettings::exec(){
    ui->memberTable->setCurrentIndex(sqlModel->index(0, 0));
    return QDialog::exec();
}

TeamSettings::~TeamSettings(){
    delete mapper;
    delete ui;
}

void TeamSettings::on_addButton_clicked(){
    QSqlQuery q(MainWindow::database);
    mapper->submit();
    q.exec("INSERT INTO Team(Nome,Cognome,Ruolo,Tel) VALUES('Nome','Cognome','Ruolo','Telefono')");
    sqlModel->select();
    ui->memberTable->setCurrentIndex(sqlModel->index(sqlModel->rowCount()-1, 0));
}

void TeamSettings::on_removeButton_clicked(){
    int index = ui->memberTable->currentIndex().row();
    if(index>=0){
        QSqlQuery q(MainWindow::database);
        mapper->submit();
        q.prepare("DELETE FROM Team WHERE IDP = :id");
        q.bindValue(":id", sqlModel->record(index).value(0).toInt());
        q.exec();
        sqlModel->select();
        ui->memberTable->setCurrentIndex(sqlModel->index(sqlModel->rowCount()-1, 0));
    }
}

void TeamSettings::on_OKbutton_clicked(){
    QSqlQuery q(MainWindow::database);
    mapper->submit();
    sqlModel->database().transaction();
    if (sqlModel->submitAll()) {
        sqlModel->database().commit();
        accept();
    }else{
        sqlModel->database().rollback();
        QMessageBox::critical(this, "Errore", sqlModel->lastError().text());
    }
}
