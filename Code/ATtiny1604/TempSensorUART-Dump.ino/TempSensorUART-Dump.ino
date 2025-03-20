#include <Wire.h>
#include "CRC.h"


void printTemp(uint16_t temp);

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  delay(5000);
}

void loop() {
  Wire.beginTransmission(70);
  Wire.write(0xFD);
  Wire.endTransmission();
  delay(10);
  Wire.requestFrom(70, 3, 1);
  uint8_t i = 0, inbytes[3];
  uint16_t readtemp = 0;
  while(Wire.available()){
    if(i<3){
      inbytes[i] = Wire.read();
    }else{
      Wire.read();
    }
    i++;
  }
  readtemp = inbytes[0] << 8;
  readtemp += inbytes[1];
  if(readtemp){
    if(calcCRC8(inbytes, 2, 0x31, 0xFF, 0, 0, 0) == inbytes[2]){
      printTemp(readtemp);
    }else{
      Serial.println("CRC FAILURE");
    }
  }

  delay(1000);
}


// Accessory Functions
void printTemp(uint16_t temp){
  Serial.print("Temperature: ");
  float outTemp = temp;
  outTemp *= 175;
  outTemp /= 65535;
  outTemp -= 45;
  Serial.print(outTemp);
  Serial.println("*C");
}