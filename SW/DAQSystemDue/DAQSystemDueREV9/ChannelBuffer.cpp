#include "ChannelBuffer.h"

void ChannelBufferClass::init(){
	packetIndex = 0;
	bufferSize = 0;
	rawSize = 0;
	buffer = new unsigned char*[Settings.getChannelNum()];
	channelUpdated = new unsigned char[Settings.getChannelNum()];

	for(int i=0; i<Settings.getChannelNum(); i++){
		buffer[i] = new unsigned char[Settings.getChannelParams(i)->dim];
		memset(buffer[i], 0, Settings.getChannelParams(i)->dim);
		bufferSize += Settings.getChannelParams(i)->dim;
	}

	rawBuffer = new unsigned char[4 + 2*Settings.getChannelNum() + bufferSize];
	
	reset();
	Serial.println("CHANNEL_BUFFER_CREATED");
}

String ChannelBufferClass::getValue(unsigned short id){
	//char *mem = new char[sizeof(long long int)];
	//if(mem == NULL)Serial.println("Impossibile creare buffer di memorizzazione");
	char mem[8];
	int index = Settings.getChannelIndex(id);
	long int res = 0;
	ChannelConvParams* params = Settings.getChannelParams(index);
	String result;

	//Serial.println("Inizio conversione");
	switch(params->dataType){
		case STRING_CONV:
			//Serial.println("STRING_CONV");
			for(int i=0; i<params->dim; i++){
				result.concat((char)buffer[index][i]);
			}
			break;

		case BITFLAG_CONV:
			//Serial.println("BITFLAG_CONV");
			for(int i=0; i<params->dim; i++){
				res += (buffer[index][i]<<i);
			}
			result = String(res,BIN);
			break;

		case SIGN_NUMBER_CONV:
			//Serial.println("SIGN_NUMBER");
			for(int i=0; i<params->dim; i++){
				mem[i] = buffer[index][i];
			}

			if(mem[params->dim-1] & (1<<7)){
				for(int i=params->dim; i<sizeof(long long int); i++){
					mem[i] = 0xFF;
				}
			}else{
				for(int i=params->dim; i<sizeof(long long int); i++){
					mem[i] = 0x00;
				}
			}
			
			result = String(convert(*(reinterpret_cast<long long int*>(mem)), params),10);
			break;

		case UNSIGN_NUMBER_CONV:
			//Serial.println("UNSIGN_NUMBER_CONV");
			for(int i=0; i<params->dim; i++){
				mem[i] = buffer[index][i];
			}

			for(int i=params->dim; i<sizeof(long long int); i++){
				mem[i] = 0x00;
			}
			result = String(convert(*(reinterpret_cast<unsigned long long int*>(mem)), params),10);
			break;
	}
	//delete[] mem;
	//Serial.println("Fine conversione");
	return result;
}

void ChannelBufferClass::setValue(unsigned short id, unsigned char*data){
	int index = Settings.getChannelIndex(id);
	if(index>=0){
		memcpy(buffer[index], data, Settings.getChannelParams(index)->dim);
		channelUpdated[index] = 1;
	}
}

void ChannelBufferClass::reset(){
	//int size;
	rawSize = 0;
	for(int i=0; i<Settings.getChannelNum(); i++){
		/*
		size = Settings.getChannelParams(i)->dim;
		for(int j=0; j<size; j++){
			buffer[i][j] = 0x00;
		}*/
		channelUpdated[i] = 0;
	}
}

void ChannelBufferClass::debug(){
	int size;
	Serial.println("//////////CHANNEL_BUFFER_DEBUG_START//////////");
	Serial.print("SIZE ->");Serial.println(bufferSize);
	for(int i=0; i<Settings.getChannelNum(); i++){
		Serial.print("0x");
		Serial.print(Settings.getChannelParams(i)->canID, HEX);
		Serial.print(" -> ");
		size = Settings.getChannelParams(i)->dim;
		for(int j=0; j<size; j++){
			Serial.print(buffer[i][j], HEX);
			Serial.print(" ");
		}
		Serial.println();
	}
	Serial.println("//////////CHANNEL_BUFFER_DEBUG_END//////////");
}

double ChannelBufferClass::convert(double value, ChannelConvParams *params){
	switch(params->convType){
		case XpA:
			value += params->a;
		break;

		case XdA:
			if(params->a != 0)
				value /= params->a;
		break;

		case XpAdB:
			if(params->b != 0)
				value = (value+params->a)/params->b;
		break;

		case XpAdBpC:
			if(params->b != 0)
				value = (value+params->a)/params->b+params->c;
		break;
	}

	return value;
}

unsigned char* ChannelBufferClass::getRawData(){
	unsigned short i,j,index,canID,cCount = 0;
	ChannelConvParams* params;

	//packet index + channelCount + can IDs + data 
	rawSize = 4;
	for(int i=0; i<Settings.getChannelNum(); i++){
		if(channelUpdated[i]){
			rawSize += 2 + Settings.getChannelParams(i)->dim;
			cCount++;
		}
	}
	//Ci sono dati da inviare oltre all'header
	if(rawSize>4){
	
		if(rawBuffer){
			rawBuffer[0] = packetIndex>>8;
			rawBuffer[1] = packetIndex;
			rawBuffer[2] = cCount>>8;
			rawBuffer[3] = cCount;

			index = 4;
			for(i=0; i<Settings.getChannelNum(); i++){
				if(channelUpdated[i]){
					params = Settings.getChannelParams(i);
					rawBuffer[index] = params->canID>>8;
					rawBuffer[index+1] = params->canID;
					index+=2;
				}
			}

			for(i=0; i<Settings.getChannelNum(); i++){
				if(channelUpdated[i]){
					params = Settings.getChannelParams(i);
					memcpy(&(rawBuffer[index]), buffer[i], params->dim);
					index+=params->dim;
				}
			}

			return rawBuffer;
		}else{
			Serial.println("BUFFER -> Memoria esaurita");
		}
	}

	rawSize = 0;
	return NULL;
}

ChannelBufferClass ChannelBuffer;

