//Serial Relay - Arduino will patch a 
//serial link between the computer and the GSMSerial Shield
//at 19200 bps 8-N-1
//Computer is connected to Hardware UART
//GSMSerial Shield is connected to the Software UART 
#define GSMSerial Serial1


void setup()
{
 GSMSerial.begin(9600);               // the GSMSerial baud rate   
 Serial.begin(9600);             // the Serial port of Arduino baud rate.
}
void loop()
{
 
 if (GSMSerial.available())              // if date is comming from softwareserial port ==> data is comming from GSMSerial shield
  Serial.write(GSMSerial.read());            // if no data transmission ends, write buffer to hardware serial port

 
 if (Serial.available())            // if data is available on hardwareserial port ==> data is comming from PC or notebook
   GSMSerial.write(Serial.read());       // write it to the GSMSerial shield
}

