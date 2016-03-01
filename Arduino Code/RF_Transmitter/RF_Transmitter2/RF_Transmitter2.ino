#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include<Wire.h>

//*************************************************************************
// MPU-6050 address definition and library included
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

#define MPU6050_ACC_Sen                16384  // Sensitivity for +-2g scale
#define MPU6050_GYRO_Sen               131  // Sensitivity for +-250 degree/s scale

int16_t AcXRaw,AcYRaw,AcZRaw,TmpRaw,GyXRaw,GyYRaw,GyZRaw;   // Raw value form acc&gyro
int8_t GYRO_Config, ACC_Config;
float AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;                          //

// Define a struct for storing acc and gyro value
struct ACC_GYRO {
  float AccX = 0, AccY = 0, AccZ = 0;
  float GyroX = 0, GyroY = 0, GyroZ = 0;
};

//************************************************************************************

RF24 radio(9,10);
const uint64_t pipe = 0xE8E8F0F0E1LL;
ACC_GYRO result_read;             // define a ACC_GYRO struct to store read value

void setup() {
  // MPU6050 Setup
  Wire.begin();
  Wire.beginTransmission(MPU6050_I2C_Addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
  Serial.println("IMU Setup Ready and radio ready to send");
}

void loop(void) {
  // Read accelerometer and gyro value.
  result_read = Read_Acc_Gyro();
   
  bool flag;
  flag = radio.write(&result_read, sizeof(ACC_GYRO));
  if(flag) Serial.println("Done TX");
  else Serial.println("Fail TX");

//  delay(5);
}

/*
 * Read acceleration and angular acceleration from MPU6050
 */
ACC_GYRO Read_Acc_Gyro() {
  ACC_GYRO ans;      // Define a struct to store reading
  Wire.beginTransmission(MPU6050_I2C_Addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_I2C_Addr,14,true);  // request a total of 14 registers
  
  AcXRaw = Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcYRaw = Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZRaw = Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  TmpRaw = Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyXRaw = Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyYRaw = Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZRaw = Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  // Convert the raw data to actual value
  AcX = 1.0 * AcXRaw / MPU6050_ACC_Sen;
  AcY = 1.0 * AcYRaw / MPU6050_ACC_Sen;
  AcZ = 1.0 * AcZRaw / MPU6050_ACC_Sen;

  GyX = 1.0 * GyXRaw / MPU6050_GYRO_Sen;
  GyY = 1.0 * GyYRaw / MPU6050_GYRO_Sen;
  GyZ = 1.0 * GyZRaw / MPU6050_GYRO_Sen;

  ans.AccX = AcX;
  ans.AccY = AcY;
  ans.AccZ = AcZ;

  ans.GyroX = GyX;
  ans.GyroY = GyY;
  ans.GyroZ = GyZ;
  
  return ans;
 }
