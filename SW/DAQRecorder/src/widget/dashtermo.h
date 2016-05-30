#ifndef DASHTERMO_H
#define DASHTERMO_H

#include "dashwidget.h"

class DashTermo : public DashWidget{
    Q_OBJECT
public:
    explicit DashTermo();
    explicit DashTermo(int index, int canID, QJsonObject json);
    ~DashTermo();

    QWidget* getSettingsWidget(int setting);
    void repaint(QPainter &painter);
    void updateValue(QVariant value);
    QString toJson();
    int type();
    DashWidget* getCopy();

protected:
    Qt::Orientation orientation;
    float value;
    int value1,value2, tick;
    QColor color;

protected slots:
   void setOrientation(int);
   void setValue(int);
   void setValue1(int);
   void setValue2(int);
   void setTick(int);
   void setColor();
};

#endif // DASHTERMO_H
