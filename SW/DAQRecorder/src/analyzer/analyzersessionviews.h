#ifndef ANALYZERSESSIONVIEWS_H
#define ANALYZERSESSIONVIEWS_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
    class AnalyzerSessionViews;
}

class AnalyzerSessionViews : public QWidget{
    Q_OBJECT

public:
    explicit AnalyzerSessionViews(QWidget *parent = 0);
    void setup();
    ~AnalyzerSessionViews();

private:
    Ui::AnalyzerSessionViews *ui;

private slots:
    void addToChart(QListWidgetItem*);
};

#endif // ANALYZERSESSIONVIEWS_H
