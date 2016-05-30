#include "analyzersession.h"
#include "../mainwindow.h"

AnalyzerSession::AnalyzerSession(QObject *parent) : QObject(parent){

}

void AnalyzerSession::startNew(QList<QTreeWidgetItem*> races){
    int raceID = races.first()->data(0, Qt::UserRole).toInt();
    int maxIndex;
    QSqlQuery q(MainWindow::database);

    qDebug()<<"ANALYZER_SESSION: Loading race "<<raceID;
    q.prepare("SELECT MAX(Indice) FROM Storico WHERE IDGara = :id ORDER BY Indice ASC");
    q.bindValue(":id",raceID);
    q.exec();
    q.next();
    maxIndex = q.value(0).toInt();

    qDebug()<<"ANALYZER_SESSION: Race data count "<<maxIndex;
    q.prepare("SELECT DISTINCT IDCan, Nome, CodTipoDato FROM Storico JOIN Canale ON IDCan = CanID WHERE IDGara = :id");
    q.bindValue(":id",raceID);
    q.exec();
    while(q.next()){
        channelsValues.insert(
                    q.value(0).toInt(),
                    new ChannelValues(raceID, q.value(1).toString(), q.value(2).toInt(), maxIndex));
        qDebug()<<"ANALYZER_SESSION: Loading channel "
                <<q.value(0).toInt()<<"  "
                <<q.value(1).toString()<<"  "
                <<q.value(2).toInt();
    }

    q.prepare("SELECT IDCan, Indice, Valore FROM Storico WHERE IDGara = :id ORDER BY IDCan,Indice ASC");
    q.bindValue(":id",raceID);
    q.exec();
    while(q.next()){
        channelsValues.value(q.value(0).toInt())->addValue(q.value(1).toInt(), q.value(2));
    }

}
void AnalyzerSession::stop(){
    for(int i=0; i<channelsValues.values().size(); i++){
        delete channelsValues.values().at(i);
    }
}

AnalyzerSession::~AnalyzerSession(){
    qDebug()<<"ANALYZER_SESSION: Deleting";
    stop();
}

