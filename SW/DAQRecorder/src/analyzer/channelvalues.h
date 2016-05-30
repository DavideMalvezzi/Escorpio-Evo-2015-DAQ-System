#ifndef CHANNELVALUES_H
#define CHANNELVALUES_H

#include <QObject>
#include <QSqlQuery>
#include <QVector>

class ChannelValues : QObject{
    Q_OBJECT
public:
    ChannelValues(int raceID, QString name, int type, int max);
    void addValue(int index, QVariant v);
    QString getName(){return name;}
    int getType(){return type;}
    int getCount(){return max;}
    QVariant* getData(){return values;}
    ~ChannelValues();
private:
    int raceID, type, lastIndex, max;
    QString name;
    QVariant *values;


};

#endif // CHANNELVALUES_H
