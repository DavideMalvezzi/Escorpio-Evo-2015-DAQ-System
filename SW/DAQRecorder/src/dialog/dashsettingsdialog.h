#ifndef DASHSETTINGSDIALOG_H
#define DASHSETTINGSDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QSqlRelationalTableModel>
#include <QTreeWidgetItem>
#include "../widget/dashwidget.h"
#include "../widget/dashpainter.h"
#include "../widget/dashled.h"
#include "../widget/dashlabel.h"
#include "../widget/dashsegment.h"
#include "../widget/dashspeedometer.h"
#include "../widget/dashbar.h"
#include "../widget/dashtermo.h"
#include "../widget/dashalarm.h"

#define LED 1
#define LABEL 2
#define DIGIT 3
#define BAR 4
#define ANGOLMETER 5
#define THERMOMETER 6
#define ALARM 7


namespace Ui {
    class DashSettingsDialog;
}

class DashSettingsDialog : public QDialog{
    Q_OBJECT

public:
    explicit DashSettingsDialog(QWidget *parent = 0);
    ~DashSettingsDialog();

private slots:
    DashWidget* createWidget(int index, int canID, int type, QString config);
    void loadWidgetSettings(DashWidget*);
    void selectWidget(QTreeWidgetItem*);
    void deleteWidget(int index);
    bool save();
    void reject();

    void on_addLabelButton_clicked();
    void on_addLedButton_clicked();
    void on_addDigitButton_clicked();
    void on_addAngularButton_clicked();
    void on_addBarButton_clicked();
    void on_addTermoButton_clicked();

    void on_addAlarmButton_clicked();

private:
    Ui::DashSettingsDialog *ui;
    QSqlRelationalTableModel *sqlModel;

    bool eventFilter(QObject *, QEvent *);
};

#endif // DASHSETTINGSDIALOG_H
