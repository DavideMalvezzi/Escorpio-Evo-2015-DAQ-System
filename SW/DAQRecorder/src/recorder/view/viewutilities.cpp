#include "viewutilities.h"
#include "ui_viewutilities.h"
#include "../../mainwindow.h"


UtilitiesView::UtilitiesView(QWidget *parent) : QWidget(parent),ui(new Ui::UtilitiesView){
    ui->setupUi(this);
    ui->timeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->statisticsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    statsUpdate = new QTimer;
    statsUpdate->setInterval(3000);
    connect(statsUpdate, SIGNAL(timeout()), this, SLOT(updateStats()));

    lastTime = -1;
    lastTimeIndex = -1;
    currentLap = 0;
    getLap = getWay = false;

    connect(ui->outChannelCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(loadOutChannelInfo(int)));
    connect(ui->channelList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(addToStatistics(QModelIndex)));
    connect(ui->statisticsTable, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(removeFromStatistics(QModelIndex)));

    connect(MainWindow::gsm, SIGNAL(callStatusUpdate(int,int,int,QString)),
            this, SLOT(handleCall(int,int,int,QString)));
    connect(MainWindow::gsm, SIGNAL(callMessage(QString)), this, SLOT(handleCallMsg(QString)));

}

void UtilitiesView::setup(int raceTrack){
    //Caricamento tabella tempi
    QSqlQuery q(MainWindow::database);
    q.prepare("SELECT COUNT(*) FROM GPSCoord WHERE CodCircuito = :id");
    q.bindValue(":id",raceTrack);
    q.exec();
    q.next();

    QStringList header;
    header<<"Trascorso"<<"Giro"<<"Gap";
    for(int i=0; i<q.value(0).toInt(); i++){
        header<<QString("Inter. %1").arg(i+1);
    }
    ui->timeTable->setColumnCount(header.count());
    ui->timeTable->setHorizontalHeaderLabels(header);

    GPSSettings settings;
    timeChannel = settings.getSpecialChannel(TIME_CHANNEL);
    lapChannel = settings.getSpecialChannel(LAP_CHANNEL);
    waypointChannel = settings.getSpecialChannel(WAYPOINT_CHANNEL);

    //Caricamento tabella statistiche
    QSqlQueryModel *columnModel = new QSqlQueryModel;
    columnModel->setQuery("SELECT CanID, Nome FROM Canale "
                          "WHERE (CodTipoDato=1 OR CodTipoDato=2) AND (CodTipoIO=1 OR CodTipoIO=3)",
                          MainWindow::database
                          );
    ui->channelList->setModel(columnModel);
    ui->channelList->setModelColumn(1);

    //Caricamento invio msg CAN
    QSqlQueryModel *outputModel = new QSqlQueryModel;
    outputModel->setQuery("SELECT CanID, Nome FROM Canale WHERE (CodTipoIO=2 OR CodTipoIO=3)",
                          MainWindow::database
                          );
    ui->outChannelCombo->setModel(outputModel);
    ui->outChannelCombo->setModelColumn(1);

    statsUpdate->start();

    //Caricamento chiamata team
    //Member::lastCallID = 0;
    QListWidgetItem *item;
    q.exec("SELECT Cognome, Nome, Ruolo,Tel FROM Team ORDER BY Ruolo,Cognome,Nome");
    ui->teamList->clear();
    teamMembers.clear();
    while(q.next()){
        item = new QListWidgetItem(
                    QIcon(":/images/icons/nocall.png"),
                    QString("%1 %2 (%3)")
                        .arg(q.value(0).toString()).arg(q.value(1).toString()).arg(q.value(2).toString()),
                    ui->teamList
                    );
        ui->teamList->addItem(item);
        teamMembers.append(Member(-1,CALL_DISCONNECT,q.value(3).toString()));
    }

}

//Call method
void UtilitiesView::on_startCallButton_clicked(){
    int index = ui->teamList->currentIndex().row();
    Member m;
    if(MainWindow::gsm->getState() > GSMInterface::GSMState::BRING_UP_WIRELESS){
        if(index>=0){
            m = teamMembers.at(index);
            if(m.status == CALL_DISCONNECT){
                MainWindow::gsm->execExternalCmd(QString("STARTCALL%1;").arg(m.number));
                ui->callErrorLabel->setText("In chiamata");
            }
        }else{
            ui->callErrorLabel->setText("<font color='yellow'>Nessun numero selezionato</font>");
        }
    }else{
        ui->callErrorLabel->setText("<font color='red'>Modulo non pronto</font>");
    }
}

void UtilitiesView::on_endCallButton_clicked(){
    MainWindow::gsm->execExternalCmd("ENDCALL");
    ui->callErrorLabel->setText("Chiamata chiusa");
}

void UtilitiesView::on_soloCall_clicked(){
    int index = ui->teamList->currentIndex().row();
    Member m;
    if(index !=-1){
        m = teamMembers.at(index);
        if(m.callID != -1 ){
            MainWindow::gsm->execExternalCmd(QString("SOLOCALL%1").arg(m.callID));
            ui->callErrorLabel->setText("Chiamata singola");
        }
    }
}

void UtilitiesView::on_groupCallButton_clicked(){
    MainWindow::gsm->execExternalCmd("GROUPCALL");
    ui->callErrorLabel->setText("Chiamata di gruppo");
}

void UtilitiesView::handleCall(int ID, int from, int status, QString num){
    Member m;
    int i=0;
    qDebug()<<"UTILITIES_VIEW: Handling call "<<ID<<" "<<status<<"  "<<num;
    while(i<teamMembers.count() && teamMembers.at(i).number.compare(num)!=0){
        i++;
    }

    if(i<teamMembers.count()){
        m = teamMembers.value(i);
        m.status = (CallStatus)status;
        if(m.status == CALL_DISCONNECT){
            m.callID = -1;
        }else{
            m.callID = ID;
        }
        teamMembers.replace(i,m);
        ui->teamList->item(i)->setIcon(getStatusIcon(m.status));
    }

    if(from && status == CALL_ACTIVE){
        QMessageBox *msg = new QMessageBox(this);
        msg->setStandardButtons(QMessageBox::Ok);
        msg->setAttribute(Qt::WA_DeleteOnClose);
        msg->setIcon(QMessageBox::Information);
        msg->setWindowModality(Qt::NonModal);
        msg->setWindowTitle("Informazione");
        msg->setModal(false);

        QSqlQuery q(MainWindow::database);
        q.prepare("SELECT Cognome, Nome FROM Team WHERE Tel = :num");
        q.bindValue(":num", num);
        q.exec();

        if(q.next()){
            msg->setText(QString("Chiamata in arrivo da %1 %2")
                         .arg(q.value(0).toString()).arg(q.value(1).toString()));
        }else{
            msg->setText("Chiamata in arrivo da numero sconosciuto");
        }
        msg->exec();
    }
}

void UtilitiesView::handleCallMsg(QString msg){
    if(msg.contains("BUSY")){
        ui->callErrorLabel->setText("<font color='red'>Numero occupato</font>");
    }else if(msg.contains("NO CARRIER")){
        ui->callErrorLabel->setText("<font color='red'>Numero disconnesso</font>");
    }else if(msg.contains("ERROR")){
        ui->callErrorLabel->setText("<font color='red'>Errore sconosciuto</font>");
    }
    on_endCallButton_clicked();
}

QIcon UtilitiesView::getStatusIcon(CallStatus status){
    switch (status) {
        case CALL_ACTIVE:
            return QIcon(":/images/icons/solocall.png");
            break;
        case CALL_DISCONNECT:
            return QIcon(":/images/icons/nocall.png");
            break;
        case CALL_HELD:
            return QIcon(":/images/icons/holdcall.png");
            break;
    }
    return QIcon(":/images/icons/loadcall.png");
}

//Time table methods
void UtilitiesView::onChannelUpdate(int ID, unsigned int packetIndex, QVariant value){
    if(ID == timeChannel){
        lastTime = value.toDouble();
        lastTimeIndex = packetIndex;

        if(getLap){
            addLapTime();
            getLap = false;
        }else if(getWay){
            addWayPointTime();
            getWay = false;
        }

    }else if(ID == lapChannel){
        if(lastTimeIndex == packetIndex){
            addLapTime();
        }else{
            getLap = true;
        }

    }else if(ID == waypointChannel){
        lastWayPoint = value.toInt();
        if(lastTimeIndex == packetIndex){
            addWayPointTime();
        }else{
            getWay = true;
        }
    }

}

void UtilitiesView::addLapTime(){
    QTime currentTime(0,0,0,0), prevTime(0,0,0,0), prevRoundTime(0,0,0,0), roundTime(0,0,0,0), gap(0,0,0,0);

    if(currentLap>0){
        prevTime = QTime::fromString(ui->timeTable->item(currentLap-1,0)->text(),"mm:ss:zzz");
        prevRoundTime = QTime::fromString(ui->timeTable->item(currentLap-1,1)->text(),"mm:ss:zzz");
    }

    ui->timeTable->insertRow(currentLap);

    currentTime = currentTime.addMSecs(lastTime);
    ui->timeTable->setItem(currentLap, 0, new QTableWidgetItem(currentTime.toString("mm:ss:zzz")));

    roundTime = roundTime.addMSecs(prevTime.msecsTo(currentTime));
    ui->timeTable->setItem(currentLap, 1, new QTableWidgetItem(roundTime.toString("mm:ss:zzz")));

    gap = gap.addMSecs(prevRoundTime.msecsTo(roundTime));
    ui->timeTable->setItem(currentLap, 2, new QTableWidgetItem(gap.toString("ss:zzz")));


    currentLap++;
}

void UtilitiesView::addWayPointTime(){
    QTime currentTime(0,0,0,0);
    currentTime = currentTime.addMSecs(lastTime);
    ui->timeTable->setItem(currentLap, 3+lastWayPoint, new QTableWidgetItem(currentTime.toString("mm:ss:zzz")));

}

//CAN msg methods
void UtilitiesView::loadOutChannelInfo(int index){
    QString typeName,mask;
    int can = ui->outChannelCombo->model()->data(ui->outChannelCombo->model()->index(index,0)).toInt();
    ChannelConvParams *param = MainWindow::recordingSession->getOutChannelParams(can);

    if(param){
        ui->msgText->setText("");
        switch(param->dataType){
            case 0:
                typeName = "BitFlags";
                for(int i=0; i<param->dim; i++){
                    mask.append("bbbbbbbb");
                }
                ui->msgText->setInputMask(mask);
                ui->msgText->setValidator(0);
                break;
            case 1:
                typeName = "Numero con segno";
                ui->msgText->setInputMask("");
                ui->msgText->setValidator(new QDoubleValidator());
                break;
            case 2:
                typeName = "Numero senza segno";
                ui->msgText->setInputMask("");
                ui->msgText->setValidator(new QDoubleValidator(0,DBL_MAX,15));
                break;
            case 3:
                typeName = "Stringa";
                ui->msgText->setInputMask("");
                ui->msgText->setValidator(0);
                break;
        }
        ui->descLabel->setText(QString("Tipo: %1 Dim: %2").arg(typeName).arg(param->dim));
    }
}

void UtilitiesView::on_sendButton_clicked(){
    /*
    if(ui->outChannelCombo->currentIndex()!=-1){
        int index = ui->outChannelCombo->currentIndex();
        int can = ui->outChannelCombo->model()->data(ui->outChannelCombo->model()->index(index,0)).toInt();
        ChannelConvParams *param = MainWindow::recordingSession->getOutChannelParams(can);
        switch(param->dataType){
            case 0:
                //typeName = "BitFlags";

                break;
            case 1:
                //typeName = "Numero con segno";

                break;
            case 2:
                //typeName = "Numero senza segno";
                //unsigned long value = ui->msgText->text().toULong();


                break;
            case 3:
                MainWindow::gsm->execExternalCmd(QString("CAN%1%2%3\n")
                                                 .arg((char)can)
                                                 .arg((char)(can>>8))
                                                 .arg(ui->msgText->text()));
                break;
        }

    }
    */
}

//Stats methods
void UtilitiesView::addToStatistics(QModelIndex index){
    int can = ui->channelList->model()->data(ui->channelList->model()->index(index.row(),0)).toInt();
    QModelIndexList matches = ui->statisticsTable->model()->match(ui->channelList->model()->index(0,0), Qt::UserRole, can);

    if(matches.count() == 0){
        QTableWidgetItem *item = new QTableWidgetItem(ui->channelList->model()->data(index).toString());
        item->setData(Qt::UserRole, can);
        ui->statisticsTable->insertRow(ui->statisticsTable->rowCount());
        ui->statisticsTable->setItem(ui->statisticsTable->rowCount()-1, 0, item);
        for(int i=0; i<3; i++){
            ui->statisticsTable->setItem(ui->statisticsTable->rowCount()-1, 1+i, new QTableWidgetItem());
        }
    }
}

void UtilitiesView::removeFromStatistics(QModelIndex index){
    ui->statisticsTable->removeRow(index.row());
}

void UtilitiesView::updateStats(){
    int can;
    QSqlQuery q(MainWindow::database);
    for(int i=0; i<ui->statisticsTable->rowCount(); i++){
        can = ui->statisticsTable->model()->data(ui->statisticsTable->model()->index(i,0),Qt::UserRole).toInt();
        q.prepare("SELECT MIN(Valore), MAX(Valore), AVG(Valore) FROM Storico WHERE IDCan = :id AND IDGara = :idg");
        q.bindValue(":id",can);
        q.bindValue(":idg", RecordingSession::raceCod);
        q.exec();
        q.next();

        for(int j=0; j<3; j++){
            ui->statisticsTable->item(i, 1+j)->setText(q.value(j).toString());
        }
    }
}

UtilitiesView::~UtilitiesView(){
    statsUpdate->stop();
    delete statsUpdate;
    delete ui;
}

QWidget* UtilitiesView::getTimeTable(){return ui->timeTable;}

void UtilitiesView::resetWidgets(){
    ui->groupBox_3->layout()->addWidget(ui->timeTable);
}



