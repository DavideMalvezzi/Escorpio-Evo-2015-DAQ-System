#ifndef CHARTVIEW_H
#define CHARTVIEW_H

#include <QWidget>
#include "../../widget/chart.h"

namespace Ui {
    class ChartView;
}

typedef struct ChartParams{
    int canID;
    QString name;
    QColor color;
}chartParams;

class ChartView : public QWidget{
    Q_OBJECT

public:
    explicit ChartView(QWidget *parent = 0);
    void setup();
    ~ChartView();

private:
    Ui::ChartView *ui;
    QMap<int, Chart*> *charts;
    QVector<QVector<ChartParams*>*> *viewsParams;
    QTimer *replotTimer;

public slots:
    void onChannelUpdate(int ID, unsigned int packetIndex, QVariant value);


private slots:
    void loadView();

    void on_buttonZoomXplus_clicked();
    void on_buttonZoomXminus_clicked();
};

#endif // CHARTVIEW_H
