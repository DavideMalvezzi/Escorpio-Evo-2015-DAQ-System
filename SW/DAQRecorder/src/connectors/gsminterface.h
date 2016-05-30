#ifndef GSMINTERFACE_H
#define GSMINTERFACE_H

#include <QThread>
#include <QSerialPort>
#include <QTimer>
#include <QVector>
#include <QByteArray>

#define PACKET_MAX_SIZE 256

class GSMInterface : public QObject{
    Q_OBJECT
public:

    enum GSMState{
        BLOCK,
        CHECK_MODULE_ON,
        CHECK_SIM_PIN,
        CHECK_GSM_CONNECTION,
        RESET_NETWORK,
        SETUP_NETWORK_SETTINGS,
        BRING_UP_WIRELESS,
        GET_CURRENT_IP,
        START_TCP_CONNECTION,
        START_UDP_CONNECTION,
        VOICE_CALL,
        CONNECTED
    };

    GSMInterface();
    ~GSMInterface();


    bool open(QString);
    void close();
    void execExternalCmd(QString cmd);

    QSerialPort* getSerial(){return port;}
    GSMState getState(){return currentState;}
    bool isDATStarted(){return DATStarted;}
    void setCurrentIP(QString IP){
        writeOnConsole("WARNRicaricando impostazioni sessione precendente...");
        serverIP = IP;
        if(currentState == START_TCP_CONNECTION){
            setState(START_TCP_CONNECTION);
        }
    }

private:
    QSerialPort *port;
    QByteArray RXbuffer;
    QVector<QByteArray> RXmsg, TXmsg;
    QVector<QString> CMDmsg;

    bool isTextMode, DATStarted;
    QTimer responseTimer;
    GSMState currentState,prevState;

    //Settings
    int UDPport, TCPport, responseCount, sendCount;
    QString pin, apn, user, psw, serverIP, clientIP;

private slots:
    void setState(GSMState);

    void addRXmsg(QByteArray);
    void addTXmsg(QByteArray);

    void request();
    void response();

    void sendCmd(QString, int, int=1);
    void sendPacket();

    void writeOnSerial(QByteArray);
    void readFromSerial();

    void loadSettings();

    void processExternalCommand();

signals:
    void writeOnConsole(QString);
    void dataPacketReceived(QByteArray);
    void callStatusUpdate(int,int,int,QString);
    void callMessage(QString);

};

#endif // GSMINTERFACE_H
