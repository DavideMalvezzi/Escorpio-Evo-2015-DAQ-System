#ifndef TRACKPLOTTER_H
#define TRACKPLOTTER_H

#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QWheelEvent>

class TrackPlotter : public QWidget{
    Q_OBJECT

public:
    explicit TrackPlotter(QWidget *parent = 0);
    ~TrackPlotter();

protected:
    float zoom;
    QPointF max,min;
    QVector<QPointF> point;
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *);


signals:

public slots:
};

#endif // TRACKPLOTTER_H
