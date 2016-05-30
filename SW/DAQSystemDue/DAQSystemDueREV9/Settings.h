// Settings.h

#ifndef _SETTINGS_h
#define _SETTINGS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define SD_CSPIN 28

#define CAN_STATUS_LED 32
#define SD_STATUS_LED  34
#define GSM_STATUS_LED 36

typedef struct ChannelConvParams{
	unsigned short canID;
	unsigned char dim, convType, dataType;
    float a, b, c;
}ChannelConvParams;


class SettingsClass{
private:
	bool gsmValid, canValid;
	unsigned int channelNum, packetPerSecond, packetTimeOut;
	short pttRequestChannel,pttStatusChannel;
	String apn, user, psw, pin, server, TCPport,UDPport,senderNum,receiverNum,clientIP,localIP;
	ChannelConvParams **channels;

 public:

	bool gsmConfigValid(){return gsmValid;}
	bool canConfigValid(){return canValid;}
	unsigned int getChannelNum(){return channelNum;}
	unsigned int getPacketPerSecond(){return packetPerSecond;}
	unsigned int getPacketTimeOut(){return packetTimeOut;}
	String getApnName(){return apn;}
	String getUserName(){return user;}
	String getUserPsw(){return psw;}
	String getReceiverNum(){return receiverNum;}
	String getSenderNum(){return senderNum;}
	String getPin(){return pin;}
	String getServerName(){return server;}
	String getServerPort(){return TCPport;}
	String getUDPPort(){return UDPport;}
	String getClientIP(){return clientIP;}
	String getLocalIP(){return localIP;}
	short getPTTRequestChannel(){return pttRequestChannel;}
	short getPTTStatusChannel(){return pttStatusChannel;}

	void setGsmConfigValid(bool valid){this->gsmValid = valid;}
	void setCanConfigValid(bool valid){this->canValid = valid;}
	void setChannelNum(unsigned int);
	void setPacketPerSecond(unsigned int value){this->packetPerSecond = value;}
	void setPacketTimeOut(unsigned int value){this->packetTimeOut = value;}
	void setApnName(String value){this->apn = value;}
	void setUserName(String value){this->user = value;}
	void setUserPsw(String value){this->psw = value;}
	void setReceiverNum(String value){this->receiverNum = value;}
	void setSenderNum(String value){this->senderNum = value;}
	void setPin(String value){this->pin = value;}
	void setServerName(String value){this->server = value;}
	void setServerPort(String value){this->TCPport = value;}
	void setUDPPort(String value){this->UDPport = value;}
	void setClientIP(String value){this->clientIP = value;}
	void setLocalIP(String value){this->localIP = value;}
	void setPTTRequestChannel(short value){this->pttRequestChannel = value;}
	void setPTTStatusChannel(short value){this->pttStatusChannel = value;}


	void addChannelParams(ChannelConvParams*, int i);
	int getChannelIndex(unsigned short);
	ChannelConvParams* findChannel(unsigned short);
	ChannelConvParams* getChannelParams(int i){return channels[i];}

	void debug();
};

extern SettingsClass Settings;

#endif

