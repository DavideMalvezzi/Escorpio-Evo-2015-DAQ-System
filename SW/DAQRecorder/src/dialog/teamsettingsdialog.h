#ifndef TEAMSETTINGSDIALOG_H
#define TEAMSETTINGSDIALOG_H

#include <QDialog>
#include <QDataWidgetMapper>
#include <QSqlTableModel>

namespace Ui{
    class TeamSettings;
}

class TeamSettings : public QDialog{
    Q_OBJECT

public:
    explicit TeamSettings(QWidget *parent = 0);
    int exec();
    ~TeamSettings();

private slots:
    void on_addButton_clicked();

    void on_removeButton_clicked();

    void on_OKbutton_clicked();

private:
    Ui::TeamSettings *ui;
    QSqlTableModel *sqlModel;
    QDataWidgetMapper *mapper;
};

#endif // TEAMSETTINGSDIALOG_H
