#ifndef DASHBAR_H
#define DASHBAR_H

#include "dashwidget.h"

class DashBar : public DashWidget{
    Q_OBJECT
 public:
     explicit DashBar();
     explicit DashBar(int index, int canID, QJsonObject json);
     QWidget* getSettingsWidget(int setting);
     void repaint(QPainter &painter);
     void updateValue(QVariant value);
     QString toJson();
     int type();
     DashWidget* getCopy();
     ~DashBar();
protected:
     Qt::Orientation orientation;
     float value;
     int value1,value2;

protected slots:
    void setOrientation(int);
    void setValue(int);
    void setValue1(int);
    void setValue2(int);

};

#endif // DASHBAR_H
