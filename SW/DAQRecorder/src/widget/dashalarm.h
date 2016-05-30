#ifndef DASHALARM_H
#define DASHALARM_H

#include "dashwidget.h"
#include <QFileDialog>
#include <QMediaPlayer>
#include <QMessageBox>

class DashAlarm : public DashWidget{
    Q_OBJECT

public:
    enum OnCondition{
        Cond_GE = 0x0,
        Cond_LE = 0x1,
        Cond_EQ = 0x2,
        Cond_NE = 0x3,
        Cond_BITON = 0x4,
    };

    explicit DashAlarm();
    explicit DashAlarm(int index, int canID, QJsonObject json);
    QWidget* getSettingsWidget(int setting);
    void repaint(QPainter &painter);
    void updateValue(QVariant value);
    QString toJson();
    int type();
    DashWidget* getCopy();
    ~DashAlarm();

private:
    static QImage *icon;
    bool isActived;
    OnCondition condition;
    QString alarmSound, alarmText, alarmCondition;
    QMediaPlayer *player;

protected slots:
    void setCondition(int i);
    void setAlarmText(QString s);
    void setAlarmSound();
    void setAlarmCondition(QString s);

};

#endif // DASHALARM_H
