#include "Settings.h"

void SettingsClass::debug(){
	if(gsmValid){
		Serial.println();
		Serial.println("GSM_SETTINGS_VALID");
		Serial.print("APN_NAME = ");Serial.println(apn);
		Serial.print("APN_USER = ");Serial.println(user);
		Serial.print("APN_PSW = ");Serial.println(psw);
		Serial.print("TCP_PORT = ");Serial.println(TCPport);
		Serial.print("UDP_PORT = ");Serial.println(UDPport);
		Serial.print("RECEIVER_NUM = ");Serial.println(receiverNum);
		Serial.print("SENDER_NUM = ");Serial.println(senderNum);
		Serial.print("SIM_PIN = ");Serial.println(pin);
		Serial.print("PACKET_PER_SECONDS = ");Serial.println(packetPerSecond);
		Serial.print("PACKET_TIMEOUT = ");Serial.println(packetTimeOut);
		Serial.print("PUSH_TO_TALK_REQUEST = ");Serial.println(pttRequestChannel);
		Serial.print("PUSH_TO_TALK_STATUS = ");Serial.println(pttStatusChannel);
	}else{
		Serial.println("GSM_SETTINGS_NOT_VALID");
	}

	Serial.println();

	if(canValid){
		Serial.println("CHANNELS_PARAMS_VALID");
		for(int i=0; i<channelNum; i++){
			Serial.print(channels[i]->canID);Serial.print("   ");
			Serial.print(channels[i]->dataType);Serial.print("   ");
			Serial.print(channels[i]->dim);Serial.print("   ");
			Serial.print(channels[i]->convType);Serial.print("   ");
			Serial.print(channels[i]->a);Serial.print("   ");
			Serial.print(channels[i]->b);Serial.print("   ");
			Serial.print(channels[i]->c);Serial.print("   ");
			Serial.println();
		}
	}else{
		Serial.println("CAN_SETTINGS_NOT_VALID");
	}

	Serial.println();
}

void SettingsClass::setChannelNum(unsigned int num){
	this->channelNum = num;
	this->channels = new ChannelConvParams*[num];
}

void SettingsClass::addChannelParams(ChannelConvParams*param, int i){
	this->channels[i] = param;
}

ChannelConvParams* SettingsClass::findChannel(unsigned short id){
	int i=0;
	while(i<channelNum && channels[i]->canID!=id)i++;

	if(i<channelNum){
		return channels[i];
	}
	return NULL;
}

int SettingsClass::getChannelIndex(unsigned short id){
	int i=0;
	while(i<channelNum && channels[i]->canID!=id)i++;

	if(i<channelNum){
		return i;
	}
	return -1;
}

SettingsClass Settings;

