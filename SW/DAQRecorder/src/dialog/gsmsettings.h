#ifndef GSMSETTINGS_H
#define GSMSETTINGS_H

#include <QDialog>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlRecord>


namespace Ui {
    class GSMSettings;
}

class GSMSettings : public QDialog{
    Q_OBJECT

public:
    enum Settings{
        APN_NAME = 0,
        APN_USER = 1,
        APN_PSW = 2,
        TCP_PORT = 3,
        UDP_PORT = 4,
        RECEIVER_NUM = 5,
        SENDER_NUM = 6,
        RECEIVER_PIN = 7,
        SENDER_PIN = 8,
        PACKET_PER_SECOND = 9,
        PACKET_TIMEOUT = 10,
        PUSH_TO_TALK_REQUEST = 11,
        PUSH_TO_TALK_STATUS = 12
    };
    Q_ENUMS(Settings)

    explicit GSMSettings(QWidget *parent = 0);
    QSqlTableModel *model;
    int exec();
    ~GSMSettings();

private:
    Ui::GSMSettings *ui;

private slots:
    void save();
    void saveAndClose();
    void exportConfig();
};

#endif // GSMSETTINGS_H
