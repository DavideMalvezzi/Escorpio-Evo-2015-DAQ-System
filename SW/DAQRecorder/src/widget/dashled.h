#ifndef DASHLED_H
#define DASHLED_H

#include "dashwidget.h"
#include <QToolButton>
#include <QColorDialog>
#include <QJsonDocument>
#include <QBitArray>

class DashLed : public DashWidget{
    Q_OBJECT

public:
    enum OnCondition{
        Cond_GE = 0x0,
        Cond_LE = 0x1,
        Cond_EQ = 0x2,
        Cond_NE = 0x3,
        Cond_BITON = 0x4,
    };

    explicit DashLed();
    explicit DashLed(int index, int canID, QJsonObject json);
    QWidget* getSettingsWidget(int setting);
    void repaint(QPainter &painter);
    void updateValue(QVariant value);
    QString toJson();
    int type();
    DashWidget* getCopy();
    ~DashLed();
protected:
    OnCondition condition;
    QString condValue;
    QColor onColor1,onColor2,offColor1,offColor2;

public slots:
    void setColor();
    void setCondition(int);
    void setValue(QString);
};

#endif // DASHLED_H
