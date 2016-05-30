#include "CANInterface.h"
#include "GSMInterface.h"
//#define CAN_DEBUG ON

void CANInterfaceClass::init(){
	if(Settings.canConfigValid()){
		Can0.begin(CAN_BPS_125K);
		Can0.watchForRange(Settings.getChannelParams(0)->canID, Settings.getChannelParams(Settings.getChannelNum()-1)->canID);
		available = true;
		Serial.println("CAN_COMUNICATION_STARTED");
	}else{
		Serial.println("CAN_NO_CHANNEL_TO_SAVE");
		Serial.println("ARDUINO_STOP");
		while(1);
	}
}


void CANInterfaceClass::readPacket(){
	#ifdef CAN_DEBUG
		Serial.println("/////////CAN_DEBUG_START//////////");
	#endif
	int i;
	while(Can0.available()){
		Can0.read(RXFrame);
		if(Settings.findChannel(RXFrame.id)){
			ChannelBuffer.setValue(RXFrame.id, RXFrame.data.bytes);
			#ifdef CAN_DEBUG
				Serial.print("CAN_READ 0x");Serial.print(RXFrame.id, HEX);
				Serial.print(" -> ");
				for(i = 0; i<RXFrame.length; i++){
					Serial.print(RXFrame.data.bytes[i], HEX);
					Serial.print(" ");
				}
				Serial.println();
			#endif
		}
		if(Settings.getPTTRequestChannel() == RXFrame.id){
			if(GSMInterface.getStatus() == GSMInterfaceClass::VOICE_CALL){
				Serial.println("CALL_END_REQUIRED");
				GSMInterface.endCall();
			}else{
				Serial.println("CALL_START_REQUIRED");
				GSMInterface.startCall();
			}
		}
	}
	#ifdef CAN_DEBUG
		Serial.println("/////////CAN_DEBUG_END//////////");
	#endif
}

void CANInterfaceClass::sendPacket(uint32_t  id, char* msg, int size){
	TXFrame.id = id;
	TXFrame.length = size;
	memcpy(TXFrame.data.bytes,msg,size);
	Can0.sendFrame(TXFrame);
}

CANInterfaceClass CANInterface;

