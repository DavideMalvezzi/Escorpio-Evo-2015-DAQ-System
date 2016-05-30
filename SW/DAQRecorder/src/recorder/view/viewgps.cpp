#include "viewgps.h"
#include "ui_viewgps.h"
#include "../../mainwindow.h"
#include "../../dialog/gpssettings.h"
#include <QSharedDataPointer>
#include <QSqlQuery>

GPSView::GPSView(QWidget *parent) : QWidget(parent), ui(new Ui::GPSView){
    ui->setupUi(this);

    QSharedPointer<OSMTileSource> osmTiles(new OSMTileSource(OSMTileSource::OSMTiles), &QObject::deleteLater);
    MapGraphicsScene *scene = new MapGraphicsScene(ui->GPSMap);
    //view = new MapGraphicsView(scene,ui->GPSMap);
    ui->GPSMap->setScene(scene);
    ui->GPSMap->setTileSource(osmTiles);
    ui->GPSMap->setZoomLevel(8);
    ui->GPSMap->centerOn(0,0);
    ui->GPSMap->installEventFilter(this);

    //startPoint = new FlagObject(FlagObject::StartPoint,true);
    //endPoint = new FlagObject(FlagObject::EndPoint,true);
    wayPoints = new QList<FlagObject*>;
    carPoint = getNewPoint(5, QColor(0,255,255));

    //scene->addObject(startPoint);
    //scene->addObject(endPoint);
    scene->addObject(carPoint);

}

void GPSView::setup(int raceTrack){
    MapGraphicsObject *temp;
    while(!wayPoints->isEmpty()){
        temp = wayPoints->takeFirst();
        ui->GPSMap->scene()->removeObject(temp);
        delete temp;
    }

    QSqlQuery q(MainWindow::database);
    q.prepare("SELECT * FROM Circuito WHERE IDCircuito = :id");
    q.bindValue(":id", raceTrack);
    q.exec();
    q.next();

    centerLon = q.value("CentroLon").toDouble();
    centerLat = q.value("CentroLat").toDouble();
    ui->GPSMap->centerOn(centerLon,centerLat);

    connect(ui->GPSMap, SIGNAL(zoomLevelChanged(quint8)), this, SLOT(setCentered()));
    ui->GPSMap->setZoomLevel(17);

    q.prepare("SELECT * FROM GPSCoord WHERE CodCircuito = :id");
    q.bindValue(":id", raceTrack);
    q.exec();
    while(q.next()){
        temp = new FlagObject((FlagObject::FlagType)q.value("Ruolo").toInt(),true);
        temp->setLatitude(q.value("Lat").toDouble());
        temp->setLongitude(q.value("Lon").toDouble());
        ui->GPSMap->scene()->addObject(temp);
    }
    lastPacketIndex = 0;

    GPSSettings settings;
    latChannel = settings.getSpecialChannel(LAT_CHANNEL);
    lonChannel = settings.getSpecialChannel(LON_CHANNEL);
}

void GPSView::onChannelUpdate(int ID, unsigned int packetIndex, QVariant value){
    if(packetIndex>=lastPacketIndex){
        lastPacketIndex = packetIndex;
        if(latChannel != -1 && lonChannel != -1){
            if(ID == latChannel){
                carPoint->setLatitude(value.toDouble());
            }else if(ID == lonChannel){
                carPoint->setLongitude(value.toDouble());
            }
        }
    }
}

bool GPSView::eventFilter(QObject *obj, QEvent *evt){
    if(obj == ui->GPSMap){
        if(evt->type() == QEvent::Resize){
            setCentered();
        }
    }
    return true;
}

CircleObject* GPSView::getNewPoint(int size, QColor color){
    CircleObject *point = new CircleObject(size, false, color);
    point->setFlag(MapGraphicsObject::ObjectIsSelectable, false);
    point->setFlag(MapGraphicsObject::ObjectIsMovable, false);
    point->setFlag(MapGraphicsObject::ObjectIsDeletable, false);
    return point;
}

void GPSView::setCentered(){
    ui->GPSMap->centerOn(centerLon,centerLat);
    ui->GPSMap->setDragMode(MapGraphicsView::NoDrag);
}

void GPSView::setZoom(int zoom){
    ui->GPSMap->setZoomLevel(zoom);
}

GPSView::~GPSView(){
    qDebug()<<"GPS_VIEW: Deleting";
    delete carPoint;
    delete wayPoints;
    delete ui;
}
