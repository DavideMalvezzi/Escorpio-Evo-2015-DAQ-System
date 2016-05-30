#include "channelvalues.h"
#include "../mainwindow.h"

ChannelValues::ChannelValues(int raceID, QString name, int type, int max){
    this->raceID = raceID;
    this->name = name;
    this->type = type;
    this->max = max;
    this->values = new QVariant[max];
    this->lastIndex = 0;
}

void ChannelValues::addValue(int index, QVariant v){
    values[index] = v;
    if(index-lastIndex>1){
        for(int i=lastIndex+1; i<index; i++){
            values[i] = values[lastIndex];
        }
    }
    lastIndex = index;
}


ChannelValues::~ChannelValues(){
    qDebug()<<"CHANNEL_VALUES: Deleting";
    delete[] values;
}

