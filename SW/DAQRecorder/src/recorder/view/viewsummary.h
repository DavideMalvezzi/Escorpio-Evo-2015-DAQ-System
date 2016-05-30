#ifndef SUMMARYVIEW_H
#define SUMMARYVIEW_H

#include <QWidget>
#include "../../widget/customsubwin.h"

namespace Ui {
    class SummaryView;
}

class SummaryView : public QWidget{
    Q_OBJECT

public:
    CustomSubWin *chartSum, *gpsSum, *dashSum, *timeSum, *statsSum, *callSum;
    explicit SummaryView(QWidget *parent = 0);
    void setup();
    void prepare();
    ~SummaryView();

private:
    Ui::SummaryView *ui;
    void resizeEvent(QResizeEvent*);

};

#endif // SUMMARYVIEW_H
