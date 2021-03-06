#import "CurieBLE.h"
#import "CurieIMU.h"

// Definitions
BLEPeripheral blePeripheral;
BLEService motionService("AB126969-AB12-6969-AB12-AB126969AB12");
BLEFloatCharacteristic accelDataX("AB126969-AB12-6969-AB12-AB126969AB13", BLERead | BLENotify);
BLEFloatCharacteristic accelDataY("AB126969-AB12-6969-AB12-AB126969AB14", BLERead | BLENotify);
BLEFloatCharacteristic accelDataZ("AB126969-AB12-6969-AB12-AB126969AB15", BLERead | BLENotify);

float x;
float y;
float z;
int dx;
int dy;
int dz;
float dvx;
float dvy;
float dvz;
float vx;
float vy;
float vz;
float k;
float i;

long currentMillis;
long prevMillis;

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

  vx = 0;
  vy = 0;
  vz = 0;

}

void loop() {

    dvx = 0;
    dvy = 0;
    dvz = 0;
    i = 0;
    
    // look for central device to connect
    BLECentral central = blePeripheral.central();

    // if a central is connected then we start to do things
    if (central) {

      // Serial.println("i is set");
      
      while(currentMillis - prevMillis < 1){
       
        // read raw accel/gyro measurements from device
        CurieIMU.readAccelerometer(dx,dy,dz);

        dvx = dvx + float(dx);
        dvy = dvy + float(dy);
        dvz = dvz + float(dz-16384);
        i = i + 1;
        currentMillis = millis();}

       // if(abs(dvx)<2000000){dvx = 0;}
       // if(abs(dvy)<2000000){dvy = 0;}
       // if(abs(dvz)<2000000){dvz = 0;}

      if(i == 0){k = 0;}
      else{k=1;}
      
      // Serial.println("k is set");

      vx = vx + k*(dvx/i);
      vy = vy + k*(dvy/i);
      vz = vz + k*(dvz/i);

      x = k*(vx/(i*1000));
      y = k*(vx/(i*1000));
      z = k*(vx/(i*1000));
              
      accelDataX.setValue(x);
      accelDataY.setValue(y);
      accelDataZ.setValue(z);
     
      Serial.print("i=  ");
      Serial.print(i);
      Serial.print("\t vx =");
      Serial.print(vx);
      Serial.print("\t dvx =");
      Serial.println(dvx);
      // Serial.print("\t");
      //Serial.print(y);
      //Serial.print("\t");
      //Serial.println(k*dvz);

      prevMillis = millis();
    }
}

