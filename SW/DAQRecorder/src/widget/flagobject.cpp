#include "flagobject.h"
#include <QDebug>
#include <QKeyEvent>

FlagObject::FlagObject(FlagType type, bool sizeIsZoomInvariant, MapGraphicsObject *parent) :
    MapGraphicsObject(sizeIsZoomInvariant,parent)
{
    /*
    this->type = type;
    this->distance = 1;
    this->duration = 1;
    this->radius = 1;
    */

    this->setFlag(MapGraphicsObject::ObjectIsSelectable);
    this->setFlag(MapGraphicsObject::ObjectIsFocusable);

    if(type == FlagType::StartPoint){
        pix = QImage(":/images/icons/startpoint.png");
    }else if(type == FlagType::EndPoint){
        pix = QImage(":/images/icons/endpoint.png");
    }else if(type == FlagType::WayPoint){
        pix = QImage(":/images/icons/waypoint.png");
    }else if(type == FlagType::CityPoint){
        pix = QImage(":/images/icons/citypoint.png");
    }else if(type == FlagType::CenterPoint){
        pix = QImage(":/images/icons/centerpoint.png");
    }
}

FlagObject::~FlagObject(){

}

QRectF FlagObject::boundingRect() const{
    return QRectF(0,-pix.height(),
                  pix.width(),
                  pix.height());
}

void FlagObject::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setRenderHint(QPainter::Antialiasing,true);

    painter->drawImage(0,0,pix.mirrored());
    if(isSelected()){
        painter->setPen(QPen(Qt::black,2));
        painter->drawRect(0,0,pix.width(),pix.height());
    }
}


//protected
//virtual from MapGraphicsObject
void FlagObject::keyReleaseEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Delete) && flags() & ObjectIsDeletable)
    {
        this->deleteLater();
        event->accept();
    }
    else
        event->ignore();
}

void FlagObject::mousePressEvent(QGraphicsSceneMouseEvent *event){
    emit pointSelected();
    MapGraphicsObject::mousePressEvent(event);
}
