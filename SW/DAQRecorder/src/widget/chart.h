#ifndef CHART_H
#define CHART_H

#include "qcustomplot.h"

class Chart : public QObject{
    Q_OBJECT
private:
    QCPAxisRect *rect;
    QCPAxis *xAxis, *yAxis;
    QCPGraph *graph;

public:
    explicit Chart(QCustomPlot* plotter);
    void setChartName(QString name);
    void setVisible(bool);
    void setPen(QPen pen);
    void hideXaxis(bool);

    QCPAxisRect *rectAxis(){return rect;}
    ~Chart();

public slots:
    void addData(double x, double y, bool autoscroll);
    void onXRangeChanged(QCPRange);

    void zoomInX();
    void zoomOutX();
    void zoomInY();
    void zoomOutY();

};

#endif // CHART_H
