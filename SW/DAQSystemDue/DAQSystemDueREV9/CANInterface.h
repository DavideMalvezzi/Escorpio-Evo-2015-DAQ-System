// CANInterface.h
#ifndef _CANINTERFACE_h
#define _CANINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif


#include "variant.h"
#include "due_can.h"
#include "Settings.h"
#include "ChannelBuffer.h"


class CANInterfaceClass{

 private:
	 CAN_FRAME RXFrame, TXFrame;

 public:
	bool available;
	void init();
	void readPacket();
	void sendPacket(uint32_t  id, char* msg, int size);
};

extern CANInterfaceClass CANInterface;

#endif