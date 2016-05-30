#include "trackplotter.h"
#include <QFile>
#include "../mapgraphics/tileSources/CompositeTileSource.h"
#include <qmath.h>

TrackPlotter::TrackPlotter(QWidget *parent) : QWidget(parent){
    QFile lat("C:\\Users\\user\\Desktop\\lat.txt");
    QFile lon("C:\\Users\\user\\Desktop\\lon.txt");
    lat.open(QFile::ReadOnly);
    lon.open(QFile::ReadOnly);

    for(int i=0; i<100000;i++){
        point.append(QPointF(
                          180 + QString(lon.readLine()).toFloat(),
                          90 - QString(lat.readLine()).toFloat()
                        ));
    }

    max = min = point.first();

    for(int i=0; i<point.size(); i++){
        if(max.x()<point.at(i).x()){
            max.setX(point.at(i).x());
        }
        if(max.y()<point.at(i).y()){
            max.setY(point.at(i).y());
        }
        if(min.x()>point.at(i).x()){
            min.setX(point.at(i).x());
        }
        if(min.y()>point.at(i).y()){
            min.setY(point.at(i).y());
        }
    }

    zoom = 1;
}

void TrackPlotter::paintEvent(QPaintEvent *evt){
    Q_UNUSED(evt);
    QPainter painter(this);
    painter.setPen(QPen(QColor(Qt::red),3));
    painter.setRenderHint(QPainter::Antialiasing);
    float w,h;
    float x,y,x2,y2;


    w = 320*zoom;
    h = 480*zoom;

    painter.translate((width()-w)/2,(height()-h)/2);

    for(int i=1; i<point.size(); i++){
        x = (point[i-1].x()-min.x())/(max.x()-min.x()) * w;
        y = (point[i-1].y()-min.y())/(max.y()-min.y()) * h;
        x2 = (point[i].x()-min.x())/(max.x()-min.x()) * w;
        y2 = (point[i].y()-min.y())/(max.y()-min.y()) * h;
        painter.drawLine(QLineF(x,y,x2,y2));
    }

}

void TrackPlotter::wheelEvent(QWheelEvent *evt){
    zoom+= evt->angleDelta().y()/120*0.1f;
    if(zoom<0.3f)zoom=0.3f;
    if(zoom>5)zoom=5;

    repaint();
}


TrackPlotter::~TrackPlotter(){

}

