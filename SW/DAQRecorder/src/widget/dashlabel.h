#ifndef DASHLABEL_H
#define DASHLABEL_H

#include "dashwidget.h"
#include <QFontDialog>
#include <QLineEdit>
#include <QToolButton>
#include <QBitArray>

class DashLabel : public DashWidget{
   Q_OBJECT
public:
    explicit DashLabel();
    explicit DashLabel(int index, int canID, QJsonObject json);
    QWidget* getSettingsWidget(int setting);
    void repaint(QPainter &painter);
    void updateValue(QVariant value);
    QString toJson();
    int type();
    DashWidget* getCopy();
    ~DashLabel();

private slots:
    void setText(QString text);
    void setFont();
    void recalculateSize();
protected:
    QString text;
    QFont font;
};

#endif // DASHLABEL_H
