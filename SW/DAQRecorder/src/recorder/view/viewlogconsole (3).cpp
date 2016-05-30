#include "viewlogconsole.h"
#include "ui_viewlogconsole.h"

#include <QDateTime>
#include <QDebug>

LogConsole::LogConsole(QWidget *parent) :QWidget(parent),ui(new Ui::LogConsole){
    ui->setupUi(this);
    ui->tableLog->horizontalHeader()->setStretchLastSection(true);
}

void LogConsole::addToLog(QString info){
    if(info.startsWith("INFO")){
        addToLog(INFO, info.remove("INFO"));
    }
    else if(info.startsWith("WARN")){
        addToLog(WARNING, info.remove("WARN"));
    }
    else if(info.startsWith("ERR")){
        addToLog(ERRORS, info.remove("ERR"));
    }
}

void LogConsole::addToLog(Type type, QString info){
    QTableWidgetItem *date = new QTableWidgetItem;
    QTableWidgetItem *infoType = new QTableWidgetItem;
    QTableWidgetItem *msg = new QTableWidgetItem;

    switch (type) {
        case WARNING:
            date->setTextColor(Qt::darkYellow);
            infoType->setTextColor(Qt::darkYellow);
            msg->setTextColor(Qt::darkYellow);
            infoType->setText("WARNING");
            break;
        case ERRORS:
            date->setTextColor(Qt::darkRed);
            infoType->setTextColor(Qt::darkRed);
            msg->setTextColor(Qt::darkRed);
            infoType->setText("ERROR");
            break;
        default:
            infoType->setText("INFO");
            break;
    }

    date->setText(QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss"));
    msg->setText(info);

    ui->tableLog->insertRow(ui->tableLog->rowCount());
    ui->tableLog->setItem(ui->tableLog->rowCount()-1, 0, date);
    ui->tableLog->setItem(ui->tableLog->rowCount()-1, 1, infoType);
    ui->tableLog->setItem(ui->tableLog->rowCount()-1, 2, msg);
    ui->tableLog->selectRow(ui->tableLog->rowCount()-1);
    ui->tableLog->resizeRowsToContents();
}


LogConsole::~LogConsole(){
    qDebug()<<"LOG_CONSOLE: Deleting";
    delete ui;
}
