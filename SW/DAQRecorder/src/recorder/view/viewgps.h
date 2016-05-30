#ifndef GPSVIEW_H
#define GPSVIEW_H

#include <QWidget>


#include "../../widget/flagobject.h"
#include "../../mapgraphics/MapGraphicsView.h"
#include "../../mapgraphics/MapGraphicsScene.h"
#include "../../mapgraphics/CircleObject.h"
#include "../../mapgraphics/tileSources/OSMTileSource.h"

namespace Ui {
    class GPSView;
}

class GPSView : public QWidget{
    Q_OBJECT

public:
    explicit GPSView(QWidget *parent = 0);
    void setup(int);
    void setZoom(int);
    ~GPSView();

public slots:
    void setCentered();
    void onChannelUpdate(int ID, unsigned int packetIndex, QVariant value);


private:
    unsigned int lastPacketIndex, latChannel, lonChannel;
    double centerLat,centerLon;
    Ui::GPSView *ui;
    //MapGraphicsView *GPSMap;
    //FlagObject *startPoint, *endPoint;
    CircleObject *carPoint;
    QList<FlagObject*> *wayPoints;
    //MapGraphicsView *view;

    CircleObject* getNewPoint(int size, QColor color);
    bool eventFilter(QObject *, QEvent *);
};

#endif // GPSVIEW_H
