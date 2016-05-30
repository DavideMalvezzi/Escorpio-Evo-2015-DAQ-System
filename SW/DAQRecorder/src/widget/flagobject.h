#ifndef FLAGOBJECT_H
#define FLAGOBJECT_H

#include "../mapgraphics/MapGraphicsObject.h"
#include <QObject>
#include <QKeyEvent>

class FlagObject : public MapGraphicsObject{
    Q_OBJECT
public:
    enum FlagType{
        WayPoint = 1,
        StartPoint = 2,
        EndPoint = 3,
        CityPoint = 4,
        CenterPoint = 5
    };
    explicit FlagObject(FlagType,bool,MapGraphicsObject *parent = 0);
    virtual ~FlagObject();

    //pure-virtual from MapGraphicsObject
    QRectF boundingRect() const;

    //pure-virtual from MapGraphicsObject
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    /*
    void setRadius(float r){this->radius = r;}
    void setDuration(float d){this->duration = d;}
    void setDistance(float d){this->distance = d;}

    float getRadius(){return radius;}
    float getDuration(){return duration;}
    float getDistance(){return distance;}
    */
    int getType(){return type;}
    int getID(){return ID;}
    void setID(int ID){this->ID = ID;}



signals:
    void customContextMenuRequested(const QPoint &pos);
    void pointSelected();

public slots:

protected:
    //virtual from MapGraphicsObject
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    QImage pix;
    FlagType type;
    int ID;
    //float radius,duration,distance;
};

#endif // FLAGOBJECT_H
