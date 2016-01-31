#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// Define a struct for storing acc and gyro value
struct ACC_GYRO {
  float AccX = 0, AccY = 0, AccZ = 0;
  float GyroX = 0, GyroY = 0, GyroZ = 0;
};

RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup(void) {
 Serial.begin(57600);
 radio.begin();
 radio.openReadingPipe(1,pipe);
 radio.startListening();
}

void loop(void) {
   ACC_GYRO val;
  if (radio.available()) {
    bool done = false;    
    while (!done) {
      done = radio.read(&val, sizeof(ACC_GYRO));
      Serial.print(" |AccX = "); Serial.print(val.AccX,3);
      Serial.print(" |AccY = "); Serial.print(val.AccY,3);
      Serial.print(" |AccZ = "); Serial.print(val.AccZ,3);
      Serial.print(" |GyroX = "); Serial.print(val.GyroX,3);
      Serial.print(" |GyroY = "); Serial.print(val.GyroY,3);
      Serial.print(" |GyroZ = "); Serial.println(val.GyroZ,3);
    }
  } else Serial.println("No radio available");
}
