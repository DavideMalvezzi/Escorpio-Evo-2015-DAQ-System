#ifndef RECORDINGSESSIONVIEWS_H
#define RECORDINGSESSIONVIEWS_H

#include <QWidget>
#include "view/viewsummary.h"
#include "view/viewchart.h"
#include "view/viewdash.h"
#include "view/viewgps.h"
#include "view/viewutilities.h"

namespace Ui{
    class RecordingSessionViews;
}

class RecordingSessionViews : public QWidget{
    Q_OBJECT

public:
    static SummaryView *summaryView;
    static ChartView *chartView;
    static DashView *dashView;
    static GPSView *gpsView;
    static UtilitiesView *utilitiesView;

    explicit RecordingSessionViews(QWidget *parent = 0);
    ~RecordingSessionViews();
    void setup();

private slots:
    void reassignWidget(int);

private:
    Ui::RecordingSessionViews *ui;
};

#endif // RECORDINGSESSIONVIEWS_H
