#include <SPI.h>

const uint8_t CSPIN = PIN_PA5,\
              STARTPIN = PIN_PA6,\
              DRDYPIN = PIN_PA4;


void setupADC();
int32_t getThermocoupleRaw();

void setup() {
  pinMode(CSPIN, OUTPUT);
  pinMode(STARTPIN, OUTPUT);
  pinMode(DRDYPIN, INPUT);
  
  Serial.begin(115200);
  SPISettings spisettings = SPISettings(1000000,1,1);     // SPI Speed 1MHz, MSBFIRST, Mode 1
  SPI.beginTransaction(spisettings);
  SPI.begin();
  delay(100);
  setupADC();
}

void loop() {
  static unsigned long lastmillis=0;
  unsigned long currmillis = millis();

  if(currmillis >= (lastmillis + 250)){
    lastmillis = currmillis;
    int32_t rawadcread = getThermocoupleRaw();
    Serial.print("ADC Value: ");
    Serial.println(rawadcread);
  }
}


// -------------------------------------------------------------
// Accessory Functions
// -------------------------------------------------------------


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
  SPI.transfer(0x00);
  SPI.transfer(0x08);
  digitalWrite(CSPIN, HIGH);

  digitalWrite(CSPIN, LOW);
  SPI.transfer(0x04);             // Start conversions
  digitalWrite(CSPIN, HIGH);
}

// ADC Main Thermocouple Reading
int32_t getThermocoupleRaw(){
  int32_t outdata = 0;
  digitalWrite(CSPIN, LOW);
  SPI.transfer(0x12);             // Read Data command
  for(uint8_t i=0; i<3; i++){     // Read out 3 bytes of data into our output 32-bit int
    outdata = (SPI.transfer(0xFF) << (24 - (i << 3)));
  }
  digitalWrite(CSPIN, HIGH);
  outdata = outdata >> 8;
  return outdata;
}