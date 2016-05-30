#ifndef DASHSEGMENT_H
#define DASHSEGMENT_H

#include "dashwidget.h"
#include <QFontDatabase>
#include <QColorDialog>
#include <QToolButton>


class DashSegment : public DashWidget{
    Q_OBJECT
public:
    enum DigitType{
        DecimalMode = 0x0,
        TimeMode = 0x1
    };

    explicit DashSegment();
    explicit DashSegment(int index, int canID, QJsonObject json);
    QWidget* getSettingsWidget(int setting);
    void repaint(QPainter &painter);
    void updateValue(QVariant value);
    int type();
    DashWidget* getCopy();
    QString toJson();
    ~DashSegment();

protected:
    int digit,decimalDigit;
    QColor color;
    DigitType mode;
    QFont font;
    QString text;

    void recalculateSize();
    void reloadValue();

private slots:
    void setMode(int type);
    void setDigitCount(int count);
    void setDecimalCount(int count);
    void setDigitSize(int size);
    void setColor();
    void setText(QString);

};

#endif // DASHSEGMENT_H
