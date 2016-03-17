// MPU-6050 Sketch
#include<Wire.h>

#define MPU6050_CONFIG             0x1A   // R/W
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
#define MPU6050_PWR_MGMT_1         0x6B   // R/W

#define MPU6050_I2C_Addr           0x68   // I2C address of the MPU-6050

#define MPU6050_ACC_Sen                8192  // Sensitivity for +-4g scale
#define MPU6050_GYRO_Sen               131  // Sensitivity for +-250 degree/s scale

int16_t AcXRaw,AcYRaw,AcZRaw,TmpRaw,GyXRaw,GyYRaw,GyZRaw;
int8_t GYRO_Config, ACC_Config, ME;
float AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int LAcX,LAcY,LAcZ,LGyX,LGyY,LGyZ;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU6050_I2C_Addr);
  Wire.write(MPU6050_PWR_MGMT_1);  // PWR_MGMT_1 register
  Wire.write(0x00);      // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(false);

  Wire.beginTransmission(MPU6050_I2C_Addr);
  Wire.write(MPU6050_ACCEL_CONFIG);  // Accelerometer Configuration register
  Wire.write(0x08);       // set scale of ACC
  Wire.endTransmission(true);

  Serial.begin(9600);
}
void loop(){
  //print_ACC_GYRO();
  DB25_ACC_Gyro();
}

/*
 * Read accelerometer data and gyro data then send through DB25 pin to NK
 */
 void DB25_ACC_Gyro() {
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

  // Convert the actual data portional to 7-bit(in total 8 bit, 1 bit for sign) distribution of 5V
  if(AcX > 0) LAcX = abs(AcX)/2*128 + 128;
  else LAcX = abs(AcX)/2*128;
  if(AcY > 0) LAcY = abs(AcY)/2*128 + 128;
  else LAcY = abs(AcY)/2*128;
  if(AcZ > 0) LAcZ = abs(AcZ)/2*128 + 128;
  else LAcZ = abs(AcZ)/2*128;

  if(GyX > 0) LGyX = abs(GyX)/2*128 + 128;
  else LGyX = abs(GyX)/2*128;
  if(GyY > 0) LGyY = abs(GyY)/2*128 + 128;
  else LGyY = abs(GyY)/2*128;
  if(GyZ > 0) LGyZ = abs(GyZ)/2*128 + 128;
  else LGyZ = abs(GyZ)/2*128;

  Serial.print("AcX = "); Serial.print(AcX, 4); Serial.print("g"); 
  Serial.print("| AcY = "); Serial.print(AcY, 4); Serial.print("g"); 
  Serial.print("| AcZ = "); Serial.print(AcZ, 4); Serial.print("g"); 
  Serial.print(" | GyX = "); Serial.print(GyX, 4); Serial.print("dps"); 
  Serial.print(" | GyY = "); Serial.print(GyY, 4); Serial.print("dps"); 
  Serial.print(" | GyZ = "); Serial.print(GyZ, 4); Serial.println("dps"); 
  
  Serial.print("LAcX = "); Serial.print(LAcX);
  Serial.print("| LAcY = "); Serial.print(LAcY); 
  Serial.print("| LAcZ = "); Serial.print(LAcZ); 
  Serial.print(" | LGyX = "); Serial.print(LGyX);
  Serial.print(" | LGyY = "); Serial.print(LGyY);
  Serial.print(" | LGyZ = "); Serial.println(LGyZ);
  Serial.println("");
  delay(100);
  
 }
 
/*
 * Read accelerometer, temperature and gyro repectively, start from 0x3B
 * Data will be sent to Serial
 */
void print_ACC_GYRO() {
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
  
  AcX = 1.0 * AcXRaw / MPU6050_ACC_Sen;
  AcY = 1.0 * AcYRaw / MPU6050_ACC_Sen;
  AcZ = 1.0 * AcZRaw / MPU6050_ACC_Sen;

  GyX = 1.0 * GyXRaw / MPU6050_GYRO_Sen;
  GyY = 1.0 * GyYRaw / MPU6050_GYRO_Sen;
  GyZ = 1.0 * GyZRaw / MPU6050_GYRO_Sen;
  
  Serial.print("AcX = "); Serial.print(AcX, 4); 
  Serial.print("| AcY = "); Serial.print(AcY, 4); 
  Serial.print("| AcZ = "); Serial.print(AcZ, 4); 
  Serial.print(" | GyX = "); Serial.print(GyX, 4);
  Serial.print(" | GyY = "); Serial.print(GyY, 4);
  Serial.print(" | GyZ = "); Serial.println(GyZ, 4);
  delay(100);
}

void print_Me() {
  /*
   *First read Who Am I Register, which is 0x75
   */
  Wire.beginTransmission(MPU6050_I2C_Addr);
  Wire.write(0x75);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_I2C_Addr,1,true);  // request a total of 1 registers
  ME = Wire.read();
  Serial.print("I am: ");
  Serial.print(ME, BIN);
  delay(100);
}
void print_Config() {
  /*
   * Read the gyro and accelerometer setup
   */
   Wire.beginTransmission(MPU6050_I2C_Addr);
   Wire.write(MPU6050_GYRO_CONFIG);  // starting with register 0x3B (ACCEL_XOUT_H)
   Wire.endTransmission(false);
   Wire.requestFrom(MPU6050_I2C_Addr,2,true);  // request a total of 1 registers
   GYRO_Config = Wire.read();
   ACC_Config = Wire.read();
   Serial.print("GYRO_Config: ");
   Serial.print(GYRO_Config, BIN);
   Serial.print("|ACC_Config: ");
   Serial.println(ACC_Config, BIN);
   delay(100);
}
