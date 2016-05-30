#ifndef CHARTVIEWSETTINGS_H
#define CHARTVIEWSETTINGS_H

#include <QDialog>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>


namespace Ui {
    class ChartViewSettings;
}

class ChartViewSettings : public QDialog{
    Q_OBJECT

public:
    explicit ChartViewSettings(QWidget *parent = 0);
    ~ChartViewSettings();
    int exec();

private:
    Ui::ChartViewSettings *ui;
    QSqlRelationalTableModel *sqlModel;
    int currentWindowID, defaultCanID;
    void reloadWindow();


private slots:
    void addWindow();
    void removeWindow();
    void selectWindow();
    void addChart();
    void removeChart();
    void saveAndClose();
    void cancelAndClose();
    bool save();

};

class ChartViewSettingsDelegate : public QSqlRelationalDelegate{
    Q_OBJECT

public:
    ChartViewSettingsDelegate(QWidget *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
    QWidget* createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // CHARTVIEWSETTINGS_H
