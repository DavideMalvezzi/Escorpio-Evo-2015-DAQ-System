#include "GSMInterface.h"
#include "Settings.h"
#include "DueTimer.h"
#include "CANInterface.h"

//#define FUNCTION_DEBUG ON
//#define RX_BUFFER_DEBUG ON
//#define STACK_DEBUG ON

char GSMInterfaceClass::ledCounter = 0;
volatile GSMInterfaceClass::GSMState GSMInterfaceClass::gsmStatus = GSMInterfaceClass::BLOCK;
volatile GSMInterfaceClass::GSMState GSMInterfaceClass::prevStatus = GSMInterfaceClass::BLOCK;

void GSMInterfaceClass::init(){
	GSMSerial.begin(19200);
	while(!GSMSerial);

	RXMessage = new ListClass("RXBUFFER",BUFFER_MAX_DIM);
	cmdMessage = new ListClass("CMDBUFFER",BUFFER_MAX_DIM);
	TXMessage = new ListClass("TXBUFFER",BUFFER_MAX_DIM*2);
	stackMessage = new ListClass("STACKBUFFER",BUFFER_MAX_DIM*2);

	RXbufferDim = 0;

	idleTimer = 0;
	idleTimeout = 0;
	cmdTimer = 0;
	cmdTimeout = 0;
	resendTimer = 0;
	radioTimer = 0;

	datStarted = false;
	waitingForMsg = false;

	setState(CHECK_MODULE_ON);
	//setState(DEBUG);
	Serial.println("GSM_CREATED");

	ledCounter = 0;
	Timer8.setFrequency(10).attachInterrupt(updateLedStatus).start();
}

void GSMInterfaceClass::updateLedStatus(){
	if(gsmStatus<=CHECK_GSM_CONNECTION){
		pinMode(GSM_STATUS_LED,LOW);

	}else if(GSMInterface.gsmStatus<=START_TCP_CONNECTION){
		if(GSMInterface.ledCounter<5){
			pinMode(GSM_STATUS_LED,HIGH);
		}else{
			pinMode(GSM_STATUS_LED,LOW);
		}

	}else if(GSMInterface.gsmStatus<=START_UDP_CONNECTION){
		if(GSMInterface.ledCounter%2==0){
			pinMode(GSM_STATUS_LED,HIGH);
		}else{
			pinMode(GSM_STATUS_LED,LOW);
		}

	}else if(GSMInterface.gsmStatus<=CONNECTED){
		pinMode(GSM_STATUS_LED,HIGH);

	}else if(GSMInterface.gsmStatus == VOICE_CALL){
		if(GSMInterface.ledCounter%3==0){
			pinMode(GSM_STATUS_LED,HIGH);
		}else{
			pinMode(GSM_STATUS_LED,LOW);
		}
	}

	GSMInterface.ledCounter = (GSMInterface.ledCounter+1)%10;

}

void GSMInterfaceClass::update(){
	readFromSerial();

	if(millis()-idleTimer>idleTimeout){
		handleIncomingMsg();
		
		//No data sending
		if (cmdMessage->getCount()){// && gsmStatus>=WAIT_FOR_CLIENT && gsmStatus!= SEND_DATA){
			Serial.println("EXECUTING CMD");
			while(cmdMessage->getCount())processCmd();
			//setState(EXECUTE_CMD);
		}else{
			if(waitingForMsg){
				response();
			}else{
				request();
			}
		}
	}

	if(millis()-radioTimer>RADIO_TIMEOUT){
		CANInterface.sendPacket(Settings.getPTTStatusChannel(), (char*)(&currentCallStatus),1);
		radioTimer = millis();
	}
}

void GSMInterfaceClass::readFromSerial(){
//int begin = -1;
	int end = -1;
	//int lastByte = -1;
	//int from = 0;
	unsigned char header;

	int size = GSMSerial.available();
	while(size){
		//Serial.println("START READING");

		GSMSerial.readBytes(&RXbuffer[RXbufferDim], size);
		RXbufferDim += size;

		for(int i=0; i<RXbufferDim; i++){
			Serial.print(RXbuffer[i], HEX); Serial.print(" ");
		}
		Serial.println();


		//Serial.println("END READING");
		//Serial.println("START PARSING");

		end = findDelimiters(RXbuffer, 0, RXbufferDim);
		while(end>=0){
			if(end>0){
				Serial.print("RX -> ");Serial.write(RXbuffer, end);Serial.println();

				header = getHeaderType(RXbuffer,end);
				if(header == CMD_MSG_HEADER || header == ACK_MSG_HEADER){
					cmdMessage->insert(header, RXbuffer, end);
				}else{
					RXMessage->insert(header, RXbuffer, end);
				}
			}

			RXbufferDim -= (end+2);
			memcpy(RXbuffer, RXbuffer+end+2, RXbufferDim);

			end = findDelimiters(RXbuffer, 0, RXbufferDim);
		}
		//Serial.println("END PARSING");

		size = GSMSerial.available();
	}

	/*
	if(Serial3.overflow()){
		Serial.println("OVERFLOW");
	}*/
}

void GSMInterfaceClass::handleIncomingMsg(){
	RXMessage->reset();
	while(RXMessage->next()){

		if(isResponse("POWER DOWN")){
			Serial.println("MODULE_POWER_DOWN");
			setIdleTime(2000);
			setState(CHECK_MODULE_ON);
		}

		else if(isResponse("+PDP: DEACT")){
			Serial.println("PDP_SHUTDOWN");
			setState(RESET_NETWORK);
		}

		else if(isResponse("RDY")){
			Serial.println("MODULE_POWER_ON");
			setIdleTime(1000);
			setState(CHECK_MODULE_ON);
		}

		else if(isResponse("CLOSE") && isResponse(String(TCPConnectionHandler))){
			Serial.println("CONNECTION_LOST");
			sendCmd("AT+CIPCLOSE = 1,1"); 
			Settings.setClientIP(String("null"));
			setState(WAIT_FOR_CLIENT);
		}

		else if(isResponse("REMOTE") && gsmStatus >= CONNECTED){
			cmdMessage->insert(RXMessage->take(RXMessage->current())); 
		}

		else if(isResponse("+CIPRXGET:1")){
			cmdMessage->insert(RXMessage->take(RXMessage->current())); 
		}

		else if(isResponse("RING")){
			//setState(VOICE_CALL);
			cmdMessage->insert(RXMessage->take(RXMessage->current()));
		}

	}
}

void GSMInterfaceClass::request(){
	switch (gsmStatus) {
        case CHECK_MODULE_ON:
			Serial.println("CHECK_MODULE_ON");
            sendCmd("AT");
            setResponseTimeOut(3000);
            break;

        case CHECK_SIM_PIN:
			Serial.println("CHECK_SIM_PIN");
            sendCmd("AT+CPIN?");
            setResponseTimeOut(1000);
            break;

        case CHECK_GSM_CONNECTION:
			Serial.println("CHECK_GSM_CONNECTION");
            sendCmd("AT+CCALR?");
            setResponseTimeOut(3000);
            break;

        case RESET_NETWORK:
			Serial.println("RESET_NETWORK");
			Settings.setClientIP(String("null"));
            sendCmd("AT+CIPSHUT");
            setResponseTimeOut(10000);
            break;

        case SETUP_NETWORK_SETTINGS:
			Serial.println("SETUP_NETWORK_SETTINGS");
			sendCmd("ATE0");
			sendCmd(
				"ATS0 = 1;"	//call auto-respond
				"+CIPRXGET = 1;" //manual data retrieve
				"+CIPMUX = 1;" //multi-connection
				"+CSTT = \"%1\", \"%2\", \"%3\";" //apn setup
				"+CIPUDPMODE = 1,1;"	//UDP setup
				"+CLPORT = 1, UDP, %4;"	//UDP port
				"+CMGF = 1;"	//Text sms
				"+CIPSPRT = 0;"	//No '>' when send
				"+CIPQSEND = 1;"	//Quick send response
				"+CLVL = 100;" //Max volume
				"+CMIC = 0,10;" //Mic gain
				"+CLCC=1", //Call status
				Settings.getApnName(), Settings.getUserName(), Settings.getUserPsw(), Settings.getUDPPort()
				);
			setResponseTimeOut(3000);
            break;

        case BRING_UP_WIRELESS:
			Serial.println("BRING_UP_WIRELESS");
            sendCmd("AT+CIICR");
            setResponseTimeOut(15000);
            break;

        case GET_CURRENT_IP:
			Serial.println("GET_CURRENT_IP");
            sendCmd("AT+CIFSR");
            setResponseTimeOut(5000);
            break;

		case SEND_IP:
			Serial.println("SEND_IP");
			sendCmd("AT+CMGS = \"%1\"", Settings.getReceiverNum());
			delay(200);
			GSMSerial.write(CR);
			GSMSerial.write(LF);
			GSMSerial.print("+IPSERVER:");
			GSMSerial.print(Settings.getLocalIP());
			GSMSerial.write(TEXT_END);
            setResponseTimeOut(30000);
			break;

        case START_TCP_CONNECTION:
			Serial.println("START_TCP_SERVER");
            sendCmd("AT+CIPSERVER = 1, \"%1\"", Settings.getServerPort());
            setResponseTimeOut(35000);
            break;

		case WAIT_FOR_CLIENT:
			Serial.println("WAIT_FOR_CLIENT");
			if(Settings.getClientIP().length()<8){
				setResponseTimeOut(35000);
			}
			break;

        case START_UDP_CONNECTION:
			Serial.println("START_UDP_CONNECTION");
			sendCmd("AT+CIPSTART = 1,\"UDP\", \"%1\", \"%2\"", Settings.getClientIP(), Settings.getUDPPort());
            setResponseTimeOut(5000);
            break;

		case CONNECTED:
			//Serial.println("CONNECTED");
			if(TXMessage->getCount() && !cmdMessage->getCount()){
				Serial.println("SENDING DATA");
				int TXSize = 0;
				TXsendCount = 0;

				TXMessage->reset();
				while(TXMessage->next() && (TXSize + TXMessage->current()->bufferDim + 7)<PACKET_MAX_SIZE){
					TXSize += 7 +TXMessage->current()->bufferDim;
					TXsendCount++;
				}

				if(TXsendCount){
					sendCmd("AT+CIPSEND = 1, %1", String(TXSize));
					setIdleTime(100);
					setResponseTimeOut(3000);
					setState(SEND_DATA);
				}

				if(millis()-resendTimer>Settings.getPacketTimeOut()){
					int currentTime = ChannelBuffer.getPacketIndex() * 1000 / Settings.getPacketPerSecond();
					int packetID, packetTime = 0;

					/*
					stackMessage->reset();
					while(currentTime-packetTime>Settings.getPacketTimeOut() && stackMessage->next()){
						packetID = ((stackMessage->current()->buffer[0]&0xFF)<<8) | (stackMessage->current()->buffer[1]&0xFF);
						packetTime = packetTime * 1000 / Settings.getPacketPerSecond();
						if(currentTime-packetTime>Settings.getPacketTimeOut()){
							Serial.print("Rinvio pacchetto ");Serial.println(packetID);
							TXMessage->insert(stackMessage->take(stackMessage->current()));
						}
					}
				
					resendTimer = millis();
					*/
				}
			}

			break;

		case SEND_DATA:
			Serial.println("SEND_DATA");
			/*
			if(millis()-cmdTimer>cmdTimeout){
				Serial.println("DATA_SEND_FAILED");
				setState(CONNECTED);
			}else{*/
				TXMessage->reset();
				for(int i=0; i<TXsendCount; i++){
					TXMessage->next();
					GSMSerial.write(CR);
					GSMSerial.write(LF);
					GSMSerial.print("DAT");
					GSMSerial.write(TXMessage->current()->buffer,TXMessage->current()->bufferDim);
					GSMSerial.write(CR);
					GSMSerial.write(LF);
					GSMSerial.flush();
				}
				Serial.print("Invio...");Serial.println(TXsendCount);
				setResponseTimeOut(2000);
			//}

			break;

		
		case VOICE_CALL:
			Serial.println("VOICE_CALL");
			setResponseTimeOut(1000000);
			break;

		case DEBUG:
			while(Serial.available()){
				GSMSerial.write(Serial.read());
			}
			break;
    }
}

void GSMInterfaceClass::response(){
	RXMessage->reset();
	if(RXMessage->next()){
		switch (gsmStatus) {
			case CHECK_MODULE_ON:
				if(isResponse("OK")){
					Serial.println("Modulo acceso");
					setState(CHECK_SIM_PIN);
				}
				break;

			case CHECK_SIM_PIN:
				if(isResponse("SIM PIN")){
					sendCmd("AT+CPIN = %1", Settings.getPin());
				}else if(isResponse("OK") || isResponse("READY")){
					Serial.println("PIN OK");
					setState(CHECK_GSM_CONNECTION);
				}else if(isResponse("ERROR")){
					Serial.println("PIN errato");
					setState(BLOCK);
				}
				break;

			case CHECK_GSM_CONNECTION:
				if(isResponse("+CCALR: 1")){
					Serial.println("Connesso alla rete GSM");
					setState(RESET_NETWORK);
				}
				break;

			case RESET_NETWORK:
				if(isResponse("SHUT OK")){
					Serial.println("Reset OK");
					setState(SETUP_NETWORK_SETTINGS);
				}
				break;
			case SETUP_NETWORK_SETTINGS:
				if(isResponse("OK")){
					Serial.println("Configurazione di rete riuscita");
					//setState(BRING_UP_WIRELESS);
					setState(BLOCK);
				}else if(isResponse("ERROR")){
					Serial.println("Configurazione di rete fallita");
					setState(RESET_NETWORK);
				}

				break;

			case BRING_UP_WIRELESS:
				if(isResponse("OK")){
					Serial.println("Wireless OK");
					setState(GET_CURRENT_IP);
				}else if(isResponse("ERROR")){
					Serial.println("Wireless ERROR");
					setIdleTime(3000);
					setState(RESET_NETWORK);
				}
				break;

			case GET_CURRENT_IP:
				if(isResponse("ERROR")){
					Serial.println("IP ERROR");
					setState(RESET_NETWORK);
				}else{
					String IP;
					for(int i=0; i<RXMessage->current()->bufferDim; i++){
						IP.concat((char)RXMessage->current()->buffer[i]);
					}
					Settings.setLocalIP(IP);
					Serial.print("CURRENT_IP -> ");Serial.println(Settings.getLocalIP());
					
					setState(SEND_IP);
				}
				break;

			case SEND_IP:
				if(isResponse("OK")){
					Serial.println("SMS IP inviato");
					setState(START_TCP_CONNECTION);
				}
				break;

			case START_TCP_CONNECTION:
				if(isResponse("SERVER OK")){
					Serial.println("Server creato");
					setState(WAIT_FOR_CLIENT);
				}else if(isResponse("ERROR")){
					Serial.println("Errore durante la creazione del server");
					setState(RESET_NETWORK);
				}
				break;

			case WAIT_FOR_CLIENT:
				if(isResponse("REMOTE IP")){
					parseIP(RXMessage->current());
					Serial.print("REMOTE_IP -> ");Serial.println(Settings.getClientIP());
					setState(START_UDP_CONNECTION);
				}
				break;

			case START_UDP_CONNECTION:
				if(isResponse("CONNECT OK")){
					Serial.println("Connessione UDP creata");
					resendTimer = millis();
					setState(CONNECTED);
				}else if(isResponse("ALREADY CONNECT")){
					Serial.println("Connessione già esistente");
					resendTimer = millis();
					setState(CONNECTED);
				}else if(isResponse("CONNECT FAIL")){
					Serial.println("Connessione fallita");
					setState(START_UDP_CONNECTION);
				}else if(isResponse("ERROR")){
					Serial.println("Errore durante la connessione");
					//setState(RESET_NETWORK);
				}
				break;

			case CONNECTED:
				if(isResponse("+CIPRXGET:")){
					RXMessage->pop();
					setState(CONNECTED);
				}
				break;

			case SEND_DATA:
				if(isResponse("ACCEPT")){
					Serial.println("DATA_SENT");
					for(int i=0; i<TXsendCount; i++){

						//stackMessage->insert(TXMessage->take());
						TXMessage->pop();
					}
					Serial.print("STACKBUFFER SIZE -> ");Serial.println(stackMessage->getCount());
	
#ifdef STACK_DEBUG
					Serial.print("STACKBUFFER ->");
					int id;
					stackMessage->reset();
					while (stackMessage->next()){
						id = ((stackMessage->current()->buffer[0]&0xFF)<<8) | (stackMessage->current()->buffer[1]&0xFF);
						Serial.print(id);Serial.print(" ");
					}
					Serial.println();
#endif
					setState(CONNECTED);

				}else if(isResponse("FAIL") || isResponse("ERROR")){
					Serial.println("Invio fallito");
					setState(CONNECTED);
				}
				break;

			
			case VOICE_CALL:
				/*
				if(isResponse("NO CARRIER")){
					Serial.println("CALL_ENDED");
					CANInterface.sendPacket(Settings.getPushToTalkChannel(),"0",2);
					setState(prevStatus);
				}else
				*/ 
				if(isResponse("BUSY")){
					Serial.println("CALL_BUSY");
					Serial.println("Destinatario occupato");
					sendCmd("ATD%1;",Settings.getReceiverNum());
				}else if(isResponse("+CLCC")){
					//+CLCC: id,from,status,0,0,"+393663154577",145,"""
					char callFrom = RXMessage->current()->buffer[9] - '0';
					char callStatus = RXMessage->current()->buffer[11] - '0';
					char status;

					if(callStatus == CALL_ACTIVE){
						Serial.println("CALL_ACTIVE");
						currentCallStatus = 1;
						//CANInterface.sendPacket(Settings.getPTTStatusChannel(), &status, 1);
					}else if(callStatus == CALL_DISCONNECT){
						Serial.println("CALL_ENDED");
						currentCallStatus = 0;
						//CANInterface.sendPacket(Settings.getPTTStatusChannel(), &status, 1);
						setState(prevStatus);
					}
				}
				break;
			
		}

		RXMessage->pop();
	}

	
    if(millis()-cmdTimer>cmdTimeout){
		onCmdTimeout();
        waitingForMsg = false;
        Serial.println("Response timeout");
    }
}

void GSMInterfaceClass::onCmdTimeout(){
	switch(gsmStatus){
		case CHECK_MODULE_ON:
			powerUp();
			setIdleTime(1000);
			break;
		case SEND_DATA:
			Serial.println("DATA_SEND_FAILED");
			setState(CONNECTED);
			break;
	}
}

void GSMInterfaceClass::processCmd(){
	cmdMessage->reset();
	cmdMessage->next();

	Serial.print("EXECUTE_CMD -> ");
	Serial.write(cmdMessage->current()->buffer,cmdMessage->current()->bufferDim);Serial.println();

	if(isCommand("ACK")){
		unsigned short ackID = ((cmdMessage->current()->buffer[3]&0xFF)<<8) | (cmdMessage->current()->buffer[4]&0xFF);
		unsigned short packetID = -1;
		//Serial.print("ACK -> ");Serial.println(ackID);
		stackMessage->reset();
		while(ackID != packetID && stackMessage->next()){
			packetID = ((stackMessage->current()->buffer[0]&0xFF)<<8) | (stackMessage->current()->buffer[1]&0xFF);
		}

		if(ackID != packetID){
			Serial.print("PACKET_NOT_FOUND ->");Serial.println(ackID);
		}else{
			stackMessage->pop(stackMessage->current());
			//Serial.print("PACKET_FOUND -> ");Serial.println(ackID);
		}
				
		//setState(CONNECTED);
	}

	else if(isCommand("REMOTE")){
		char oldConnection = TCPConnectionHandler;
		Serial.println("NEW_CLIENT");
		parseIP(cmdMessage->current());
		sendCmd("AT+CIPCLOSE = %1,1;"
				"+CIPUDPMODE = 1,2,\"%2\",%3", 
				String(oldConnection), Settings.getClientIP(), Settings.getUDPPort());
		//setState(CONNECTED);
	}

	else if(isCommand("+CIPRXGET:1")){
		Serial.println("RETRIEVING DATA");
		sendCmd("AT+CIPRXGET = 2,%1,1460", String(TCPConnectionHandler));
		//setState(CONNECTED);
	}

	else if(isCommand("STARTDAT")){
		Serial.println("START_SEND_DATA");
		datStarted = true;
		//setState(CONNECTED);
	}	

	else if(isCommand("ENDDAT")){
		Serial.println("END_SEND_DATA");
		datStarted = false;
		//setState(CONNECTED);
	}

	//CALL CMD
	else if(isCommand("RING")){
		Serial.println("INCOMING_CALL");
		setState(VOICE_CALL);
	}

	else if(isCommand("STARTCALL")){
		Serial.println("START_CALLING");
		sendCmd("ATD%1;",Settings.getReceiverNum());
		setState(VOICE_CALL);
	}

	else if(isCommand("ENDCALL")){
		Serial.println("CLOSING_CALL");
		sendCmd("ATH");
		//setState(CONNECTED);
	}
	
	cmdMessage->pop();
}

void GSMInterfaceClass::setState(GSMState state){
#ifdef FUNCTION_DEBUG
	Serial.println("setState");
#endif
	if(state != gsmStatus){
		prevStatus = gsmStatus;
	}
	gsmStatus = state;

	waitingForMsg = false;
	RXMessage->clear();

	//resetRXBuffer();
}

void GSMInterfaceClass::setResponseTimeOut(uint32_t time){
#ifdef FUNCTION_DEBUG
	Serial.println("setResponseTimeOut");
#endif
	waitingForMsg = true;
	cmdTimer = millis();
	cmdTimeout = time;
}

void GSMInterfaceClass::setIdleTime(uint32_t time){
#ifdef FUNCTION_DEBUG
	Serial.println("setIdleTime");
#endif
	idleTimer = millis();
	idleTimeout = time;
}

int GSMInterfaceClass::findDelimiters(unsigned char*buffer, unsigned char from, unsigned char size){
#ifdef FUNCTION_DEBUG
	Serial.println("findDelimiters");
#endif
	int i=from;
	while(i<size){
		if(buffer[i] == CR && buffer[i+1] == LF){
			return i;
		}
		i++;
	}
	return -1;
}

bool GSMInterfaceClass::isResponse(const char* response){
#ifdef FUNCTION_DEBUG
	Serial.print("isResponse -> ");Serial.println(response);
#endif
	return strstr((char*)RXMessage->current()->buffer, response)!=NULL;
}

bool GSMInterfaceClass::isResponse(String response){
#ifdef FUNCTION_DEBUG
		Serial.print("isResponse2 -> ");Serial.println(response);
#endif
	return strstr((char*)RXMessage->current()->buffer, response.c_str())!=NULL;
}

bool GSMInterfaceClass::isCommand(const char* cmd){
#ifdef FUNCTION_DEBUG
	Serial.println("isCommand");
#endif
	return strstr((char*)cmdMessage->current()->buffer, cmd)!=NULL;
}

void GSMInterfaceClass::resetRXBuffer(){
#ifdef FUNCTION_DEBUG
	Serial.println("resetRXBuffer");
#endif
	/*
	while(GSMSerial.available()){
		GSMSerial.read();
	}
	*/
	for(int i=0; i<RXbufferDim; i++){
		RXbuffer[i] = 0;
	}
	RXbufferDim = 0;
}

void GSMInterfaceClass::sendCmd(String cmd){
#ifdef FUNCTION_DEBUG
	Serial.println("sendCmd");
#endif
	Serial.print("TX -> ");Serial.println(cmd);
	GSMSerial.println(cmd);
	GSMSerial.flush();
	setIdleTime(100);
}
void GSMInterfaceClass::sendCmd(String cmd, String param1){
#ifdef FUNCTION_DEBUG
	Serial.println("sendCmd1");
#endif
	cmd.replace("%1", param1);
	sendCmd(cmd);
}
void GSMInterfaceClass::sendCmd(String cmd, String param1, String param2){
#ifdef FUNCTION_DEBUG
	Serial.println("sendCmd2");
#endif
	cmd.replace("%2",param2);
	sendCmd(cmd, param1);
}
void GSMInterfaceClass::sendCmd(String cmd, String param1, String param2, String param3){
#ifdef FUNCTION_DEBUG
	Serial.println("sendCmd3");
#endif
	cmd.replace("%3",param3);
	sendCmd(cmd, param1, param2);
}
void GSMInterfaceClass::sendCmd(String cmd, String param1, String param2, String param3, String param4){
#ifdef FUNCTION_DEBUG
	Serial.println("sendCmd4");
#endif
	cmd.replace("%4",param4);
	sendCmd(cmd, param1, param2, param3);
}

bool GSMInterfaceClass::isHeader(unsigned char*msg, const char*header){
#ifdef FUNCTION_DEBUG
	Serial.println("isHeader");
#endif
	return strstr((char*)msg, header) == (char*)msg;
}

unsigned char GSMInterfaceClass::getHeaderType(unsigned char*msg, unsigned short size){
#ifdef FUNCTION_DEBUG
	Serial.println("getHeaderType");
#endif
	if(isHeader(msg,"DAT")){
		return DAT_MSG_HEADER;
	}

	else if(isHeader(msg,"ACK")){
		return ACK_MSG_HEADER;
	}
	
	else if(isHeader(msg,"CMD")){
		return CMD_MSG_HEADER;
	}
	
	else if(isHeader(msg,"CAN")){
		return CAN_MSG_HEADER;
	}

	return AT_MSG_HEADER;
}

void GSMInterfaceClass::send(unsigned char type, unsigned char*msg, unsigned short size){
#ifdef FUNCTION_DEBUG
	Serial.println("send");
#endif
	if(size){
		/*
		for(int i=0; i<size; i++){
			Serial.print(msg[i],HEX);Serial.print(" ");
		}
		Serial.println();
		*/
		if(datStarted){
			TXMessage->insert(type,msg,size);
		}else{
			//Serial.println("DATA_NOT_STARTED");
		}
	}else{
		Serial.println("INVALID_PACKET");
	}
}

void GSMInterfaceClass::parseIP(Node *msg){
#ifdef FUNCTION_DEBUG
	Serial.println("parseIP");
#endif
	String IP;
	for(int i=0; i<msg->bufferDim; i++){
		IP.concat((char)msg->buffer[i]);
	}
	TCPConnectionHandler = IP.charAt(0);
	IP.remove(0,14);//remove n,REMOTE IP: 
	Settings.setClientIP(IP);
}

void GSMInterfaceClass::startCall(){
#ifdef FUNCTION_DEBUG
	Serial.println("startCall");
#endif
	cmdMessage->insert(CMD_MSG_HEADER,(unsigned char*)"STARTCALL",9);
}

void GSMInterfaceClass::endCall(){
#ifdef FUNCTION_DEBUG
	Serial.println("endCall");
#endif
	cmdMessage->insert(CMD_MSG_HEADER,(unsigned char*)"ENDCALL",7);
}

void GSMInterfaceClass::powerUp(){
	pinMode(GSM_POWER, OUTPUT); 
	digitalWrite(GSM_POWER,LOW);
	delay(1000);
	digitalWrite(GSM_POWER,HIGH);
	delay(2000);
	digitalWrite(GSM_POWER,LOW);
}

GSMInterfaceClass GSMInterface;