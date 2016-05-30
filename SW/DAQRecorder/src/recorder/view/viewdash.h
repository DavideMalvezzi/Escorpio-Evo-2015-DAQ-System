#ifndef VIEWDASH_H
#define VIEWDASH_H

#include <QWidget>
#include <QSqlRelationalTableModel>
#include <QSqlRecord>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include "../../widget/dashwidget.h"


namespace Ui {
    class DashView;
}

class DashView : public QWidget{
    Q_OBJECT

public:
    explicit DashView(QWidget *parent = 0);
    void setup();
    ~DashView();

private:
    Ui::DashView *ui;
    DashWidget* createWidget(int index, int canID, int type, QString config);
    QTimer *repaintTimer;
    unsigned int lastPacketIndex;

public slots:
    void onChannelUpdate(int ID, unsigned int packetIndex, QVariant value);
};

#endif // VIEWDASH_H
