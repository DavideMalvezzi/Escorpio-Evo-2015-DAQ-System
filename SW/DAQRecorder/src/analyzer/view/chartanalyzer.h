#ifndef CHARTANALYZER_H
#define CHARTANALYZER_H

#include <QWidget>
#include "../../widget/qcustomplot.h"
#include "../channelvalues.h"
#include "../../widget/chart.h"

namespace Ui {
    class ChartAnalyzer;
}

class ChartAnalyzer : public QWidget{
    Q_OBJECT

public:
    explicit ChartAnalyzer(QWidget *parent = 0);
    void addChart(ChannelValues*channel);
    ~ChartAnalyzer();

private:
    static Qt::GlobalColor colors[6];
    Ui::ChartAnalyzer *ui;
    int currentColor, chartCount;
    bool eventFilter(QObject *, QEvent *);

private slots:
    void contexMenu(QPoint);
};

#endif // CHARTANALYZER_H
