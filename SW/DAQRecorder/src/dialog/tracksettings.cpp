#include "tracksettings.h"
#include "ui_tracksettings.h"

#include "../mainwindow.h"
#include <QSharedDataPointer>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlError>

TrackSettings::TrackSettings(QWidget *parent) : QDialog(parent), ui(new Ui::TrackSettings){
    ui->setupUi(this);

    //GPS widget
    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    MapGraphicsScene *scene = new MapGraphicsScene(this);
    view = new MapGraphicsView(scene,this);
    view->setGeometry(190,40,595,370);
    view->setTileSource(osmTiles);
    view->setZoomLevel(0);
    view->centerOn(0,0);
    //Contex menu
    QAction *addStart,*addEnd,*addCenter,*addWayPoint,*addCity;
    addStart = new QAction("Punto iniziale", &contexMenu);
    addEnd = new QAction("Punto finale", &contexMenu);
    addCenter = new QAction("Punto centrale", &contexMenu);
    addWayPoint = new QAction("Aggiungi waypoint", &contexMenu);
    addCity = new QAction("Salva città", &contexMenu);
    contexMenu.addAction(addStart);
    contexMenu.addAction(addEnd);
    contexMenu.addAction(addCenter);
    contexMenu.addAction(addWayPoint);
    contexMenu.addAction(addCity);

    connect(view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(addStart, SIGNAL(triggered()), this, SLOT(setStartPoint()));
    connect(addEnd, SIGNAL(triggered()), this, SLOT(setEndPoint()));
    connect(addCenter, SIGNAL(triggered()), this, SLOT(setCentralPoint()));
    connect(addWayPoint, SIGNAL(triggered()), this, SLOT(addWayPoint()));
    connect(addCity, SIGNAL(triggered()), this, SLOT(addCity()));

    ui->gpsLat->setValidator(new QDoubleValidator(-90, 90, 8));
    ui->gpsLon->setValidator(new QDoubleValidator(-180, 180, 8));
    connect(ui->centerMapButton, SIGNAL(clicked()), this, SLOT(centerMapOn()));

    //Caricamento waypoint
    startPoint = new FlagObject(FlagObject::StartPoint,true);
    connect(startPoint, SIGNAL(pointSelected()), this, SLOT(selectPoint()));

    endPoint = new FlagObject(FlagObject::EndPoint,true);
    connect(endPoint, SIGNAL(pointSelected()), this, SLOT(selectPoint()));

    centralPoint = new FlagObject(FlagObject::CenterPoint,true);
    centralPoint->setFlag(MapGraphicsObject::ObjectIsSelectable, false);

    cityPoint = new FlagObject(FlagObject::CityPoint,true);
    cityPoint->setFlag(MapGraphicsObject::ObjectIsSelectable, false);

    scene->addObject(startPoint);
    scene->addObject(endPoint);
    scene->addObject(centralPoint);
    scene->addObject(cityPoint);

    //Caricamento database
    wayPointsModel = new QSqlRelationalTableModel(this, MainWindow::database);
    wayPointsModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    wayPointsModel->setTable("GPSCoord");
    ui->wayTable->setModel(wayPointsModel);

    QSqlQueryModel *cityName = new QSqlQueryModel();
    cityName->setQuery("SELECT * FROM Città WHERE CodStato = 1", MainWindow::database);
    connect(ui->trackCity, SIGNAL(currentIndexChanged(int)), this, SLOT(selectTrackCityPoint()));
    ui->trackCity->setModel(cityName);
    ui->trackCity->setModelColumn(1);

    QSqlQueryModel *stateName = new QSqlQueryModel();
    stateName->setQuery("SELECT * FROM Stato", MainWindow::database);
    connect(ui->trackState, SIGNAL(currentIndexChanged(int)), this, SLOT(selectTrackState()));
    ui->trackState->setModel(stateName);
    ui->trackState->setModelColumn(1);
    ui->trackState->setCurrentIndex(0);

    QSqlQueryModel *tracksName = new QSqlQueryModel();
    tracksName->setQuery("SELECT * FROM Circuito", MainWindow::database);
    connect(ui->tracksCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(selectTrack()));
    ui->tracksCombo->setModel(tracksName);
    ui->tracksCombo->setModelColumn(1);

    //Mapping
    mapper = new QDataWidgetMapper(this);
    mapper->setModel(wayPointsModel);
    mapper->addMapping(ui->wayRadiusSpin, 3);
    mapper->addMapping(ui->wayDistanceSpin, 4);
    mapper->addMapping(ui->wayTimeSpin, 5);
    mapper->addMapping(ui->wayRifCheck, 6);

    connect(ui->wayTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            mapper, SLOT(setCurrentModelIndex(QModelIndex)));


    connect(ui->addButton, SIGNAL(clicked()), this, SLOT(addTrack()));
    connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeTrack()));
    connect(ui->trackModelButton, SIGNAL(clicked()), this, SLOT(selectTrackModel()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->OKButton, SIGNAL(clicked()), this, SLOT(save()));
    connect(ui->OKButton, SIGNAL(clicked()), this, SLOT(accept()));

}

void TrackSettings::reloadTracks(){
    QSqlQueryModel *tracksName = (QSqlQueryModel*)ui->tracksCombo->model();
    tracksName->setQuery("SELECT IDCircuito, Nome FROM Circuito", MainWindow::database);
}

void TrackSettings::reloadCities(){
    QSqlQueryModel *trackCity = (QSqlQueryModel*)ui->trackCity->model();
    trackCity->setQuery(QString("SELECT * FROM Città WHERE CodStato = %1")
                            .arg(ui->trackState->currentIndex()+1), MainWindow::database);
}

void TrackSettings::addCity(){
   QPointF pos = view->mapToScene(globalPos);
   QString cityName = QInputDialog::getText(this, "Nuova città", "Inserisci nome nuova città").trimmed();
   if(!cityName.isEmpty() && !cityName.isNull()){
       cityPoint->setPos(pos);
       QSqlQuery q(MainWindow::database);
       q.prepare("INSERT INTO Città(Nome, Lat, Lon, CodStato) VALUES(:name, :lat, :lon, :cods)");
       q.bindValue(":name", cityName);
       q.bindValue(":cods", ui->trackState->currentIndex()+1);
       q.bindValue(":lat", cityPoint->latitude());
       q.bindValue(":lon", cityPoint->longitude());
       q.exec();
       if(q.lastError().isValid()){
           QMessageBox::critical(this, "Errore", QString("Impossibile salvare!\n%1").arg(q.lastError().text()));
       }
       reloadCities();
       ui->trackCity->setCurrentIndex(ui->trackCity->count()-1);
   }
}

void TrackSettings::addTrack(){
    int lastTrackIndex;
    QSqlQueryModel *tracksName = (QSqlQueryModel*)ui->tracksCombo->model();
    QSqlQuery q(MainWindow::database);
    q.exec("INSERT INTO Circuito(Nome) VALUES('Nuovo Circuito')");
    reloadTracks();

    lastTrackIndex = tracksName->data(tracksName->index(ui->tracksCombo->count()-1, 0)).toInt();
    q.prepare("INSERT INTO GPSCoord(Lat,Lon,Ruolo,CodCircuito) VALUES(0,0,:role,:cod)");
    q.bindValue(":role",FlagObject::StartPoint);
    q.bindValue(":cod", lastTrackIndex);
    q.exec();

    q.prepare("INSERT INTO GPSCoord(Lat,Lon,Ruolo,CodCircuito) VALUES(0,0,:role,:cod)");
    q.bindValue(":role",FlagObject::EndPoint);
    q.bindValue(":cod", lastTrackIndex);
    q.exec();

    ui->tracksCombo->setCurrentIndex(ui->tracksCombo->count()-1);
}

void TrackSettings::removeTrack(){
    QSqlQueryModel *trackNames = (QSqlQueryModel*)ui->tracksCombo->model();
    int index = ui->tracksCombo->currentIndex();
    int id = trackNames->data(trackNames->index(index, 0)).toInt();

    QSqlQuery q(MainWindow::database);
    q.prepare("SELECT * FROM Gara WHERE CodCircuito=:id LIMIT 1");
    q.bindValue(":id", id);
    q.exec();

    if(q.next()){
        QMessageBox::critical(this, "Errore", "Impossibile eliminare il circuito poichè già in uso!");
    }else{
        q.prepare("DELETE FROM Circuito WHERE IDCircuito=:id");
        q.bindValue(":id", id);
        q.exec();
        reloadTracks();
        ui->tracksCombo->setCurrentIndex(0);
    }
}

void TrackSettings::save(){
    int trackIndex = ui->tracksCombo->currentIndex();
    //QSqlQueryModel *tracksName = (QSqlQueryModel*)ui->tracksCombo->model();
    //int trackIndex = ui->tracksCombo->currentIndex();
    //int trackID = tracksName->data(tracksName->index(trackIndex, 0)).toInt();

    if(trackIndex!=-1){
        QSqlQueryModel *tracksCity = (QSqlQueryModel*)ui->trackCity->model();
        int cityIndex = ui->trackCity->currentIndex();
        int cityID = tracksCity->data(tracksCity->index(cityIndex, 0)).toInt();

        QSqlQuery q(MainWindow::database);
        q.prepare(QString("UPDATE Circuito SET Nome=:nome, Modello=:mod, Descrizione=:desc, "
                          //"InizioLat=:ilat, InizioLon=:ilon, FineLat=:flat, FineLon=:flon, "
                          "CentroLat=:clat, CentroLon=:clon, CodCittà=:idC WHERE IDCircuito=:id"));

        q.bindValue(":id",currentTrack);
        q.bindValue(":nome", ui->trackName->text());
        q.bindValue(":mod",  ui->trackModel->text());
        q.bindValue(":desc", ui->trackDesc->toPlainText());
        q.bindValue(":clat", centralPoint->latitude());
        q.bindValue(":clon", centralPoint->longitude());
        q.bindValue(":idC", cityID);
        q.exec();
        if(q.lastError().isValid()){
            QMessageBox::critical(this, "Errore", QString("Impossibile salvare!\n%1").arg(q.lastError().text()));
        }

        wayPointsModel->database().transaction();
        if(wayPointsModel->submitAll()) {
            wayPointsModel->database().commit();
        }else{
            wayPointsModel->database().rollback();
            QMessageBox::critical(this, "Errore", QString("Errore durante il salvataggio: %1").arg(wayPointsModel->lastError().text()));
        }

        reloadTracks();
        ui->tracksCombo->setCurrentIndex(trackIndex);
    }
}

void TrackSettings::selectTrack(){
    QSqlQueryModel *tracksName = (QSqlQueryModel*)ui->tracksCombo->model();
    int trackIndex = ui->tracksCombo->currentIndex();

    if(trackIndex!=-1){
        currentTrack = tracksName->data(tracksName->index(trackIndex, 0)).toInt();
        int cityID, stateID;

        //Info circuito
        QSqlQuery q(MainWindow::database);
        q.prepare("SELECT * FROM Circuito WHERE IDCircuito=:id");
        q.bindValue(":id",currentTrack);
        q.exec();
        q.next();

        ui->trackName->setText(q.value(1).toString());
        ui->trackModel->setText(q.value(2).toString());
        ui->trackDesc->setPlainText(q.value(3).toString());
        centralPoint->setLatitude(q.value(4).toFloat());
        centralPoint->setLongitude(q.value(5).toFloat());
        cityID = q.value(6).toInt();

        //Selezione stato e città
        q.prepare("SELECT CodStato, Nome FROM Città WHERE IDCittà=:idC");
        q.bindValue(":idC", cityID);
        q.exec();
        q.next();
        stateID = q.value(0).toInt();


        //Caricamento waypoint
        for(int i=0; i<wayPoints.count(); i++){
            view->scene()->removeObject(wayPoints.at(i));
        }
        wayPoints.clear();

        wayPointsModel->setFilter(QString("CodCircuito = %1").arg(currentTrack));
        wayPointsModel->select();

        QSqlRecord record;
        int type;
        for(int i=0; i<wayPointsModel->rowCount(); i++){
            record = wayPointsModel->record(i);
            type = record.value(7).toInt();
            if(type == FlagObject::StartPoint){
               startPoint->setPos(QPointF(record.value(2).toFloat(), record.value(1).toFloat()));
               startPoint->setID(i);
            }else if(type == FlagObject::EndPoint){
               endPoint->setPos(QPointF(record.value(2).toFloat(), record.value(1).toFloat()));
               endPoint->setID(i);
            }else if(type == FlagObject::WayPoint){
                addWayPoint(i,record.value(1).toFloat(), record.value(2).toFloat());
            }
        }

        ui->trackState->setCurrentIndex(stateID-1);
        ui->trackCity->setCurrentIndex(ui->trackCity->findText(q.value(1).toString()));
        view->setZoomLevel(16);
        view->centerOn(centralPoint);
    }
}

void TrackSettings::selectPoint(){
    FlagObject *sender = (FlagObject*)QObject::sender();
    ui->wayTable->setCurrentIndex(ui->wayTable->model()->index(sender->getID(),0));
}

void TrackSettings::selectTrackState(){
    QSqlQueryModel *stateModel = (QSqlQueryModel*)ui->trackState->model();
    QSqlQueryModel *cityModel = (QSqlQueryModel*)ui->trackCity->model();
    int stateIndex = ui->trackState->currentIndex();
    int stateID = stateModel->data(stateModel->index(stateIndex, 0)).toInt();

    cityModel->setQuery(QString("SELECT * FROM Città WHERE CodStato = %1").arg(stateID), MainWindow::database);
    ui->trackCity->setModel(cityModel);
}

void TrackSettings::selectTrackCityPoint(){
    QSqlQueryModel *cityModel = (QSqlQueryModel*)ui->trackCity->model();
    cityPoint->setLatitude(cityModel->data(cityModel->index(ui->trackCity->currentIndex(),2)).toFloat());
    cityPoint->setLongitude(cityModel->data(cityModel->index(ui->trackCity->currentIndex(),3)).toFloat());
    view->centerOn(cityPoint);
}

void TrackSettings::selectTrackModel(){
    QFileDialog d(this);
    d.setFileMode(QFileDialog::ExistingFile);
    if(d.exec() == 1){
        ui->trackModel->setText(d.selectedFiles().first());
    }
}

void TrackSettings::showContextMenu(const QPoint& pos){
    globalPos = pos;
    contexMenu.exec(((QWidget*)(QObject::sender()))->mapToGlobal(pos));
}

void TrackSettings::setStartPoint(){
    QPointF pos = view->mapToScene(globalPos);
    startPoint->setPos(pos);
    ui->wayTable->setCurrentIndex(ui->wayTable->model()->index(startPoint->getID(),0));
    wayPointsModel->setData(wayPointsModel->index(startPoint->getID(),1),startPoint->latitude());
    wayPointsModel->setData(wayPointsModel->index(startPoint->getID(),2),startPoint->longitude());
}

void TrackSettings::setEndPoint(){
    QPointF pos = view->mapToScene(globalPos);
    endPoint->setPos(pos);
    ui->wayTable->setCurrentIndex(ui->wayTable->model()->index(endPoint->getID(),0));
    wayPointsModel->setData(wayPointsModel->index(endPoint->getID(),1),endPoint->latitude());
    wayPointsModel->setData(wayPointsModel->index(endPoint->getID(),2),endPoint->longitude());
}

void TrackSettings::setCentralPoint(){
    QPointF pos = view->mapToScene(globalPos);
    centralPoint->setPos(pos);
}

void TrackSettings::addWayPoint(){
    QPointF pos = view->mapToScene(globalPos);
    FlagObject *coord = new FlagObject(FlagObject::WayPoint,true);
    coord->setFlag(MapGraphicsObject::ObjectIsDeletable);
    coord->setPos(pos);
    coord->setID(wayPointsModel->rowCount());

    wayPoints.append(coord);
    view->scene()->addObject(coord);

    wayPointsModel->insertRow(wayPointsModel->rowCount());
    wayPointsModel->setData(wayPointsModel->index(coord->getID(),1),coord->latitude());
    wayPointsModel->setData(wayPointsModel->index(coord->getID(),2),coord->longitude());
    wayPointsModel->setData(wayPointsModel->index(coord->getID(),3),1);
    wayPointsModel->setData(wayPointsModel->index(coord->getID(),4),1);
    wayPointsModel->setData(wayPointsModel->index(coord->getID(),5),1);
    wayPointsModel->setData(wayPointsModel->index(coord->getID(),6),0);
    wayPointsModel->setData(wayPointsModel->index(coord->getID(),7),(int)FlagObject::WayPoint);
    wayPointsModel->setData(wayPointsModel->index(coord->getID(),8),currentTrack);

    connect(coord, SIGNAL(destroyed()), this, SLOT(removeWayPoint()));
    connect(coord, SIGNAL(pointSelected()), this, SLOT(selectPoint()));

}

void TrackSettings::addWayPoint(int ID, float lat, float lon){
    FlagObject *coord = new FlagObject(FlagObject::WayPoint,true);
    coord->setFlag(MapGraphicsObject::ObjectIsDeletable);
    coord->setLatitude(lat);
    coord->setLongitude(lon);
    coord->setID(ID);

    wayPoints.append(coord);
    view->scene()->addObject(coord);

    connect(coord, SIGNAL(destroyed()), this, SLOT(removeWayPoint()));
    connect(coord, SIGNAL(pointSelected()), this, SLOT(selectPoint()));
}

void TrackSettings::removeWayPoint(){
    FlagObject *sender = (FlagObject*)QObject::sender();
    wayPointsModel->removeRow(sender->getID());
    view->scene()->removeObject(sender);
    wayPoints.removeAt(wayPoints.indexOf(sender));
}

void TrackSettings::centerMapOn(){
    view->setZoomLevel(16);
    view->centerOn(ui->gpsLon->text().toFloat(), ui->gpsLat->text().toFloat());
}

int TrackSettings::exec(){
    if(ui->tracksCombo->count()>0){
        ui->tracksCombo->setCurrentIndex(0);
    }
    return QDialog::exec();
}

void TrackSettings::on_exportButton_clicked(){
    if(ui->tracksCombo->currentIndex()!=-1){
        QFileDialog d(this);
        d.setAcceptMode(QFileDialog::AcceptOpen);
        d.setOption(QFileDialog::ShowDirsOnly);
        d.setFileMode(QFileDialog::Directory);
        if(d.exec() == 1){
            QFile file(d.selectedFiles().first().append("/GpsConf.txt"));
            if(file.open(QIODevice::WriteOnly)){
                QTextStream stream(&file);
                QSqlRecord record;
                stream<<wayPointsModel->rowCount()<<endl;
                for(int i=0; i<wayPointsModel->rowCount(); i++){
                    record = wayPointsModel->record(i);
                    for(int i=1; i<7; i++){
                        stream<<record.value(i).toString()<<",";
                    }
                    stream<<endl;
                }
                file.close();
            }
        }
    }
}


TrackSettings::~TrackSettings(){
    delete mapper;
    delete wayPointsModel;
    delete view;
    delete ui;
}

