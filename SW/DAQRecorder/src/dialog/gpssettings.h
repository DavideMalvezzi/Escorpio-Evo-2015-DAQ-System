#ifndef GPSSETTINGS_H
#define GPSSETTINGS_H

#define SPECIAL_CHANNELS 6
#define LAT_CHANNEL 0
#define LON_CHANNEL 1
#define LAP_CHANNEL 2
#define SPACE_CHANNEL 3
#define TIME_CHANNEL 4
#define WAYPOINT_CHANNEL 5


#include <QDialog>
#include <QSqlQuery>
#include <QSqlTableModel>

namespace Ui {
    class GPSSettings;
}

class GPSSettings : public QDialog{
    Q_OBJECT

public:
    explicit GPSSettings(QWidget *parent = 0);
    static int getSpecialChannel(int channel);
    int exec();
    ~GPSSettings();
private slots:
    void save();
    void loadSpecialChannels();

private:
    Ui::GPSSettings *ui;
    QSqlTableModel *sqlModel;
    static int *channels;
};

#endif // GPSSETTINGS_H
