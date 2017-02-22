#import "CurieBLE.h"
#import "CurieIMU.h"

// Definitions
BLEPeripheral blePeripheral;
BLEService motionService("AB126969-AB12-6969-AB12-AB126969AB");
BLEIntCharacteristic accelDataX("XB126969-AB12-6969-AB12-AB126969AB", BLERead | BLENotify);
BLEIntCharacteristic accelDataY("YB126969-AB12-6969-AB12-AB126969AB", BLERead | BLENotify);
BLEIntCharacteristic accelDataZ("ZB126969-AB12-6969-AB12-AB126969AB", BLERead | BLENotify);

int x;
int y;
int z;

int calibrateOffsets = 1; // int to determine whether calibration takes place or not




void setup() {
  Serial.begin(9600); // initialize Serial communication
  while (!Serial);    // wait for the serial port to open

  // initialize device
  Serial.println("Initializing IMU device...");
  CurieIMU.begin();

  // verify connection
  Serial.println("Testing device connections...");
  if (CurieIMU.begin()) {
    Serial.println("CurieIMU connection successful");
  } else {
    Serial.println("CurieIMU connection failed");
  }

  // use the nncode below to calibrate accel/gyro offset values
  if (calibrateOffsets == 1) {
    Serial.println("Internal sensor offsets BEFORE calibration...");
    Serial.print(CurieIMU.getAccelerometerOffset(X_AXIS));
    Serial.print("\t"); // -76
    Serial.print(CurieIMU.getAccelerometerOffset(Y_AXIS));
    Serial.print("\t"); // -235
    Serial.println(CurieIMU.getAccelerometerOffset(Z_AXIS));

    Serial.println("About to calibrate. Make sure your board is stable and upright");
    delay(5000);

    Serial.print("Starting Acceleration calibration and enabling offset compensation...");
    CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
    CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
    CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
    Serial.println(" Done");

    Serial.println("Internal sensor offsets AFTER calibration...");
    Serial.print(CurieIMU.getAccelerometerOffset(X_AXIS));
    Serial.print("\t"); // -76
    Serial.print(CurieIMU.getAccelerometerOffset(Y_AXIS));
    Serial.print("\t"); // -2359
    Serial.println(CurieIMU.getAccelerometerOffset(Z_AXIS));
  }

   // Add service name and UUID for bluetooth
  blePeripheral.setLocalName("stylus");
  blePeripheral.setAdvertisedServiceUuid(motionService.uuid());
 
  //HID Service and Characteristics
  blePeripheral.addAttribute(motionService);
  Serial.println("Motion Service added");
  blePeripheral.addAttribute(accelDataX);
  Serial.println("Accelerometer X Characteristic added");
  blePeripheral.addAttribute(accelDataY);
  Serial.println("Accelerometer Y Characteristic added");
  blePeripheral.addAttribute(accelDataZ);
  Serial.println("Accelerometer Z Characteristic added");


  // Setting the initial value for our mouse input
  accelDataX.setValue(0);
  accelDataY.setValue(0);
  accelDataZ.setValue(0);
  
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
    // look for central device to connect
    BLECentral central = blePeripheral.central();

    // if a central is connected then we start to do things
    if (central) {
      // read raw accel/gyro measurements from device
      CurieIMU.readAccelerometer(x, y, z);
    
      Serial.print(x);
      Serial.print("\t");
      Serial.print(y);
      Serial.print("\t");
      Serial.println(z);
      
      accelDataX.setValue(x);
      accelDataY.setValue(y);
      accelDataZ.setValue(z);
      
    }
}

