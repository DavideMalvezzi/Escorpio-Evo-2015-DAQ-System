#ifndef DASHSPEEDOMETER_H
#define DASHSPEEDOMETER_H

#include "dashwidget.h"
#include <QtMath>
#include <QPainterPath>

class DashSpeedometer : public DashWidget{
    Q_OBJECT
public:
    DashSpeedometer();
    DashSpeedometer(int index, int canID, QJsonObject json);
    void repaint(QPainter &painter);
    QString toJson();
    void updateValue(QVariant value);
    QWidget* getSettingsWidget(int setting);
    DashWidget* getCopy();
    int type();
    ~DashSpeedometer();

protected:
    float value;
    int fontSize,tick,startAngle,arcSize,value1,value2;

private slots:
    void setWidth(int w);
    void setFontSize(int t);
    void setTick(int t);
    void setStartAngle(int angle);
    void setArcLenght(int len);
    void setValue(float v);
    void setValue1(int v);
    void setValue2(int v);
};

#endif // DASHSPEEDOMETER_H
