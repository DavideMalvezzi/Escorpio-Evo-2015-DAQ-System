#include "gsmsettings.h"
#include "ui_gsmsettings.h"
#include "../mainwindow.h"


GSMSettings::GSMSettings(QWidget *parent) : QDialog(parent), ui(new Ui::GSMSettings){
    ui->setupUi(this);
    model = new QSqlTableModel(this, MainWindow::database);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setTable("ImpostazioniGSM");
    model->select();

    connect(ui->OKButton, SIGNAL(clicked()), this, SLOT(saveAndClose()));
    connect(ui->exportButton, SIGNAL(clicked()), this, SLOT(exportConfig()));
}

int GSMSettings::exec(){
    QList<QString> settings;
    QSqlQuery q(MainWindow::database);
    q.exec("SELECT Valore FROM ImpostazioniGSM");
    while(q.next()){
        settings.append(q.value(0).toString());
    }

    ui->pttStatCombo->clear();
    ui->pttReqCombo->clear();
    q.exec("SELECT CanID, Nome FROM Canale");
    while(q.next()){
        ui->pttStatCombo->addItem(q.value(1).toString(), q.value(0).toInt());
        ui->pttReqCombo->addItem(q.value(1).toString(), q.value(0).toInt());
    }
    ui->pttStatCombo->insertItem(0, "Nessuno", -1);
    ui->pttReqCombo->insertItem(0, "Nessuno", -1);

    ui->receiverNumText->setText(settings.at(RECEIVER_NUM));
    ui->senderNumText->setText(settings.at(SENDER_NUM));
    ui->receiverPinText->setText(settings.at(RECEIVER_PIN));
    ui->senderPinText->setText(settings.at(SENDER_PIN));
    ui->apnNameText->setText(settings.at(APN_NAME));
    ui->apnUserText->setText(settings.at(APN_USER));
    ui->apnPswText->setText(settings.at(APN_PSW));
    ui->TCPportSpin->setValue(settings.at(TCP_PORT).toInt());
    ui->UDPportSpin->setValue(settings.at(UDP_PORT).toInt());
    ui->packetPerSecondSpin->setValue(settings.at(PACKET_PER_SECOND).toInt());
    ui->packetTimeOutSpin->setValue(settings.at(PACKET_TIMEOUT).toInt());
    ui->pttReqCombo->setCurrentIndex(ui->pttReqCombo->findData(settings.at(PUSH_TO_TALK_REQUEST).toInt()));
    ui->pttStatCombo->setCurrentIndex(ui->pttStatCombo->findData(settings.at(PUSH_TO_TALK_STATUS).toInt()));

    return QDialog::exec();
}

void GSMSettings::save(){
    model->setData(model->index(RECEIVER_NUM, 2), ui->receiverNumText->text().trimmed());
    model->setData(model->index(SENDER_NUM, 2), ui->senderNumText->text().trimmed());
    model->setData(model->index(RECEIVER_PIN, 2), ui->receiverPinText->text().trimmed());
    model->setData(model->index(SENDER_PIN, 2), ui->senderPinText->text().trimmed());
    model->setData(model->index(APN_NAME, 2), ui->apnNameText->text().trimmed());
    model->setData(model->index(APN_USER, 2), ui->apnUserText->text().trimmed());
    model->setData(model->index(APN_PSW, 2), ui->apnPswText->text().trimmed());
    model->setData(model->index(TCP_PORT, 2), ui->TCPportSpin->value());
    model->setData(model->index(UDP_PORT, 2), ui->UDPportSpin->value());
    model->setData(model->index(PACKET_PER_SECOND, 2), ui->packetPerSecondSpin->value());
    model->setData(model->index(PACKET_TIMEOUT, 2), ui->packetTimeOutSpin->value());
    model->setData(model->index(PUSH_TO_TALK_REQUEST, 2), ui->pttReqCombo->currentData().toInt());
    model->setData(model->index(PUSH_TO_TALK_STATUS, 2), ui->pttStatCombo->currentData().toInt());

    model->database().transaction();
    model->submitAll();
    model->database().commit();

}

void GSMSettings::saveAndClose(){
    save();
    accept();
}

void GSMSettings::exportConfig(){
    QMetaObject obj = GSMSettings::staticMetaObject;
    QMetaEnum settingEnum = obj.enumerator(0);
    QFileDialog d(this);
    d.setAcceptMode(QFileDialog::AcceptOpen);
    d.setOption(QFileDialog::ShowDirsOnly);
    d.setFileMode(QFileDialog::Directory);

    save();
    if(d.exec() == 1){
        QFile file(d.selectedFiles().first().append("/GsmConf.txt"));
        if (file.open(QIODevice::WriteOnly)){
            QTextStream stream(&file);
            for(int i=0; i<settingEnum.keyCount(); i++){
                stream<<settingEnum.key(i)<<" = "<<model->data(model->index(i,2)).toString()<<endl;
            }

            file.close();
        }
    }

}

GSMSettings::~GSMSettings(){
    delete ui;
}
