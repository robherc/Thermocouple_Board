#include <Wire.h>
#include <SPI.h>
#include <stdbool.h>
#include "CRC.h"

const uint8_t CSPIN = PIN_PA5,\
              STARTPIN = PIN_PA6,\
              DRDYPIN = PIN_PA4;


void printTemp(uint16_t temp);
void startCJTemp();
uint16_t getCJTemp();
VOID setupADC();
int32_t getThermocoupleRaw();

void setup() {
  pinMode(CSPIN, OUTPUT);
  pinMode(STARTPIN, OUTPUT);
  pinMode(DRDYPIN, INPUT);
  
  Wire.begin();
  Serial.begin(115200);
  SPI.SPISettings(1000000,1,1);     // SPI Speed 1MHz, MSBFIRST, Mode 1
  SPI.begin();
  delay(100);
  setupADC();
}

void loop() {
  static unsigned long lastmillis=0, lastcjstart=0, lastcjtemp=0;
  static bool cjinprogress=0;

  unsigned long currmillis = millis();

  if(currmillis >= (lastmillis + 10)){
    lastmillis = currmillis;
    int32_t rawadcread = getThermocoupleRaw();
    Serial.print(ADC Value: );
    Serial.println(rawadcread);
    if(cjinprogress && currmillis >= (lastcjstart + 10)){
      uint16_t cjtemp = getCJTemp();
      printTemp(cjtemp);
      lastcjtemp = currmillis;
      cjinprogress = 0;
    }else if(currmillis >= lastcjtemp + 1000){
      lastcjstart = currmillis;
      cjinprogress = 1;
      startCJTemp();
    }
  }
}


// -------------------------------------------------------------
// Accessory Functions
// -------------------------------------------------------------

// Debug function -- to be removed in final software
void printTemp(uint16_t temp){
  Serial.print("Temperature: ");
  float outTemp = temp;
  outTemp *= 175;
  outTemp /= 65535;
  outTemp -= 45;
  Serial.print(outTemp);
  Serial.println("*C");
}


// CJ Temperature Sensor
// -------------------------------------------------------------

// Start Reading
void startCJTemp(){
  Wire.beginTransmission(70);
  Wire.write(0xFD);
  Wire.endTransmission();
}

// Retrieve Reading Data
uint16_t getCJTemp(){
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
      return readtemp;
    }else{
      Serial.println("CRC FAILURE");
      return 0;
    }
  }
}


// ADC
// -------------------------------------------------------------

// Set the ADC parameters
void setupADC(){
  digitalWrite(STARTPIN, HIGH);
  digitalWrite(CSPIN, LOW);
  SPI.transfer(0x06);             // Reset ADC
  digitalWrite(CSPIN, HIGH);
  delay(1);
  digitalWrite(CSPIN, LOW);
  SPI.transfer(0x16);             // SDATAC (new values don't interrup output data during a read)

  SPI.transfer(0x40);             // Reg Write, starting at 0x0
  SPI.transfer(0x03);             // write 4 bytes
  SPI.transfer(0x01);
  SPI.transfer(0x00);
  SPI.transfer(0x03);
  SPI.transfer(0x42);
  digitalWrite(CSPIN, HIGH);

  digitalWrite(CSPIN, LOW);
  SPI.transfer(0x04);             // Start conversions
  digitalWrite(CSPIN, HIGH);
}

// ADC Main Thermocouple Reading
int32_t getThermocoupleRaw(){
  int32_t outdata = 0;
  digitalWrite(CSPIN, LOW);
  SPI.transfer(0x12h);            // Read Data command
  for(uint8_t i=0; i<3; i++){     // Read out 3 bytes of data into our output 32-bit int
    outdata = (SPI.transfer(0xFF) << (24 - (8 * i)));
  }
  digitalWrite(CSPIN, HIGH);
  outdata == outdata >> 8;
  return outdata;
}