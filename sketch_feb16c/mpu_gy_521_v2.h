#ifndef MPU_GY_521_V2_H
#define MPU_GY_521_V2_H

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include "limits.h"

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)

#define X_MIN 112 
#define X_MAX 156
#define Y_MIN -10
#define Y_MAX -9
#define Z_MIN -2
#define Z_MAX 2

bool blinkState = false;

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() 
{
    mpuInterrupt = true;
}

class mpu521 : public MPU6050
{
private:
  	float gyrosMinX, gyrosMinY, gyrosMinZ;
  	float gyrosMaxX, gyrosMaxY, gyrosMaxZ;
  
  	float acceloMinX, acceloMinY, acceloMinZ;
  	float acceloMaxX, acceloMaxY, acceloMaxZ;
    
    float _gyrosX, _gyrosY, _gyrosZ;
    float _acceloX, _acceloY, _acceloZ;
    
    // MPU control/status vars
    bool dmpReady = false;  // set true if DMP init was successful
    uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
    uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
    uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
    uint16_t fifoCount;     // count of all bytes currently in FIFO
    uint8_t fifoBuffer[64]; // FIFO storage buffer
    
    // orientation/motion vars
    Quaternion q;           // [w, x, y, z]         quaternion container
    VectorInt16 aa;         // [x, y, z]            accel sensor measurements
    VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
    VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
    VectorFloat gravity;    // [x, y, z]            gravity vector
	  float euler[3];         // [psi, theta, phi]    Euler angle container
    float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

public:
    void initialization();
    
    void recordValues();
    float acceloX();
    float acceloY();
    float acceloZ();
    float gyrosX();
    float gyrosY();
    float gyrosZ();

    void printValues();
    void printBoundaries();
};
void mpu521::initialization()
{
    // join I2C bus (I2Cdev library doesn't do this automatically)
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties

    initialize();
    pinMode(INTERRUPT_PIN, INPUT);

    // load and configure the DMP
    devStatus = dmpInitialize();

    // supply your own gyro offsets here, scaled for min sensitivity
    setXGyroOffset(220);
    setYGyroOffset(76);
    setZGyroOffset(-85);
    setZAccelOffset(1788); // 1688 factory default for my test chip

    setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = getIntStatus();

    dmpReady = true;
    packetSize = dmpGetFIFOPacketSize();

    // configure LED for output
    pinMode(LED_PIN, OUTPUT);

    // initialise the values for minimum and maximum - Gyroscope
    gyrosMinX = gyrosMinY = gyrosMinZ = INT_MAX;
    gyrosMaxX = gyrosMaxY = gyrosMaxZ = INT_MIN;

    // initialise the values for minimum and maximum - Accelerometer
    acceloMinX = acceloMinY = acceloMinZ = INT_MAX;
    acceloMaxX = acceloMaxY = acceloMaxZ = INT_MIN;
}
void mpu521::recordValues()
{
    // wait for MPU interrupt or extra packet(s) available
    while (!mpuInterrupt && fifoCount < packetSize) 
    { 
        if (mpuInterrupt && fifoCount < packetSize) 
        {
          // try to get out of the infinite loop
          fifoCount = getFIFOCount();
        }  
    }

    // reset interrupt flag and get INT_STATUS byte
    mpuInterrupt = false;
    mpuIntStatus = getIntStatus();

    // get current FIFO count
    fifoCount = getFIFOCount();

    if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) 
    {
        // wait for correct available data length, should be a VERY short wait
        while (fifoCount < packetSize) 
        {
            fifoCount = getFIFOCount();
        }

        // read a packet from FIFO
        getFIFOBytes(fifoBuffer, packetSize);
        
        // track FIFO count here in case there is > 1 packet available
        // (this lets us immediately read more without waiting for an interrupt)
        fifoCount -= packetSize;

        // display initial world-frame acceleration, adjusted to remove gravity
        // and rotated based on known orientation from quaternion
        dmpGetQuaternion(&q, fifoBuffer);
        dmpGetAccel(&aa, fifoBuffer);
        dmpGetGravity(&gravity, &q);
        dmpGetLinearAccel(&aaReal, &aa, &gravity);
        dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);

        // update boundaries for accelerometer
        _acceloX = aaWorld.x;
        _acceloY = aaWorld.y;
        _acceloZ = aaWorld.z;
    
        if (acceloMinX > _acceloX)
          acceloMinX = _acceloX;
        if (acceloMaxX < _acceloX)
          acceloMaxX = _acceloX;
    
        if (acceloMinY > _acceloY)
          acceloMinY = _acceloY;
        if (acceloMaxY < _acceloY)
          acceloMaxY = _acceloY;
        
        if (acceloMinZ > _acceloZ)
          acceloMinZ = _acceloZ;
        if (acceloMaxZ < _acceloZ)
          acceloMaxZ = _acceloZ;


        /*
        dmpGetEuler(euler, &q);
        _gyrosX = euler[0] * 180 / M_PI;
        _gyrosY = euler[1] * 180 / M_PI;
        _gyrosZ = euler[2] * 180 / M_PI;
        */

        dmpGetQuaternion(&q, fifoBuffer);
        dmpGetGravity(&gravity, &q);
        dmpGetYawPitchRoll(ypr, &q, &gravity);
        _gyrosX = ypr[0] * 180.0 / M_PI;
        _gyrosY = ypr[1] * 180.0 / M_PI;
        _gyrosZ = ypr[2] * 180.0 / M_PI;

        if (gyrosMinX > _gyrosX)
          gyrosMinX = _gyrosX;
        if (gyrosMaxX < _gyrosX)
          gyrosMaxX = _gyrosX;
 
        if (gyrosMinY > _gyrosY)
          gyrosMinY = _gyrosY;
        if (gyrosMaxY < _gyrosY)
          gyrosMaxY = _gyrosY;
        
        if (gyrosMinZ > _gyrosZ)
          gyrosMinZ = _gyrosZ;
        if (gyrosMaxZ < _gyrosZ)
          gyrosMaxZ = _gyrosZ;
    }
}
float mpu521::acceloX()
{
    return aaWorld.x;
}
float mpu521::acceloY()
{
    return aaWorld.y;
}
float mpu521::acceloZ()
{
    return aaWorld.z;
}
float mpu521::gyrosX()
{
    return _gyrosX;
}
float mpu521::gyrosY()
{
    return _gyrosY;
}
float mpu521::gyrosZ()
{
    return _gyrosZ;
}
void mpu521::printValues()
{
    Serial.print("Gyro (deg)");
    Serial.print(" X = ");
    Serial.print(_gyrosX);
    Serial.print(" Y = ");
    Serial.print(_gyrosY);
    Serial.print(" Z = ");
    Serial.println(_gyrosZ);
   
    /*Serial.print(" Accel (g)");
    Serial.print(" X = ");
    Serial.print(aaWorld.x);
    Serial.print(" Y = ");
    Serial.print(aaWorld.y);
    Serial.print(" Z = ");
    Serial.println(aaWorld.z);*/
}
void mpu521::printBoundaries()
{
	  Serial.print("Gyro xmin = ");
    Serial.print(gyrosMinX);
    Serial.print(" xmax = ");
    Serial.print(gyrosMaxX);
    Serial.print("\tymin = ");
    Serial.print(gyrosMinY);
    Serial.print(" ymax = ");
    Serial.print(gyrosMaxY);
    Serial.print("\tzmin = ");
    Serial.print(gyrosMinZ);
    Serial.print(" zmax = ");
    Serial.println(gyrosMaxZ);

    /*Serial.print("Accel xmin = ");
    Serial.print(acceloMinX);
    Serial.print(" xmax = ");
    Serial.print(acceloMaxX);
    Serial.print("\tymin = ");
    Serial.print(acceloMinY);
    Serial.print(" ymax = ");
    Serial.print(acceloMaxY);
    Serial.print("\tzmin = ");
    Serial.print(acceloMinZ);
    Serial.print(" zmax = ");
    Serial.println(acceloMaxZ);*/
}
#endif
