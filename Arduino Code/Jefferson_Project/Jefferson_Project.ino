// For the Accelerometer
#include<Wire.h>
#include <Time.h>
#include <SPI.h>

// For the Radio
#include <SPI.h>
#include "RF24.h"
#include "printf.h"
/*************  USER Configuration *****************************/
// Hardware configuration
RF24 radio(7,8); // Set up nRF24L01 radio on SPI bus plus pins 7 & 8\
/***************************************************************/

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };   // Radio pipe addresses for the 2 nodes to communicate.

byte data[32];                           //Data buffer for testing data transfer speeds

unsigned long counter, rxTimer;          //Counter and timer for keeping track transfer info
unsigned long startTime, stopTime;  
bool TX=1,RX=0,role=0;

#define MPU6050_GYRO_CONFIG        0x1B   // R/W
#define MPU6050_ACCEL_CONFIG       0x1C   // R/W
#define MPU6050_ACCEL_XOUT_H       0x3B   // R  
#define MPU6050_ACCEL_XOUT_L       0x3C   // R  
#define MPU6050_ACCEL_YOUT_H       0x3D   // R  
#define MPU6050_ACCEL_YOUT_L       0x3E   // R  
#define MPU6050_ACCEL_ZOUT_H       0x3F   // R  
#define MPU6050_ACCEL_ZOUT_L       0x40   // R  
#define MPU6050_TEMP_OUT_H         0x41   // R  
#define MPU6050_TEMP_OUT_L         0x42   // R  
#define MPU6050_GYRO_XOUT_H        0x43   // R  
#define MPU6050_GYRO_XOUT_L        0x44   // R  
#define MPU6050_GYRO_YOUT_H        0x45   // R  
#define MPU6050_GYRO_YOUT_L        0x46   // R  
#define MPU6050_GYRO_ZOUT_H        0x47   // R  
#define MPU6050_GYRO_ZOUT_L        0x48   // R  

#define MPU6050_I2C_Addr               0x68   // I2C address of the MPU-6050

#define MPU6050_ACC_Sen                8192  // Sensitivity for +-4g scale
#define MPU6050_GYRO_Sen               131  // Sensitivity for +-250 degree/s scale

const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
void setup() {
  
  Serial.begin(57600);
  printf_begin();
  radio.begin();                           // Setup and configure rf radio
  radio.setChannel(1);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(1);                     // Ensure autoACK is enabled
  radio.setRetries(2,15);                   // Optionally, increase the delay between retries & # of retries
  radio.setCRCLength(RF24_CRC_8); 
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();                 // Start listening
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
  
  printf("\n\rJefferson_Project\n\r");
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");
  
  randomSeed(analogRead(0));              //Seed for random number generation
  
  for(int i=0; i<32; i++){
     data[i] = random(255);               //Load the buffer with random data
  }
  radio.powerUp();                        //Power up the radio

  Serial.println("i2c setup");
  Wire.begin();
  //wake up the mpu5060
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  int sleep_result = Wire.endTransmission(true);
  Serial.print("i2c wake up: "); Serial.println(sleep_result);

  // set accelerometer
  Wire.beginTransmission(MPU_addr);
  Wire.write(MPU6050_ACCEL_CONFIG);  // accelerometer config register
  Wire.write(0x08);     // set to +-4g scale
  int accel_config_result = Wire.endTransmission(true);
  // print IMU Values
  Serial.print("  i2c acc config: "); Serial.println(accel_config_result);
  Serial.println("end i2c setup\n");
  //Serial.print("Time (ms),");
  //Serial.print("AcX,");
  //Serial.print("AcY,");
  //Serial.print("AcZ,");
  //Serial.print("GyX,");
  //Serial.print("GyY,");
  //Serial.println("GyZ");

  //printf("waiting for RPi first message...");
  //int waitForMessage = 0;
  //while(waitForMessage != 123){
    //printf("waiting...\n");
    //while(radio.available()){       
      //radio.read(&data,32);
      //waitForMessage = data[0];
      //counter+=1;
    //}
    //if(millis() - rxTimer > 1000){
      //rxTimer = millis();     
      //float numBytes = (counter*32)/1000.0;
      //Serial.print("Rate: ");
      //Serial.print(numBytes);
      //printf("KB/s \n Payload Count: %d \n\r", counter);
      //printf("Data: %d \n\r", data[0]);
      //counter = 0;
    //}
  //}
  //printf("Recieved first message, begining transmission", counter);
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.stopListening();
  role = TX; // Become the primary transmitter (ping out)
}

time_t t0 = millis();
void loop() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  //Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  time_t t = millis() - t0;
  //Serial.print(t);
  //print acceleration data
  //Serial.print(","); 
  //Serial.print(AcX);
  //Serial.print(","); Serial.print(AcY);
  //Serial.print(","); Serial.print(AcZ);
  //Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  //print gyro data
  //Serial.print(","); Serial.print(GyX);
  //Serial.print(","); Serial.print(GyY);
  //Serial.print(","); Serial.println(GyZ);

  if(role == TX){ 
    //Serial.print("Transmitting...\n");
    //delay(2000);
    //printf("Initiating Basic Data Transfer\n\r");
    unsigned long cycles = 10000; //Change this to a higher or lower number.   
    startTime = millis();
    unsigned long pauseTime = millis();

    //for(int i=0; i<cycles; i++){        //Loop through a number of cycles
      data[0] = -100;  //Change the first byte of the payload for identification
      data[1] = (byte)(AcX >> 8);
      data[2] = (byte)AcX  & 0x00FF;
      //Serial.println(data[1] << 8 | data[2],DEC);
      data[3] = (byte)(AcY >> 8);
      data[4] = (byte)AcY  & 0x00FF;
      data[5] = (byte)(AcZ >> 8);
      data[6] = (byte)AcZ  & 0x00FF;
      
      data[7] = (byte)(GyX >> 8);
      data[8] = (byte)GyX  & 0x00FF;
      data[9] = (byte)(GyY >> 8);
      data[10] = (byte)GyY  & 0x00FF;
      data[11] = (byte)(GyZ >> 8);
      data[12] = (byte)GyZ  & 0x00FF;

      data[13] = t >> 32;
      data[14] = t >> 16;
      data[15] = t >> 8;
      data[16] = t & 0x00FF;
      if(!radio.writeFast(&data,32)){   //Write to the FIFO buffers        
        counter++;                      //Keep count of failed payloads
      }
    //}
    stopTime = millis();  //This should be called to wait for completion and put the radio in standby mode after transmission, returns 0 if data still in FIFO (timed out), 1 if success
  }
  
  if(role == RX){
    while(radio.available()){       
      radio.read(&data,32);
      counter++;
    }
    if(millis() - rxTimer > 1000){
      rxTimer = millis();     
      float numBytes = (counter*32)/1000.0;
      Serial.print("Rate: ");
      Serial.print(numBytes);
      printf("KB/s \n Payload Count: %d \n\r", counter);
      counter = 0;
    }
  }
  //
  // Change roles
  //
  if(Serial.available()) {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == RX ) {
      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);
      radio.stopListening();
      role = TX; // Become the primary transmitter (ping out)
    }
    else if ( c == 'R' && role == TX ) {
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]); 
      radio.startListening();
      printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n\r");      
      role = RX;                // Become the primary receiver (pong back)
    }
  } 
}
