#include "ChannelBuffer.h"
#include "List.h"
#include "Settings.h"
#include "ChannelBuffer.h"
#include "SDInterface.h"
#include "CANInterface.h"
#include "GSMInterface.h"
#include "DueTimer.h"

#include <SPI.h>
#include <SD.h>

#define INV_STEER_RAD 0x501
#define STEER_ANGLE 0x502

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;

#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}

// Angolo utilizzato per la taratura
// Con un braccio sterzo-sensore di 0.098m
// questo angolo corrisponde a d = 0.017m
#define   RefAngle    10.0
#define   RefDist     0.017
#define   SensorB     0.098
#define   Passo       1.600
#define   Carreggiata 0.520
#define	  Centrale	  755
#define   Dist_dx     633
#define   Dist_sx     870

int32_t steerAvg = 0, steerAngle;
signed short steerValue = 0;
uint16_t steerCount = 0;
volatile bool steerFlag = false;
//===============================================


unsigned long long int saveTimer;
unsigned short int count;
void steerSensorRoutine();

void setup(){
	Serial.begin(19200);
	while(!Serial);

	pinMode(CAN_STATUS_LED, OUTPUT);
	pinMode(SD_STATUS_LED, OUTPUT);
	pinMode(GSM_STATUS_LED, OUTPUT);
	pinMode(13,INPUT);

	delay(1000);
	digitalWrite(CAN_STATUS_LED, LOW);
	digitalWrite(SD_STATUS_LED, LOW);
	digitalWrite(GSM_STATUS_LED, LOW);

	SDInterface.init(SD_CSPIN);
	if(SDInterface.available){
		digitalWrite(SD_STATUS_LED, HIGH);

		CANInterface.init();
		ChannelBuffer.init();
		digitalWrite(CAN_STATUS_LED, HIGH);

		GSMInterface.init();
		Serial.println();

		saveTimer = millis();
	}else{
		while(1);
	}

	count = 0;

	uint32_t wdp_ms = 10 * 256;
	
	WDT_Enable(WDT, 0x2000 | wdp_ms | ( wdp_ms << 16 ));

	Timer7.setPeriod(1000).attachInterrupt(steerSensorRoutine).start();
}

int waitTime = 0;
void loop(){
	// Restart watchdog
	WDT_Restart(WDT);

	//Serial.println("read from can");
	CANInterface.readPacket();
	GSMInterface.update();

	if(millis()-saveTimer>1000/Settings.getPacketPerSecond()){
		ChannelBuffer.updatePacketIndex();
		SDInterface.saveLog();
		
		/*
		int value = (int)(sinf(ChannelBuffer.getPacketIndex())*1000000);
		ChannelBuffer.setValue(0x100, (unsigned char*)(&value));
		*/

		//ChannelBuffer.debug();
		GSMInterface.send(DAT_MSG_HEADER, ChannelBuffer.getRawData(), ChannelBuffer.getRawSize());
		
		ChannelBuffer.reset();
		//Serial.print("Memoria rimasta ");Serial.println(freeMemory());
		

		saveTimer = millis();
	}

	//Steer Routine
	if(steerFlag){
		steerFlag = false;
		/*
		CANInterface.sendPacket(INV_STEER_RAD, (char*)&steerValue, 2);
		ChannelBuffer.setValue(INV_STEER_RAD, (unsigned char*)&steerValue);
		*/
		CANInterface.sendPacket(STEER_ANGLE, (char*)&steerAngle, 4);
		ChannelBuffer.setValue(STEER_ANGLE, (unsigned char*)&steerAngle);
	}
	
}





void steerSensorRoutine(){
	steerAvg += analogRead(A0);
	steerCount++;

	// Calcolo di TrackRadiusInv
	if(steerCount>=500){
		float m;
		steerAvg /= steerCount;
		//Calcolo dell'angolo
		if (steerAvg>=Centrale){
			// angolo positivo
			m = ((float)RefDist)/(float)((float)Dist_dx-(float)Centrale);
		}else{
			// angolo negativo
			m = ((float)RefDist)/(float)((float)Centrale-(float)Dist_sx);
		}
	
		float q = -m*(float)Centrale;
		float Dist = ((m*((float)steerAvg)+q));
		float Angle = asin(Dist/SensorB);
		steerAngle = Angle * 10000;
		float tgangle = tan(Angle);

		if(tgangle == 0){
			// allora vado dritto
			steerValue = 0;
		}else{
			// sto curvando
			float radius = sqrt(Passo*Passo + pow((Carreggiata/2.0)+(Passo/tgangle), 2));
			steerValue = (float)(1000.0/radius);
		}

		/*
		Serial.print("Angle  ");Serial.println(Angle/3.14*180.0);
		Serial.print("Avg  ");Serial.println(steerAvg);
		Serial.print("Final  ");Serial.println(steerValue);
		*/
		steerAvg = 0;
		steerCount = 0;
		steerFlag = true;
	}

}


/*
int x = 0;
long long int val;

double lat[]={
44.61021423,
44.60902405,
44.61042447,
44.61051994,
44.61057722,
44.61058868,
44.61063451,
44.61060014,
44.61056195,
44.61048557,
44.61041301,
44.61038246,
44.61032899,
44.6101686,
44.61013041,
44.61002348,
44.61000056,
44.6097867
};

double lon[]={
10.91294289,
10.91785622,
10.91310382,
10.91332376,
10.9135437,
10.91371,
10.91391385,
10.91413915,
10.91435373,
10.91462195,
10.91498137,
10.91520667,
10.91556072,
10.91618299,
10.91651559,
10.9168911,
10.91722369,
10.91755629
};

/*
#define GSMSerial Serial3
unsigned char buffer[64]; // buffer array for data recieve over serial port
int count=0;     // counter for buffer array 
void setup()
{
 GSMSerial.begin(19200);               // the GSMSerial baud rate   
 Serial.begin(19200);             // the Serial port of Arduino baud rate.
}
void loop()
{
 if (GSMSerial.available())              // if date is comming from softwareserial port ==> data is comming from GSMSerial shield
 {
   while(GSMSerial.available())          // reading data into char array 
   {
     buffer[count++]=GSMSerial.read();     // writing data into array
     if(count == 64)break;
 }
   Serial.write(buffer,count);            // if no data transmission ends, write buffer to hardware serial port
   clearBufferArray();              // call clearBufferArray function to clear the storaged data from the array
   count = 0;                       // set counter of while loop to zero
 }
 if (Serial.available())            // if data is available on hardwareserial port ==> data is comming from PC or notebook
   GSMSerial.write(Serial.read());       // write it to the GSMSerial shield
}
void clearBufferArray()              // function to clear buffer array
{
 for (int i=0; i<count;i++)
   { buffer[i]=NULL;}                  // clear all index of array with command NULL
}
*/