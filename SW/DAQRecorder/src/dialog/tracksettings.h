#ifndef TRACKSETTINGS_H
#define TRACKSETTINGS_H

#include <QDialog>
#include <QMenu>
#include <QPoint>
#include <QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include "../widget/flagobject.h"
#include "../mapgraphics/MapGraphicsScene.h"
#include "../mapgraphics/MapGraphicsView.h"
#include "../mapgraphics/tileSources/OSMTileSource.h"


namespace Ui {
    class TrackSettings;
}

class TrackSettings : public QDialog{
    Q_OBJECT

public:
    explicit TrackSettings(QWidget *parent = 0);
    ~TrackSettings();
    int exec();

private slots:
    void reloadTracks();
    void reloadCities();
    void addCity();
    void addTrack();
    void removeTrack();
    void save();

    void selectPoint();
    void selectTrack();
    void selectTrackModel();
    void selectTrackState();
    void selectTrackCityPoint();
    void showContextMenu(const QPoint& pos);

    void setStartPoint();
    void setEndPoint();
    void setCentralPoint();
    void addWayPoint();
    void addWayPoint(int,float,float);
    void removeWayPoint();

    void centerMapOn();

    void on_exportButton_clicked();

private:
    Ui::TrackSettings *ui;
    QMenu contexMenu;
    MapGraphicsView *view;
    QPoint globalPos;
    FlagObject *startPoint, *endPoint, *centralPoint, *cityPoint;
    QList<FlagObject*> wayPoints;
    QSqlRelationalTableModel *wayPointsModel;
    QDataWidgetMapper *mapper;
    int currentTrack;

};

#endif // TRACKSETTINGS_H
