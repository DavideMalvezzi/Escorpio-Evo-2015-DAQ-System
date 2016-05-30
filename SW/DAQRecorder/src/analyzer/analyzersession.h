#ifndef ANALYZERSESSION_H
#define ANALYZERSESSION_H

#include <QObject>
#include <QTreeWidgetItem>
#include "channelvalues.h"
#include <QSqlQuery>

class AnalyzerSession : public QObject{
    Q_OBJECT
public:
    explicit AnalyzerSession(QObject *parent = 0);
    void startNew(QList<QTreeWidgetItem*> races);
    void stop();
    QMap<int, ChannelValues*> getChannelsValues(){return channelsValues;}
    ~AnalyzerSession();

private:
   QMap<int, ChannelValues*> channelsValues;



signals:

public slots:
};

#endif // ANALYZERSESSION_H
