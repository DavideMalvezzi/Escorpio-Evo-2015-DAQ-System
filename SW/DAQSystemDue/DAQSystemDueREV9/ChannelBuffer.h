// ChannelBuffer.h

#ifndef _CHANNELBUFFER_h
#define _CHANNELBUFFER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Settings.h"
#define BITFLAG_CONV 0
#define SIGN_NUMBER_CONV 1
#define UNSIGN_NUMBER_CONV 2
#define STRING_CONV 3

#define XpA 1
#define XdA 2
#define XpAdB 3
#define XpAdBpC 4

class ChannelBufferClass{
 private:
	unsigned char *channelUpdated;
	unsigned char **buffer, *rawBuffer;
	unsigned short packetIndex, bufferSize, rawSize;
	double convert(double, ChannelConvParams *);

 public:
	void init();
	unsigned char* getRawData();
	String getValue(unsigned short);
	void setValue(unsigned short, unsigned char*);
	unsigned short getPacketIndex(){return packetIndex;}
	unsigned short getBufferSize(){return bufferSize;}
	unsigned short getRawSize(){return rawSize;}
	void updatePacketIndex(){packetIndex++;}
	void reset();
	void debug();
	unsigned char* getRawValue(int index){return buffer[index];}
	unsigned char isUpdated(int channel){int index = Settings.getChannelIndex(channel);return channelUpdated[index];}


};

extern ChannelBufferClass ChannelBuffer;

#endif

