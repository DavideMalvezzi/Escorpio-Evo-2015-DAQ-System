// GSMInterface.h

#ifndef _GSMINTERFACE_h
#define _GSMINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "List.h"
#include "ChannelBuffer.h"

#define GSMSerial Serial3
#define CR 0x0D
#define LF 0x0A
#define TEXT_END 0x1A

#define AT_MSG_HEADER 0
#define DAT_MSG_HEADER 1
#define ACK_MSG_HEADER 2
#define CAN_MSG_HEADER 3
#define CMD_MSG_HEADER 4

#define BUFFER_MAX_DIM 512
#define PACKET_MAX_SIZE 512
#define RADIO_TIMEOUT 500

#define GSM_POWER 26

class GSMInterfaceClass{
	public:
		enum GSMState{
			//Led OFF
			BLOCK ,
			CHECK_MODULE_ON,
			CHECK_SIM_PIN,
			CHECK_GSM_CONNECTION,	
			//Led ON-OFF 500ms
			RESET_NETWORK,
			SETUP_NETWORK_SETTINGS,
			BRING_UP_WIRELESS,		
			GET_CURRENT_IP,
			SEND_IP,				
			START_TCP_CONNECTION,	
			//Led ON-OFF 100ms
			WAIT_FOR_CLIENT,		
			START_UDP_CONNECTION,
			//Led ON
			CONNECTED,	
			SEND_DATA,
			EXECUTE_CMD,
			VOICE_CALL,
			DEBUG
		};

		enum CallStatus{
			CALL_ACTIVE = 0,
			CALL_HELD = 1,
			CALL_DIALING = 2,
			CALL_ALERTING = 3,
			CALL_INCOMING = 4,
			CALL_WAITING = 5,
			CALL_DISCONNECT = 6
		};

		void init();
		void update();
		void send(unsigned char type, unsigned char*msg, unsigned short size);
		void startCall();
		void endCall();
		GSMState getStatus(){return gsmStatus;}

	private:
		unsigned char RXbuffer[BUFFER_MAX_DIM], RXbufferDim, currentCallStatus;
		char TCPConnectionHandler;
		uint32_t cmdTimer, cmdTimeout, idleTimer, idleTimeout, resendTimer, TXsendCount, radioTimer;
		bool waitingForMsg, datStarted;
		static char ledCounter;
		volatile static GSMState gsmStatus, prevStatus;

		ListClass *RXMessage, *TXMessage, *stackMessage, *cmdMessage;

		void powerUp();
		void readFromSerial();
		void handleIncomingMsg();
		void request();
		void response();
		void onCmdTimeout();
		void processCmd();

		void setState(GSMState state);
		void setResponseTimeOut(uint32_t time);
		void setIdleTime(uint32_t time);

		void sendCmd(String cmd);
		void sendCmd(String cmd, String param1);
		void sendCmd(String cmd, String param1, String param2);
		void sendCmd(String cmd, String param1, String param2, String param3);
		void sendCmd(String cmd, String param1, String param2, String param3, String param4);

		void resetRXBuffer();

		bool isResponse(const char* response);
		bool isResponse(String response);
		bool isCommand(const char* cmd);
		bool isHeader(unsigned char*msg, const char*header);
		unsigned char getHeaderType(unsigned char*, unsigned short);
		int findDelimiters(unsigned char*,unsigned char from, unsigned char size);
		
		void parseIP(Node*msg);
		
		static void updateLedStatus();

		
};

extern GSMInterfaceClass GSMInterface;

#endif

