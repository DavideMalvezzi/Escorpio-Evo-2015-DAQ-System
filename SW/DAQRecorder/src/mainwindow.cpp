#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSerialPortInfo>
#include <QSerialPort>
#include <QMessageBox>

#include <QSqlQueryModel>
#include <QSqlError>

#include <QNetworkProxy>

//Database
QSqlDatabase MainWindow::database = QSqlDatabase::addDatabase("QSQLITE", "EscorpioDB");

//Views
MainWindow *MainWindow::mainWindow = Q_NULLPTR;
LogConsole *MainWindow::logConsole = Q_NULLPTR;
RecordingSessionViews *MainWindow::recordingViews = Q_NULLPTR;
AnalyzerSessionViews *MainWindow::analyzerViews = Q_NULLPTR;

//Dialogs
/*
PacketSettingsDialog *MainWindow::packetSettingsDialog = Q_NULLPTR;
ChartViewSettings *MainWindow::chartsSettingsDialog = Q_NULLPTR;
DashSettingsDialog *MainWindow::dashSettingsDialog = Q_NULLPTR;
GPSSettings *MainWindow::gpsSettingsDialog = Q_NULLPTR;
TeamSettings *MainWindow::teamSettingsDialog = Q_NULLPTR;
VehicleSettings *MainWindow::vehicleSettingsDialog = Q_NULLPTR;
TrackSettings *MainWindow::trackSettingsDialog = Q_NULLPTR;
GSMSettings *MainWindow::gsmSettingsDialog = Q_NULLPTR;
AboutDialog *MainWindow::aboutDialog = Q_NULLPTR;
*/

//Session
RecordingSession *MainWindow::recordingSession = Q_NULLPTR;
AnalyzerSession *MainWindow::analyzerSession = Q_NULLPTR;
GSMInterface *MainWindow::gsm = new GSMInterface();

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
    QFontDatabase::addApplicationFont(":/fonts/DigitFont.ttf");


/*
    QFile f("C:\\Users\\user\\Desktop\\fake.txt");
    f.open(QFile::Text|QFile::WriteOnly);
    QTextStream stream(&f);
    for(uint i=0; i<10000; i++){
        QString n = QString::number(i,16);
        int sin = sinf(i)*100000;
        int cos = cosf(i)*100000;
        while(n.size()<4)n.prepend('0');

        stream<<"#0d#0a#44#41#54";
        stream<<"#"<<n.at(0)<<n.at(1);
        stream<<"#"<<n.at(2)<<n.at(3);
        stream<<"#00#02#01#00#01#01";

        QString c = QString::number(sin,16);
        while(c.size()>8)c = c.remove(0,1);
        while(c.size()<8)c = c.prepend('0');
        for(int j=0; j<8;j++){
            if(j%2==0)stream<<"#";
            stream<<c.at(8-j-1);
        }

        c = QString::number(cos,16);
        while(c.size()>8)c = c.remove(0,1);
        while(c.size()<8)c = c.prepend('0');
        for(int j=0; j<8;j++){
            if(j%2==0)stream<<"#";
            stream<<c.at(8-j-1);
        }


        stream<<"#0d#0a\n";
    }
    f.close();


    */
    /*
    QNetworkProxy proxy;
    proxy.setType(QNetworkProxy::HttpProxy);
    proxy.setHostName("192.168.0.1");
    proxy.setPort(8080);
    QNetworkProxy::setApplicationProxy(proxy);
*/

    //Puntatori globali alle istanze delle view da usare per accedere ad esse da classi esterne
    this->mainWindow = this;
    this->logConsole = ui->logConsoleTable;
    this->recordingViews = ui->recordingSessionViews;
    this->analyzerViews = ui->analyzerSessionViews;
    //this->aboutDialog = new AboutDialog(this);

    //Refresh porte seriali
    on_buttonRefreshPort_clicked();

    //gsm = new GSMInterface();
    recordingSession = new RecordingSession(gsm, this);
    analyzerSession = new AnalyzerSession(this);
    ui->comboTrack->setModel(new QSqlQueryModel());
    ui->comboVehicle->setModel(new QSqlQueryModel());

    connect(ui->showGraficiInSommario, SIGNAL(toggled(bool)), recordingViews->summaryView->chartSum, SLOT(setVisible(bool)));
    connect(recordingViews->summaryView->chartSum, SIGNAL(onClose(bool)), ui->showGraficiInSommario, SLOT(setChecked(bool)));

    connect(ui->showCruscottoInSommario, SIGNAL(toggled(bool)), recordingViews->summaryView->dashSum, SLOT(setVisible(bool)));
    connect(recordingViews->summaryView->dashSum, SIGNAL(onClose(bool)), ui->showCruscottoInSommario, SLOT(setChecked(bool)));

    connect(ui->showGPSInSommario, SIGNAL(toggled(bool)), recordingViews->summaryView->gpsSum, SLOT(setVisible(bool)));
    connect(recordingViews->summaryView->gpsSum, SIGNAL(onClose(bool)), ui->showGPSInSommario, SLOT(setChecked(bool)));

    connect(ui->showTimerInSommario, SIGNAL(toggled(bool)), recordingViews->summaryView->timeSum, SLOT(setVisible(bool)));
    connect(recordingViews->summaryView->timeSum, SIGNAL(onClose(bool)), ui->showTimerInSommario, SLOT(setChecked(bool)));

    connect(ui->actionChiudi_Recorder, SIGNAL(triggered()), this, SLOT(closeRecorder()));

    connect(gsm, SIGNAL(writeOnConsole(QString)), logConsole, SLOT(addToLog(QString)));
}

void MainWindow::raceTreeItemClicked(QTreeWidgetItem *item){
   QVariant raceID = item->data(0, Qt::UserRole);
   if(raceID.isValid()){
       QSqlQuery q(database);
       q.prepare(QString("SELECT Gara.Nome, Gara.Data, Gara.OraInizio, Gara.OraFine, Gara.Descrizione, Veicolo.Nome, Circuito.Nome "
                         "FROM (Gara JOIN Veicolo ON IDVeicolo = CodVeicolo) JOIN Circuito ON IDCircuito = CodCircuito "
                         "WHERE Gara.IDGara = :id"));
       q.bindValue(":id", raceID.toInt());
       q.exec();
       q.next();

       QTime begin, end, duration(0,0,0,0);
       begin = QTime::fromString(q.value(2).toString(), "hh:mm:ss");
       end = QTime::fromString(q.value(3).toString(), "hh:mm:ss");
       duration = duration.addSecs(begin.secsTo(end));

       ui->raceNameLabel->setText(q.value(0).toString());
       ui->raceDateLabel->setText(q.value(1).toString());
       ui->raceStartTimelabel->setText(q.value(2).toString());
       ui->raceDurationLabel->setText(duration.toString("hh:mm:ss"));
       ui->raceDescLabel->setPlainText(q.value(4).toString());
       ui->raceVehicleLabel->setText(q.value(5).toString());
       ui->raceTrackLabel->setText(q.value(6).toString());

       q.prepare("SELECT COUNT(*) FROM Storico WHERE IDGara = :id");
       q.bindValue(":id", raceID.toInt());
       q.exec();
       q.next();
       ui->raceDataLabel->setText(q.value(0).toString());

   }else{
       ui->raceNameLabel->setText("");
       ui->raceNameLabel->setText("");
       ui->raceDateLabel->setText("");
       ui->raceStartTimelabel->setText("");
       ui->raceDurationLabel->setText("");
       ui->raceDescLabel->setPlainText("");
       ui->raceVehicleLabel->setText("");
       ui->raceTrackLabel->setText("");
       ui->raceDataLabel->setText("");
   }
}

void MainWindow::on_buttonRefreshPort_clicked(){
    ui->comboPortName->clear();
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    for(int i=0; i<portList.size(); i++){
        ui->comboPortName->addItem(portList.at(i).portName());
    }
}

void MainWindow::refreshTrackAndVehicle(){
    int tindex = ui->comboTrack->currentIndex();
    int vindex = ui->comboVehicle->currentIndex();
    QSqlQueryModel *trackModel = (QSqlQueryModel*)ui->comboTrack->model();
    QSqlQueryModel *vehicleModel = (QSqlQueryModel*)ui->comboVehicle->model();
    trackModel->setQuery("SELECT IDCircuito, Nome FROM Circuito", MainWindow::database);
    vehicleModel->setQuery("SELECT IDVeicolo, Nome FROM Veicolo", MainWindow::database);
    ui->comboTrack->setModelColumn(1);
    ui->comboVehicle->setModelColumn(1);
    ui->comboTrack->setCurrentIndex(tindex);
    ui->comboVehicle->setCurrentIndex(vindex);

}

void MainWindow::on_buttonOpenCom_clicked(){
    QSqlQueryModel *trackModel = (QSqlQueryModel*)ui->comboTrack->model();
    QSqlQueryModel *vehicleModel = (QSqlQueryModel*)ui->comboVehicle->model();
    if(ui->comboPortName->count()>0){
        if(ui->comboTrack->currentIndex()!=-1){
            if(ui->comboVehicle->currentIndex()!=-1){
                if(!ui->newRaceNameText->text().trimmed().isEmpty()){
                   if(gsm->open(ui->comboPortName->currentText())){

                        logConsole->addToLog(LogConsole::INFO, "Porta seriale aperta correttamente");
                        ui->mainPanel->setCurrentIndex(1);
                        ui->menuFile->setEnabled(false);
                        ui->menuRecorder->setEnabled(true);
                        ui->menuSommario->setEnabled(true);
                        ui->menuImpostazioni->setEnabled(false);

                        recordingSession->startNew(
                                    ui->newRaceNameText->text(),
                                    ui->raceDescText->toPlainText(),
                                    trackModel->data(trackModel->index(ui->comboTrack->currentIndex(),0)).toInt(),
                                    vehicleModel->data(vehicleModel->index(ui->comboTrack->currentIndex(),0)).toInt()
                        );
                        showMaximized();
                        recordingViews->setup();


                    }else{
                        QMessageBox::critical(this, "Errore", gsm->getSerial()->errorString());
                        logConsole->addToLog(LogConsole::ERRORS, gsm->getSerial()->errorString());

                    }
                }else{
                    QMessageBox::critical(this, "Errore", "Il nome della gara non può essere vuoto!");
                }
            }else{
                QMessageBox::critical(this, "Errore", "Nessun veicolo selezionato!");
                logConsole->addToLog(LogConsole::ERRORS, "Nessun veicolo selezionato!");
            }
        }else{
            QMessageBox::critical(this, "Errore", "Nessun circuito selezionato!");
            logConsole->addToLog(LogConsole::ERRORS, "Nessuna circuito selezionato!");
        }
    }else{
      QMessageBox::critical(this, "Errore", "Impossibile trovare un dispositivo di ricezione!");
      logConsole->addToLog(LogConsole::ERRORS, "Impossibile trovare un dispositivo di ricezione!");
    }

}

void MainWindow::on_buttonOpemAnalyzer_clicked(){
    QList<QTreeWidgetItem*> selectedRaces;
    QTreeWidgetItem *races = ui->raceTree->topLevelItem(0);
    for(int i=0; i<races->childCount(); i++){
        if(races->child(i)->checkState(0) == Qt::Checked){
            selectedRaces.append(races->child(i));
        }
    }

    if(selectedRaces.size()){
        analyzerSession->startNew(selectedRaces);
        ui->mainPanel->setCurrentIndex(2);
        showMaximized();
        analyzerViews->setup();
    }else{
        QMessageBox::critical(this, "Errore", "Nessuna gara selezionata!");
        logConsole->addToLog(LogConsole::ERRORS, "Nessuna gara selezionata!");
    }
}

//Menù impostazioni
void MainWindow::on_actionChannelsSettings_triggered(){
    PacketSettingsDialog d;
    d.exec();
    //packetSettingsDialog->exec();
}

void MainWindow::on_actionChartSettings_triggered(){
    PacketSettingsDialog p;
    if(p.getRecordNum()>0){
        ChartViewSettings d;
        d.exec();
    }else{
        QMessageBox::critical(this, "Errore", "Nessun canale disponibile!");
        p.exec();
    }
    /*
    if(packetSettingsDialog->getRecordNum()>0){
        chartsSettingsDialog->exec();
    }else{
        QMessageBox::critical(this, "Errore", "Nessun canale disponibile!");
        packetSettingsDialog->exec();
    }
    */
}

void MainWindow::on_actionWidgetSettings_triggered(){
    DashSettingsDialog d;
    d.exec();
    /*
    if(packetSettingsDialog->getRecordNum()>0){
        dashSettingsDialog->exec();
    }else{
        QMessageBox::critical(this, "Errore", "Nessun canale disponibile!");
        packetSettingsDialog->exec();
    }
    */
}

void MainWindow::on_actionGPSSettings_triggered(){
    PacketSettingsDialog p;
    if(p.getRecordNum()>0){
        GPSSettings d;
        d.exec();
    }else{
        QMessageBox::critical(this, "Errore", "Nessun canale disponibile!");
        p.exec();
    }

    /*
    if(packetSettingsDialog->getRecordNum()>0){
        gpsSettingsDialog->exec();
    }else{
        QMessageBox::critical(this, "Errore", "Nessun canale disponibile!");
        packetSettingsDialog->exec();
    }
    */
}

void MainWindow::on_actionTeam_triggered(){
    TeamSettings d;
    d.exec();
    //teamSettingsDialog->exec();
}

void MainWindow::on_actionVehicleSettings_triggered(){
    VehicleSettings d;
    d.exec();
    //vehicleSettingsDialog->exec();
    refreshTrackAndVehicle();
}

void MainWindow::on_actionTrackSettings_triggered(){
    TrackSettings d;
    d.exec();
    //trackSettingsDialog->exec();
    refreshTrackAndVehicle();
}

void MainWindow::on_actionGSM_triggered(){
    GSMSettings d;
    d.exec();
    //gsmSettingsDialog->exec();
}

void MainWindow::on_actionInformazioni_triggered(){
    AboutDialog d;
    d.exec();
    //aboutDialog->exec();
}

//Barra dei menù
void MainWindow::on_actionApri_triggered(){
    QFileDialog d(this);
    d.setFileMode(QFileDialog::ExistingFile);
    d.setNameFilter("Database (*.sqlite)");
    if(d.exec()){
        loadDB(d.selectedFiles().first());
    }
}

void MainWindow::on_actionNuovo_triggered(){
    QString url = QFileDialog::getSaveFileName(this,"Salva","","Database (*.sqlite)");
    if(!url.isEmpty()){
        if(database.isOpen()){
            database.close();
        }

        database.setDatabaseName(url);

        if(!database.open()){
            QMessageBox::critical(this, "Errore", "Impossibile aprire il database");
        }else{
            DatabaseCreator *dbCreator = new DatabaseCreator(url);
            dbCreator->start();
            //dbCreator.wait();
            //loadDB(url);
        }
    }

}

void MainWindow::on_actionExportCSV_triggered(){
   if(ui->raceTree->model()->rowCount()>0){
       QList<int> *selectedRaces = new QList<int>;
       QTreeWidgetItem* races = ui->raceTree->topLevelItem(0);
       for(int i=0; i<races->childCount(); i++){
            if(races->child(i)->checkState(0) == Qt::Checked){
                selectedRaces->append(races->child(i)->data(0, Qt::UserRole).toInt());
            }
       }
       if(selectedRaces->count()>0){
           QFileDialog d(this);
           d.setAcceptMode(QFileDialog::AcceptOpen);
           d.setOption(QFileDialog::ShowDirsOnly);
           d.setFileMode(QFileDialog::Directory);
           if(d.exec()==1){
               QSqlQuery q(database);
               QString fileName;
               QDate date;
               QFile *file;
               int index;
               for(int i=0; i<selectedRaces->count(); i++){
                   q.prepare("SELECT * FROM Gara WHERE IDGara=:id");
                   q.bindValue(":id", selectedRaces->at(i));
                   q.exec();
                   q.next();
                   date = q.value(2).toDate();
                   fileName = QString("/%1-%2-%3_%4.csv").arg(date.year()).arg(date.month()).arg(date.day()).arg(q.value(1).toString());
                   file = new QFile(d.selectedFiles().first().append(fileName));
                   if (file->open(QIODevice::WriteOnly)){
                       QTextStream stream(file);
                       q.prepare("SELECT * FROM (SELECT DISTINCT IDCan FROM Storico WHERE IDGara=:id) INNER JOIN (SELECT CanID, Nome FROM Canale) ON IDCan = CanID");
                       q.bindValue(":id", selectedRaces->at(i));
                       q.exec();
                       stream<<"Index;";
                       while(q.next()){
                           stream<<q.value("Nome").toString()<<"(0x"<<QString::number(q.value("CanID").toInt(),16)<<");";
                       }
                       stream<<endl;

                       q.prepare("SELECT * FROM Storico WHERE IDGara=:id ORDER BY Indice, IDCan ASC");
                       q.bindValue(":id", selectedRaces->at(i));
                       q.exec();
                       q.next();
                       index = q.value("Indice").toInt();
                       stream<<index<<";";
                       do{
                           if(index!=q.value("Indice").toInt()){
                               stream<<endl;
                               index = q.value("Indice").toInt();
                               stream<<index<<";";
                           }
                           stream<<q.value("Valore").toString()<<";";
                       }while(q.next());

                       file->close();
                   }
               }
           }
       }else{
           QMessageBox::critical(this, "Errore", "Non è stata selezionata nessuna gara da esportare!");
       }
   }else{
       QMessageBox::critical(this, "Errore", "Nessun database aperto da cui esportare!");
   }
}

void MainWindow::on_actionEsci_triggered(){
    exit(0);
}

void MainWindow::loadDB(QString name){
    if(database.isOpen()){
         ui->menuImpostazioni->setEnabled(false);
         /*
         if(packetSettingsDialog)delete packetSettingsDialog;
         if(chartsSettingsDialog)delete chartsSettingsDialog;
         if(dashSettingsDialog)delete dashSettingsDialog;
         if(gpsSettingsDialog)delete gpsSettingsDialog;
         if(teamSettingsDialog)delete teamSettingsDialog;
         if(vehicleSettingsDialog)delete vehicleSettingsDialog;
         if(trackSettingsDialog)delete trackSettingsDialog;
         if(gsmSettingsDialog)delete gsmSettingsDialog;
*/
         database.close();
    }
    database.setDatabaseName(name);
    //Caricamento DB e creazione dialog delle impostazioni
    if(!database.open()){
        QMessageBox::critical(this, "Errore", "Impossibile aprire il database");
    }else{
        ui->menuImpostazioni->setEnabled(true);
        QSqlQuery q(database);
        q.exec("PRAGMA foreign_keys=ON;");

        /*
        this->packetSettingsDialog = new PacketSettingsDialog();
        this->chartsSettingsDialog = new ChartViewSettings();
        this->dashSettingsDialog = new DashSettingsDialog();
        this->gpsSettingsDialog = new GPSSettings();
        this->teamSettingsDialog = new TeamSettings();
        this->vehicleSettingsDialog = new VehicleSettings();
        this->trackSettingsDialog = new TrackSettings();
        this->gsmSettingsDialog = new GSMSettings();
        */

        q.exec("SELECT IDGara,Nome FROM Gara");

        ui->raceTree->clear();

        QTreeWidgetItem* races = new QTreeWidgetItem(ui->raceTree);
        races->setText(0, "Gare");
        connect(ui->raceTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
                 this, SLOT(raceTreeItemClicked(QTreeWidgetItem*)));


        ui->raceTree->addTopLevelItem(races);
        ui->raceNameLabel->setText("");
        ui->raceNameLabel->setText("");
        ui->raceDateLabel->setText("");
        ui->raceStartTimelabel->setText("");
        ui->raceDurationLabel->setText("");
        ui->raceDescLabel->setPlainText("");
        ui->raceVehicleLabel->setText("");
        ui->raceTrackLabel->setText("");

        QTreeWidgetItem* race;
        while(q.next()){
            race = new QTreeWidgetItem();
            race->setData(0,Qt::UserRole, q.value(0).toInt());
            race->setText(0, q.value(1).toString());
            race->setFlags(race->flags()|Qt::ItemIsUserCheckable);
            race->setCheckState(0, Qt::Unchecked);
            races->addChild(race);
        }

        ui->raceTree->expandAll();
        refreshTrackAndVehicle();
        ui->comboTrack->setCurrentIndex(0);
        ui->comboVehicle->setCurrentIndex(0);
    }
}

void MainWindow::closeRecorder(){
    recordingSession->stop();
    gsm->close();
    ui->mainPanel->setCurrentIndex(0);
    ui->menuFile->setEnabled(true);
    ui->menuRecorder->setEnabled(false);
    ui->menuImpostazioni->setEnabled(true);

    //TODO: aggiungere alla treeview la gara appena registrata
}

void MainWindow::on_actionAvvia_registrazione_triggered(){
    if(gsm->getState()>=GSMInterface::CONNECTED){
        if(gsm->isDATStarted()){
            ui->actionAvvia_registrazione->setText("Inizia registrazione");
            ui->actionAvvia_registrazione->setIcon(QIcon(":/images/icons/record.png"));
            gsm->execExternalCmd("ENDDAT");
        }else{
            ui->actionAvvia_registrazione->setText("Fine registrazione");
            ui->actionAvvia_registrazione->setIcon(QIcon(":/images/icons/stop.png"));
            gsm->execExternalCmd("STARTDAT");
        }
    }
}

void MainWindow::on_actionRiprendi_registrazione_triggered(){
    QSettings lastUsedIP("config.ini", QSettings::IniFormat);
    QString lastIP = lastUsedIP.value("GSMCONFIG/lastIP").toString();
    gsm->setCurrentIP(lastIP);
}

MainWindow::~MainWindow(){
    if(database.isOpen()){
        database.close();
    }

    delete recordingSession;
    /*
    delete packetSettingsDialog;
    delete chartsSettingsDialog;
    delete dashSettingsDialog;
    delete gpsSettingsDialog;
    delete teamSettingsDialog;
    delete vehicleSettingsDialog;
    delete trackSettingsDialog;
    delete gsmSettingsDialog;
    */
    delete gsm;
    delete ui;

}

