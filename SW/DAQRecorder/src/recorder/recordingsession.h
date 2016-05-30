#ifndef RECORDINGSESSION_H
#define RECORDINGSESSION_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRelationalTableModel>
#include "../connectors/gsminterface.h"

#include <QTime>

#define BITFLAG_CONV 0
#define SIGN_NUMBER_CONV 1
#define UNSIGN_NUMBER_CONV 2
#define STRING_CONV 3

#define XpA 1
#define XdA 2
#define XpAdB 3
#define XpAdBpC 4

typedef struct ChannelConvParams{
    double a, b, c;
    int dim, convType, dataType;
}ChannelConvParams;

class RecordingSession : public QObject{
    Q_OBJECT

public:
    static double lon[48],lat[48];

    static int raceCod,trackCod,vehicleCod;
    explicit RecordingSession(GSMInterface *,QObject *parent = 0);
    void stop();
    void startNew(QString name, QString desc, int trackCod, int vehicleCod);

    ~RecordingSession();
    ChannelConvParams* getInChannelParams(int can);
    ChannelConvParams* getOutChannelParams(int can);


private:
    QMap<int, ChannelConvParams*> *channelsConvParams;
    QMap<int, ChannelConvParams*> *outChannelsConvParams;
    QSqlRelationalTableModel *sqlModel;

    QTimer *saveTimer, *fakeDateSender;
    int i;

    QBitArray convertToBit(QByteArray& data, int index, ChannelConvParams *params);
    double convertToDouble(QByteArray& data, int index, ChannelConvParams *params);
    double convertToUDouble(QByteArray& data, int index, ChannelConvParams *params);
    QString convertToString(QByteArray& data, int index, ChannelConvParams *params);
    double engConversion(double value, ChannelConvParams *params);

    bool isHeader(QByteArray& data, QString header);


signals:
    void channelUpdate(int ID, unsigned int packetIndex, QVariant value);

public slots:
    void onDATPacketReceived(QByteArray);
    void save();
    void sendFakeData();

};

#endif // RECORDINGSESSION_H
