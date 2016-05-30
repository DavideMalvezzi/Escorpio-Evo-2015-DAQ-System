#ifndef VIEWUTILITIES_H
#define VIEWUTILITIES_H

#include <QWidget>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QValidator>
#include <QIcon>
#include "../../connectors/gsminterface.h"

namespace Ui {
    class UtilitiesView;
}



class UtilitiesView : public QWidget{
    Q_OBJECT

private:
    Ui::UtilitiesView *ui;
    QTimer *statsUpdate;
    bool getLap, getWay;
    int currentLap, lastTimeIndex, lastWayPoint;
    int timeChannel, lapChannel, waypointChannel;
    double lastTime;

    enum CallStatus{
        CALL_ACTIVE = 0,
        CALL_HELD = 1,
        CALL_DIALING = 2,
        CALL_ALERTING = 3,
        CALL_INCOMING = 4,
        CALL_WAITING = 5,
        CALL_DISCONNECT = 6
    };

    class Member {
        public:
            Member(){
                this->callID = -1;
                this->status = CALL_DISCONNECT;
                this->number = "";
            }
            Member(int id, CallStatus status, QString num){
                this->callID = id;
                this->status = status;
                this->number = num;
            }

            int callID;
            CallStatus status;
            QString number;
    };

    QList<Member> teamMembers;

public:
    explicit UtilitiesView(QWidget *parent = 0);
    void setup(int raceTrack);
    QWidget* getTimeTable();
    //QWidget* getStatsTable();
    //QWidget* getCallManager();

    void resetWidgets();
    ~UtilitiesView();

public slots:
    void onChannelUpdate(int ID, unsigned int packetIndex, QVariant value);



private slots:
    void loadOutChannelInfo(int);
    void addToStatistics(QModelIndex);
    void removeFromStatistics(QModelIndex);
    void updateStats();

    void addLapTime();
    void addWayPointTime();

    void on_sendButton_clicked();

    void on_startCallButton_clicked();
    void on_endCallButton_clicked();
    void on_groupCallButton_clicked();
    void handleCall(int ID, int from, int status, QString num);
    void handleCallMsg(QString msg);
    QIcon getStatusIcon(CallStatus status);
    void on_soloCall_clicked();
};

#endif // VIEWUTILITIES_H
