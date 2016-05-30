#include "chart.h"

Chart::Chart(QCustomPlot* plotter){
    rect = new QCPAxisRect(plotter);
    xAxis = rect->axis(QCPAxis::atBottom);
    yAxis = rect->axis(QCPAxis::atLeft);
    graph = new QCPGraph(xAxis,yAxis);

    xAxis->setRange(0,100);
    xAxis->setVisible(true);
    xAxis->grid()->setVisible(true);
    xAxis->grid()->setSubGridVisible(true);
    xAxis->setTickLabelColor(Qt::white);

    yAxis->setRange(-0.1, 0.1);
    //yAxis->setAntialiased(true);
    yAxis->setBasePen(QPen(Qt::white, 1));
    yAxis->setLabelColor(Qt::white);
    yAxis->setTickLabelColor(Qt::white);
    yAxis->setTickPen(QPen(QColor(140,140,140,255)));
    yAxis->setSubTickPen(QPen(QColor(120,120,120,255)));
    yAxis->setAutoTickStep(true);
    yAxis->setAutoTickCount(3);
    yAxis->setAutoSubTicks(true);
    //yAxis->grid()->setAntialiased(true);
    //yAxis->grid()->setAntialiasedSubGrid(true);
    //yAxis->grid()->setSubGridVisible(true);
    yAxis->grid()->setPen(QPen(QColor(140,140,140,255),1, Qt::DotLine));
    yAxis->grid()->setSubGridPen(QPen(QColor(120,120,120,255),1, Qt::DotLine));

    yAxis->setLabelFont(QFont("Helvetica", 8));
    yAxis->setLabelPadding(0);

    rect->setAutoMargins(QCP::msNone);
    rect->setMargins(QMargins(50,-10,0,15));
    rect->setRangeZoom(Qt::Horizontal);
    rect->setRangeDrag(Qt::Horizontal);

    //graph->setAntialiased(true);
    graph->setScatterStyle(QCPScatterStyle::ssNone);

    connect(xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(onXRangeChanged(QCPRange)));
}

void Chart::addData(double key, double value, bool autoScroll){
    float rangeX = xAxis->range().upper - xAxis->range().lower;
    float rangeY = qMax(yAxis->range().upper,value) - qMin(yAxis->range().lower,value);
    graph->addData(key, value);

    //AutoScroll
    if(autoScroll){
        //is last value out of the plot?
        if(xAxis->range().upper < key){
            xAxis->setRangeUpper(qCeil(key));
            xAxis->setRangeLower(qFloor(key - rangeX));
        }

    }
    //is value in y-positive range?
    if(yAxis->range().upper < value*1.5){
        yAxis->setRangeUpper(qCeil(value+rangeY/10));
    }
    //is value in y-negative range?
    if(yAxis->range().lower > value*1.5){
        yAxis->setRangeLower(qFloor(value-rangeY/10));
    }

}

void Chart::hideXaxis(bool show){
    xAxis->setBasePen(show ? QPen(Qt::transparent, 0) : QPen(Qt::white, 1));
    xAxis->setTickPen(show ? QPen(Qt::transparent, 0) : QPen(Qt::white, 1));
    xAxis->setSubTickPen(show ? QPen(Qt::transparent, 0) : QPen(Qt::white, 1));
    xAxis->grid()->setZeroLinePen(show ? QPen(Qt::transparent, 0) : QPen(Qt::white, 1));
    xAxis->setTickLabels(!show);
}

void Chart::setChartName(QString name){
    yAxis->setLabel(name);
}

void Chart::setPen(QPen pen){
    graph->setPen(pen);
}

void Chart::setVisible(bool visible){
    rect->setVisible(visible);
}

void Chart::onXRangeChanged(QCPRange range){
    Q_UNUSED(range)
    if(xAxis->range().lower<0){
        xAxis->setRangeLower(0);
    }
}

void Chart::zoomInX(){
    xAxis->setRange(xAxis->range().lower, xAxis->range().upper / 1.5f);
}

void Chart::zoomOutX(){
    xAxis->setRange(xAxis->range().lower, xAxis->range().upper * 1.5f);
}

void Chart::zoomInY(){
    double delta,center;
    delta = yAxis->range().upper - yAxis->range().lower;
    center = delta/2;

    yAxis->setRange(center - delta/2 / 1.5f, center + delta/2 / 1.5f);
}

void Chart::zoomOutY(){
    double delta,center;
    delta = yAxis->range().upper - yAxis->range().lower;
    center = delta/2;

    yAxis->setRange(center - delta/2 * 1.5f, center + delta/2 * 1.5f);
}

Chart::~Chart(){

}
