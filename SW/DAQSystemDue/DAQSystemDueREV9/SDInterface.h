// SDInterface.h

#ifndef _SDINTERFACE_h
#define _SDINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <SPI.h>
#include <SD.h>

#include "Settings.h"
#include "ChannelBuffer.h"

#define GSM_CONFIG_FILE "GsmConf.txt"
#define CAN_CONFIG_FILE "CanConf.txt"
#define LAST_LOG_FILE "LastLog.txt"
#define LOG_FILE "LOG"

class SDInterfaceClass{
 private:
	 char log_filename[16];
	 unsigned int timeStep;
	 
	 void loadGSMSettings();
	 void loadCANSettings();
	 void createNewLogFile(String *header);
	 String parseNextConfig(File&);
 public:
	 boolean available;
	 void init(uint8_t CSPin);
	 void saveLog();

};

extern SDInterfaceClass SDInterface;

#endif

