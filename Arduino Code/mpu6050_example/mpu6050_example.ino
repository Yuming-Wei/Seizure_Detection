// MPU-6050 Short Example Sketch
// By Arduino User JohnChi
// August 17, 2014
// Public Domain
#include<Wire.h>
#include <Time.h>

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
  Serial.println("i2c setup");
  Wire.begin();
  //wake up the mpu5060
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  int sleep_result = Wire.endTransmission(true);
  Serial.print("i2c wake up: "); Serial.println(sleep_result);

  //set accelerometer
  Wire.beginTransmission(MPU_addr);
  Wire.write(MPU6050_ACCEL_CONFIG);  // accelerometer config register
  Wire.write(0x08);     // set to +-4g scale
  int accel_config_result = Wire.endTransmission(true);
  Serial.print("  i2c acc config: "); Serial.println(accel_config_result);
  Serial.println("end i2c setup\n");
  Serial.print("Time (ms),");
  Serial.print("AcX,");
  Serial.print("AcY,");
  Serial.print("AcZ,");
  Serial.print("GyX,");
  Serial.print("GyY,");
  Serial.println("GyZ");
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
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  time_t t = millis() - t0;
  Serial.print(t);
  //print acceleration data
  Serial.print(","); Serial.print(AcX);
  Serial.print(","); Serial.print(AcY);
  Serial.print(","); Serial.print(AcZ);
  //Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
  //print gyro data
  Serial.print(","); Serial.print(GyX);
  Serial.print(","); Serial.print(GyY);
  Serial.print(","); Serial.println(GyZ);
}

