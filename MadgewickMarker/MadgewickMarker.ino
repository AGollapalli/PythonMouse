#include <BLECharacteristic.h>
#include <CurieBLE.h>
#include <BLEAttribute.h>
#include <BLECommon.h>
#include <BLEPeripheral.h>
#include <BLEUuid.h>
#include <BLEService.h>
#include <BLETypedCharacteristics.h>
#include <BLEDescriptor.h>
#include <BLETypedCharacteristic.h>
#include <BLECentral.h>

#include <CurieIMU.h>
#include <MadgwickAHRS.h>

// Definitions
BLEPeripheral blePeripheral;
BLEService motionService("AB126969-AB12-6969-AB12-AB126969AB12");
BLEFloatCharacteristic accelDataRoll("AB126969-AB12-6969-AB12-AB126969AB13", BLERead | BLENotify);
BLEFloatCharacteristic accelDataPitch("AB126969-AB12-6969-AB12-AB126969AB14", BLERead | BLENotify);
BLEFloatCharacteristic accelDataYaw("AB126969-AB12-6969-AB12-AB126969AB15", BLERead | BLENotify);

Madgwick filter;
unsigned long microsPerReading, microsPrevious;
float accelScale, gyroScale;

void setup() {
  Serial.begin(9600);

  // start the IMU and filter
  CurieIMU.begin();
  CurieIMU.setGyroRate(25);
  CurieIMU.setAccelerometerRate(25);
  filter.begin(25);

  // Set the accelerometer range to 2G
  CurieIMU.setAccelerometerRange(2);
  // Set the gyroscope range to 250 degrees/second
  CurieIMU.setGyroRange(250);

  // initialize variables to pace updates to correct rate
  microsPerReading = 1000000 / 25;
  microsPrevious = micros();

    // Add service name and UUID for bluetooth
  blePeripheral.setLocalName("stylus");
  blePeripheral.setAdvertisedServiceUuid(motionService.uuid());
 
  //HID Service and Characteristics
  blePeripheral.addAttribute(motionService);
  Serial.println("Motion Service added");
  blePeripheral.addAttribute(accelDataRoll);
  Serial.println("Accelerometer X Characteristic added");
  blePeripheral.addAttribute(accelDataPitch);
  Serial.println("Accelerometer Y Characteristic added");
  blePeripheral.addAttribute(accelDataYaw);
  Serial.println("Accelerometer Z Characteristic added");


  // Setting the initial value for our mouse input
  accelDataRoll.setValue(0);
  accelDataPitch.setValue(0);
  accelDataYaw.setValue(0);
  
  // Starting the service
  blePeripheral.begin();
  Serial.println("Bluetooth device active, waiting for connections...");
  
  // look for central device to connect
  BLECentral central = blePeripheral.central();

  Serial.print("Connecting to central:   ");
  // print the central's MAC address
  Serial.println(central.address());
  
}

void loop() {
  int aix, aiy, aiz;
  int gix, giy, giz;
  float ax, ay, az;
  float gx, gy, gz;
  float roll, pitch, heading;
  unsigned long microsNow;

  // check if it's time to read data and update the filter
  microsNow = micros();
  if (microsNow - microsPrevious >= microsPerReading) {

    // read raw data from CurieIMU
    CurieIMU.readMotionSensor(aix, aiy, aiz, gix, giy, giz);

    // convert from raw data to gravity and degrees/second units
    ax = convertRawAcceleration(aix);
    ay = convertRawAcceleration(aiy);
    az = convertRawAcceleration(aiz);
    gx = convertRawGyro(gix);
    gy = convertRawGyro(giy);
    gz = convertRawGyro(giz);

    // update the filter, which computes orientation
    filter.updateIMU(gx, gy, gz, ax, ay, az);

    // print the heading, pitch and roll
    roll = filter.getRoll();
    pitch = filter.getPitch();
    heading = filter.getYaw();
    Serial.print("Orientation: ");
    Serial.print(heading);
    Serial.print(" ");
    Serial.print(pitch);
    Serial.print(" ");
    Serial.println(roll);

      accelDataRoll.setValue(roll);
      accelDataPitch.setValue(pitch);
      accelDataYaw.setValue(heading);
     

    // increment previous time, so we keep proper pace
    microsPrevious = microsPrevious + microsPerReading;
  }
}

float convertRawAcceleration(int aRaw) {
  // since we are using 2G range
  // -2g maps to a raw value of -32768
  // +2g maps to a raw value of 32767
 
  float a = (aRaw * 2.0) / 32768.0;
  return a;
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767
 
  float g = (gRaw * 250.0) / 32768.0;
  return g;
}
