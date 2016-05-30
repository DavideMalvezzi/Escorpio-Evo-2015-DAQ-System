#include "recordingsession.h"
#include "../mainwindow.h"
#include <QSqlRecord>

int RecordingSession::raceCod = -1;
int RecordingSession::trackCod = -1;
int RecordingSession::vehicleCod = -1;


RecordingSession::RecordingSession(GSMInterface *gsm, QObject *parent) : QObject(parent){
    channelsConvParams = new QMap<int,ChannelConvParams*>;
    outChannelsConvParams = new QMap<int,ChannelConvParams*>;

    sqlModel = new QSqlRelationalTableModel(this, MainWindow::database);
    sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    sqlModel->setTable("Storico");

    saveTimer = new QTimer();
    saveTimer->setInterval(10000);
    connect(saveTimer, SIGNAL(timeout()), this, SLOT(save()));


    connect(gsm, SIGNAL(dataPacketReceived(QByteArray)), this, SLOT(onDATPacketReceived(QByteArray)));

    connect(this, SIGNAL(channelUpdate(int,unsigned int,QVariant)),
            RecordingSessionViews::chartView, SLOT(onChannelUpdate(int,unsigned int,QVariant)));

    connect(this, SIGNAL(channelUpdate(int,unsigned int,QVariant)),
            RecordingSessionViews::dashView, SLOT(onChannelUpdate(int,uint,QVariant)));

    connect(this, SIGNAL(channelUpdate(int,unsigned int,QVariant)),
            RecordingSessionViews::gpsView, SLOT(onChannelUpdate(int,unsigned int,QVariant)));

    connect(this, SIGNAL(channelUpdate(int,uint,QVariant)),
            RecordingSessionViews::utilitiesView, SLOT(onChannelUpdate(int,uint,QVariant)));


    fakeDateSender = new QTimer();
    fakeDateSender->setInterval(100);
    connect(fakeDateSender, SIGNAL(timeout()), this, SLOT(sendFakeData()));
}

void RecordingSession::startNew(QString name, QString desc, int trackCod, int vehicleCod){
    QSqlQuery q(MainWindow::database);
    q.prepare("INSERT INTO Gara(Nome, Data, OraInizio, Descrizione, CodVeicolo, CodCircuito) VALUES(:name,:date,:begin,:desc,:codv,:codc)");
    q.bindValue(":name", name);
    q.bindValue(":date", QDate::currentDate().toString("yyyy-MM-dd"));
    q.bindValue(":begin", QTime::currentTime().toString("hh:mm:ss"));
    q.bindValue(":desc", desc);
    q.bindValue(":codv", vehicleCod);
    q.bindValue(":codc", trackCod);
    q.exec();

    q.exec("SELECT IDGara FROM Gara ORDER BY IDGara DESC LIMIT 1");
    q.next();
    this->raceCod = q.value(0).toInt();
    this->trackCod = trackCod;
    this->vehicleCod = vehicleCod;

    ChannelConvParams* newChannel;
    q.exec("SELECT CanID, Dimensione, CodTipoDato, CodTipoConversione, a, b, c, CodTipoIO FROM Canale");
    while(q.next()){
        newChannel = new ChannelConvParams;
        newChannel->a = q.value("a").toFloat();
        newChannel->b = q.value("b").toFloat();
        newChannel->c = q.value("c").toFloat();
        newChannel->dataType = q.value("CodTipoDato").toInt();
        newChannel->convType = q.value("CodTipoConversione").toInt();
        newChannel->dim = q.value("Dimensione").toInt();

        if(q.value("CodTipoIO") == 2){ //OUT CHANNEL
            outChannelsConvParams->insert(q.value("CanID").toInt(), newChannel);
        }else if(q.value("CodTipoIO") == 1){ //IN CHANNEL
            channelsConvParams->insert(q.value("CanID").toInt(), newChannel);
        }else{ //IN/OUT CHANNEL
            channelsConvParams->insert(q.value("CanID").toInt(), newChannel);
            outChannelsConvParams->insert(q.value("CanID").toInt(), newChannel);
        }
    }

    i = 0;
    saveTimer->start();
    //fakeDateSender->start();

}

void RecordingSession::stop(){
    QSqlQuery q(MainWindow::database);
    q.prepare("UPDATE Gara SET OraFine = :end WHERE IDGara = :id");
    q.bindValue(":end", QTime::currentTime().toString("hh:mm:ss"));
    q.bindValue(":id", this->raceCod);
    q.exec();

    saveTimer->stop();
    fakeDateSender->stop();
    channelsConvParams->clear();

    save();
}

void RecordingSession::sendFakeData(){
    /*
    emit channelUpdate(255,i,(double)qSin(qDegreesToRadians((float)i)));
    emit channelUpdate(256,i,(double)(i*i));
    emit channelUpdate(257,i,(double)i*100);
    emit channelUpdate(258,i,QBitArray(8,i%2==0));
    emit channelUpdate(259,i,lat[i%48]);
    emit channelUpdate(260,i,lon[i%48]);

    time 203 -> 515
    lap 204
    inter 205
            */
    emit channelUpdate(515, i, i*100);
    if(i%30==0 && i>0)emit channelUpdate(516, i, 1);
    i++;
}

ChannelConvParams* RecordingSession::getInChannelParams(int can){
    return channelsConvParams->value(can);
}

ChannelConvParams* RecordingSession::getOutChannelParams(int can){
    return outChannelsConvParams->value(can);
}

void RecordingSession::onDATPacketReceived(QByteArray data){
    bool packetIsValid = true;
    unsigned int packetIndex, channelCount, idsIndex, dataIndex;
    unsigned int *channelsIDs;
    unsigned int i = 0;
    QVector<QVariant> channelsValues;
    ChannelConvParams *params;

    data.remove(0,3); //remove DAT
    packetIndex = ((data.at(0)&0xFF)<<8) | (data.at(1)&0xFF);
    channelCount = ((data.at(2)&0xFF)<<8) | (data.at(3)&0xFF);

    channelsIDs = new unsigned int[channelCount];

    //qDebug()<<"Ricevuto pacchetto "<<packetIndex<<"  "<<channelCount;

    idsIndex = 4;
    dataIndex = idsIndex+channelCount*2;
    while(i<channelCount && packetIsValid){
        channelsIDs[i] = ((data.at(idsIndex)&0xFF)<<8) | (data.at(idsIndex+1)&0xFF);
        params = channelsConvParams->value(channelsIDs[i]);

        if(!params || params->dim>data.size()-dataIndex){
            packetIsValid = false;
            qDebug()<<"CAN ID invalido "<<channelsIDs[i];

        }else if(params){
            switch(params->dataType){
                case BITFLAG_CONV:
                    channelsValues.append(QVariant(convertToBit(data, dataIndex, params)));
                break;

                case SIGN_NUMBER_CONV:
                    channelsValues.append(QVariant(convertToDouble(data, dataIndex, params)));
                break;

                case UNSIGN_NUMBER_CONV:
                    channelsValues.append(QVariant(convertToUDouble(data, dataIndex, params)));
                break;

                case STRING_CONV:
                    channelsValues.append(QVariant(convertToString(data, dataIndex, params)));
                break;
            }
            idsIndex+= 2;
            dataIndex+= params->dim;
        }
		i++;
    }

    if(packetIsValid){
        for(i=0; i<channelCount; i++){
            emit channelUpdate(channelsIDs[i], packetIndex, channelsValues.at(i));
            qDebug()<<"Ricevuto "<<packetIndex<<"  "<<channelsIDs[i]<<"  "<<channelsValues.at(i).toString();

            QSqlRecord record = sqlModel->record();
            record.setValue("IDGara", raceCod);
            record.setValue("Indice", packetIndex);
            record.setValue("IDCan", channelsIDs[i]);
            record.setValue("OraArrivo", QTime::currentTime().toString("hh:mm:ss:zzz"));
            if(channelsValues.at(i).type() == QVariant::Double){ //remove exponential format (i.e. 6e-5)
                record.setValue("Valore", QString::number(channelsValues.at(i).toDouble(),'f',10));
            }else{
                record.setValue("Valore", channelsValues.at(i).toString());
            }
            sqlModel->insertRecord(0, record);
            sqlModel->selectRow(0);
        }

    }

    delete[] channelsIDs;
}

bool RecordingSession::isHeader(QByteArray& data, QString header){
    for(int i=0; i<3; i++){
        if(data.at(i) != header.at(i))
            return false;
    }
    return true;
}

QBitArray RecordingSession::convertToBit(QByteArray& data, int index, ChannelConvParams *params){
    int d = 0;
    QBitArray value(params->dim*8, false);
    for(int i=0; i<params->dim; i++){
       d+=(data.at(index+i)<<(8*i));
    }

    for(int i=0; i<value.count(); i++){
        if(d & (1<<(value.count()-i-1))){
            value.setBit(i, true);
        }
    }
    return value;
}

double RecordingSession::convertToDouble(QByteArray& data, int index, ChannelConvParams *params){
    char mem[sizeof(long long int)];

    for(int i=0; i<params->dim; i++){
        mem[i] = data.at(index+i);
    }

    if(mem[params->dim-1] & (1<<7)){
        for(int i=params->dim; i<sizeof(long long int); i++){
            mem[i] = 0xFF;
        }
    }else{
        for(int i=params->dim; i<sizeof(long long int); i++){
            mem[i] = 0x00;
        }
    }

    return engConversion(*(reinterpret_cast<long long int*>(mem)), params);
}

double RecordingSession::convertToUDouble(QByteArray& data, int index, ChannelConvParams *params){
    char mem[sizeof(long long int)];

    for(int i=0; i<params->dim; i++){
        mem[i] = data.at(index+i);
    }

    for(int i=params->dim; i<sizeof(long long int); i++){
        mem[i] = 0x00;
    }

    return engConversion(*(reinterpret_cast<unsigned long long int*>(mem)), params);
}

QString RecordingSession::convertToString(QByteArray& data, int index, ChannelConvParams *params){
    QString value;
    for(int i=0; i<params->dim; i++){
        value.append(data.at(index+i));
    }
    value+='\0';
    return value;
}

double RecordingSession::engConversion(double v, ChannelConvParams *params){
    switch (params->convType){
        case XpA:
            return v+params->a;
        break;

        case XdA:
            if(params->a !=0)
                return v/params->a;
        break;

        case XpAdB:
            if(params->b !=0)
                return (v+params->a)/params->b;
        break;

        case XpAdBpC:
            if(params->b !=0)
                return (v+params->a)/params->b+params->c;
        break;
    }
    return v;
}

void RecordingSession::save(){
    sqlModel->database().transaction();
    sqlModel->submitAll();
    sqlModel->database().commit();
    sqlModel->clear();
    sqlModel->setTable("Storico");
}

RecordingSession::~RecordingSession(){
    qDebug()<<"RECORDING_SESSION: Deleting";
    saveTimer->stop();
    delete saveTimer;

    fakeDateSender->stop();
    delete fakeDateSender;

    channelsConvParams->clear();
    delete channelsConvParams;

    delete sqlModel;
}

double RecordingSession::lat[] = {
44.60892361092688,
44.608931248963714,
44.60895798208475,
44.60897325814839,
44.60896562011707,
44.60895416306821,
44.60900762927688,
44.60904200038504,
44.60904963840631,
44.60906109543634,
44.609106923533844,
44.609068733455096,
44.60909164750535,
44.60913747557875,
44.609351339443165,
44.60954992661217,
44.61003111578272,
44.61025261421976,
44.610435922632284,
44.61055812791923,
44.61058867920079,
44.610573403562015,
44.61051230096676,
44.610413009112364,
44.61032135494225,
44.61026025208183,
44.61014568404527,
44.6100616673415,
44.60998528841442,
44.609809616500975,
44.60962630611176,
44.609435357174455,
44.6092214936194,
44.60893888699955,
44.6087632119212,
44.60866391707666,
44.60855698397733,
44.60850351735384,
44.608518793537,
44.60855698397733,
44.608618088629626,
44.60868683128662,
44.60867155514766,
44.6086944693546,
44.60878612609199,
44.608778488036066,
44.60884723050323,
44.60884723050323
};

double RecordingSession::lon[] = {
10.914632678031921,
10.914568305015564,
10.914482474327087,
10.91441810131073,
10.91439664363861,
10.914337635040283,
10.914235711097717,
10.914117693901062,
10.914037227630615,
10.913940668106079,
10.913887023925781,
10.913758277893066,
10.913608074188232,
10.913339853286743,
10.913103818893433,
10.912867784500122,
10.912814140319824,
10.913007259368896,
10.913307666778564,
10.913586616516113,
10.913940668106079,
10.914326906204224,
10.91465950012207,
10.915077924728394,
10.915485620498657,
10.915882587432861,
10.916215181350708,
10.916687250137329,
10.917105674743652,
10.917556285858154,
10.91765284538269,
10.91789960861206,
10.917953252792358,
10.917813777923584,
10.91764211654663,
10.917524099349976,
10.917212963104248,
10.91690182685852,
10.91667652130127,
10.916311740875244,
10.916097164154053,
10.915871858596802,
10.915818214416504,
10.915721654891968,
10.915528535842896,
10.915346145629883,
10.915099382400513,
10.915013551712036
};
