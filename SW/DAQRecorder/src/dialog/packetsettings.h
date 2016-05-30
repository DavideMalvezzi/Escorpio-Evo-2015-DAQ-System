#ifndef DIALOGPACKETSETTINGS_H
#define DIALOGPACKETSETTINGS_H

#include <QDialog>
#include <QSqlRelationalTableModel>
#include <QSqlRelationalDelegate>

namespace Ui {
    class PacketSettingsDialog;
}

class PacketSettingsDialog : public QDialog{
    Q_OBJECT

public:
    explicit PacketSettingsDialog(QWidget *parent = 0);
    ~PacketSettingsDialog();
    int exec();
    int getRecordNum();

private slots:
    void addRow();
    void saveAndClose();
    bool save();
    void cancelAndClose();
    void exportConfig();

private:
    Ui::PacketSettingsDialog *ui;
    QSqlRelationalTableModel *sqlModel;

};


class PacketSettingsDelegate : public QSqlRelationalDelegate{
    public:
        explicit PacketSettingsDelegate(QWidget *parent=0);
        QWidget* createEditor(QWidget *aParent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // DIALOGPACKETSETTINGS_H
