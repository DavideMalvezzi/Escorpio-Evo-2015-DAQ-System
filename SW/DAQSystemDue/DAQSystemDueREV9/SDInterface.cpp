#include "SDInterface.h"
//#define SD_DEBUG ON

void SDInterfaceClass::init(uint8_t CSPin){
	pinMode(CSPin, OUTPUT);
	timeStep = 0;

	if(SD.begin(CSPin)){
		loadGSMSettings();
		loadCANSettings();
		Settings.debug();
	}else{
		Serial.println("SD_NOT_INSERTED");
		Serial.println("ARDUINO_STOP");
		while(1);
	}
		
	//Estensione omessa, ma dovrebbe funzionare lo stesso
	//O_CREAT | O_WRITE velocizza la scritto x100 -> http://forum.arduino.cc/index.php/topic,49649.0.html
		
}

void SDInterfaceClass::loadGSMSettings(){
	String temp;
	if(SD.exists(GSM_CONFIG_FILE)){
		File config = SD.open(GSM_CONFIG_FILE, O_READ);
		
		Settings.setApnName(parseNextConfig(config));
		Settings.setUserName(parseNextConfig(config));
		Settings.setUserPsw(parseNextConfig(config));
		Settings.setServerPort(parseNextConfig(config));
		Settings.setUDPPort(parseNextConfig(config));
		Settings.setReceiverNum(parseNextConfig(config));
		Settings.setSenderNum(parseNextConfig(config));
		parseNextConfig(config);//Rimuove pin ricevitore
		Settings.setPin(parseNextConfig(config));

		Settings.setPacketPerSecond(parseNextConfig(config).toInt());
		Settings.setPacketTimeOut(parseNextConfig(config).toInt());
		Settings.setPTTRequestChannel(parseNextConfig(config).toInt());
		Settings.setPTTStatusChannel(parseNextConfig(config).toInt());


		config.close();
		
		Serial.println("SD_GSM_CONFIG_LOADED");
		Settings.setGsmConfigValid(true);
	}else{
		Serial.println("SD_GSM_CONFIG_FILE_MISSING");
		Settings.setGsmConfigValid(false);
	}
}

String SDInterfaceClass::parseNextConfig(File& file){
	String value;
	value = file.readStringUntil('\n');
	value = value.substring(value.indexOf('=')+1);
	value.trim();
	return value;
}

void SDInterfaceClass::loadCANSettings(){
	String temp, *header;
	ChannelConvParams *params;
	if(SD.exists(CAN_CONFIG_FILE)){
		File config = SD.open(CAN_CONFIG_FILE, O_READ);

		Settings.setChannelNum(config.parseInt());
		config.read(); //remove \n
		
		header = new String[Settings.getChannelNum()];
		for(int i=0; i<Settings.getChannelNum(); i++){
			params = new ChannelConvParams;
			
			params->canID = (unsigned short)config.readStringUntil(',').toInt();
			header[i] = config.readStringUntil(','); //remove name
			params->dataType = (unsigned char)config.readStringUntil(',').toInt();
			params->dim = (unsigned char)config.readStringUntil(',').toInt();
			config.read();config.read();//remove , I/O ,
			params->convType = (unsigned char)config.readStringUntil(',').toInt();
			params->a = config.readStringUntil(',').toFloat();
			params->b = config.readStringUntil(',').toFloat();
			params->c = config.readStringUntil(',').toFloat();
			config.read(); //remove \n

			Settings.addChannelParams(params, i);
		}

		Serial.println("SD_CAN_CONFIG_LOADED");
		Settings.setCanConfigValid(true);
		createNewLogFile(header);
		config.close();
	}else{
		Serial.println("SD_CAN_CONFIG_FILE_MISSING");
		Settings.setCanConfigValid(false);
	}
}

void SDInterfaceClass::createNewLogFile(String *header){
	unsigned int lastLogFileNum;
	File last;
	if(SD.exists(LAST_LOG_FILE)){
		last = SD.open(LAST_LOG_FILE, O_READ);
		lastLogFileNum = last.parseInt();
		lastLogFileNum++;
		last.close();
		SD.remove(LAST_LOG_FILE);
	}else{
		lastLogFileNum = 1;
	}

	last = SD.open(LAST_LOG_FILE, O_CREAT | O_WRITE);
	last.print(lastLogFileNum);
	last.close();

	(String(LOG_FILE) + String(lastLogFileNum) + String(".csv")).toCharArray(log_filename, 16);
	last = SD.open(log_filename , O_CREAT | O_WRITE);
	last.print(Settings.getChannelNum());
	last.print(";");
	for(int i=0; i<Settings.getChannelNum(); i++){
		last.print(Settings.getChannelParams(i)->canID);
		last.print(";");
	}
	last.println();

	last.print("Index;");
	for(int i=0; i<Settings.getChannelNum(); i++){
		last.print(header[i]);
		last.print(";");
	}
	last.println();
    
	last.close();

	Serial.println("SD_LOG_FILE_CREATED");
	available = true;
}

void SDInterfaceClass::saveLog(){
	#ifdef SD_DEBUG 
		Serial.println("/////////SD_DEBUG_START//////////");
	#endif
	//Serial.println("SD OPEN");
	File log = SD.open(log_filename, O_WRITE);
	
	//Serial.println("SD PRINT1");
	log.print(ChannelBuffer.getPacketIndex());
	log.print(";");
	
	
	for(int i=0; i<Settings.getChannelNum(); i++){
		#ifdef SD_DEBUG 
			Serial.print("0x");Serial.print(Settings.getChannelParams(i)->CanID, HEX);
			Serial.print(" -> ");
			Serial.println(ChannelBuffer.getValue(Settings.getChannelParams(i)->CanID));
		#endif
		//Serial.println("SD PRINT CHANNELS");
		log.print(ChannelBuffer.getValue(Settings.getChannelParams(i)->canID));
		log.print(";");
	}
	//Serial.println("SD CLOSE");
	log.println();
	log.close();
	#ifdef SD_DEBUG 
		Serial.println("/////////SD_DEBUG_END//////////");
	#endif
}


SDInterfaceClass SDInterface;

