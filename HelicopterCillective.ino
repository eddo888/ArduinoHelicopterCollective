
// Program used to test the USB Joystick library when used as 
// a Flight Controller on the Arduino Leonardo or Arduino 
// Micro.
//
// Matthew Heironimus
// 2016-05-29 - Original Version
//------------------------------------------------------------

/*
 * board profile = Leonardy Aeroplane
 * usb id = HIDFF1
 */
#include "Joystick.h"

#define NUM_BUTTONS 3
#define NUM_HATS 0

Joystick_ Joystick(
  JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK,
  NUM_BUTTONS, // buttons
  NUM_HATS,    // hat switches
  true,  // Xaxis
  true,  // Yaxis
  true,  // Zaxis
  true,  // rXaxis
  true,  // rYaxis
  true,  // rZaxis
  false, // Rudder
  true,  // Throttle
  false, // Accellerator
  false, // Brake
  false  // Steering
);

static const uint8_t D2  = 2; // brown stripe, button stick
static const uint8_t D3  = 3; // blue stripe,  switch left
static const uint8_t D4  = 4; // orange stripe, switch right
static const uint8_t D5  = 5; // 
static const uint8_t D6  = 6; // 
static const uint8_t D7  = 7; // 
static const uint8_t D8  = 8; // analog - collective


// Mapping of analog pins as digital I/O
/*
static const uint8_t A0 = 18; // Rudder
static const uint8_t A1 = 19; // toe right
static const uint8_t A2 = 20; // toe left
static const uint8_t A3 = 21; // x axiz camera
static const uint8_t A4 = 22; // y axis camera
static const uint8_t A5 = 23; // z throttle
*/

// A6-A11 share with digital pins
/*
static const uint8_t A6 = 24;  // D4
static const uint8_t A7 = 25;  // D6
static const uint8_t A8 = 26;  // D8 - collective
static const uint8_t A9 = 27;  // D9
static const uint8_t A10 = 28; // D10
static const uint8_t A11 = 29; // D12
*/

int rxAxisC; // center
int ryAxisC; // center

void setup() {
  
  Serial.begin(57600);
  
  Joystick.setXAxisRange(-127, 127);
  Joystick.setYAxisRange(-127, 127);
  Joystick.setZAxisRange(-127, 127);

  Joystick.setRxAxisRange(-127, 127);
  Joystick.setRyAxisRange(-127, 127);
  Joystick.setRzAxisRange(-127, 127);
  
  //Joystick.setRudderRange(-127, 127);
  Joystick.setThrottleRange(0, 255);

  //Joystick.setAcceleratorRange(0, 255);
  //Joystick.setBrakeRange(0, 255);
  //Joystick.setSteeringRange(-127, 127);
  
  Joystick.begin(false);

  pinMode(D2,  INPUT); // button
  pinMode(D3,  INPUT); // switch
  pinMode(D4,  INPUT); // switch
  pinMode(D9,  INPUT); // switch
  

  Serial.println("\n");
  Serial.println("and so it begins ...");

  rxAxisC = analogRead(A3);
  ryAxisC = analogRead(A4);

}

int xAxis = -1;
int yAxis = -1;
int zAxis = -1;
int rxAxis = -1;
int ryAxis = -1;
int rzAxis = -1;
//int rudder = 0;
int throttle = -1;

int button1 = -1; // reset trim
int switchLeft = -1; // screenshot
int switchRight = -1; // lock collective
int coveredSwitch = -1; // canopy

// clears after 250 milliseconds
int digits = -1;
unsigned long digitsMillis = 0;
unsigned long coveredMillis = 0;

int logging = 0;

void loop() {

  delay(1); // delay  between reads for stability
  
  unsigned long currentTime = millis();
    
  char mystr[16];
  
  // switch left
  uint8_t switchLeftI = digitalRead(D3);
  if (switchLeftI != switchLeft) {
    switchLeft = switchLeftI;
    sprintf(mystr, "Switch Left = %d", switchLeftI);
    Serial.println(mystr);
    Joystick.setButton(NUM_BUTTONS-3, 1^switchLeftI);
  }
  
  // switch right - force trim to middle 
  uint8_t switchRightI = digitalRead(D4);
  if (switchRightI != switchRight) {
    switchRight = switchRightI;
    sprintf(mystr, "Switch Right = %d", switchRightI);
    Serial.println(mystr);    
    if (switchRightI == 0) {
      rxAxis = rxAxisC;
      Joystick.setRxAxis(0);
      ryAxis = ryAxisC;
      Joystick.setRyAxis(0);
    }
  }

  uint8_t coveredRead  = digitalRead(D9);
  if (coveredRead != coveredSwitch) {
    coveredSwitch = coveredRead;
    if (coveredRead == 1) {
      Joystick.pressButton(NUM_BUTTONS-2);
    }
    else {
      Joystick.pressButton(NUM_BUTTONS-1);
    }
    coveredMillis = currentTime;
  }

  if (coveredMillis > 0 && currentTime - coveredMillis > 250) {
    Joystick.releaseButton(NUM_BUTTONS-2);
    Joystick.releaseButton(NUM_BUTTONS-1);
    coveredMillis = 0;
  }

  
  int xAxisI = analogRead(A0);
  float xAxisF = (xAxisI*256/1024)-127;
  if (xAxisI != xAxis) {
    if (logging) {
      sprintf(mystr, "Axis X=%d", xAxisI);
      Serial.println(mystr);
    }
    xAxis = xAxisI;
  }
  Joystick.setXAxis(xAxisF);

  int yAxisI = analogRead(A1 );
  float yAxisF = (yAxisI*256/1024)-127;
  if (yAxisI != yAxis) {
    if (logging) {
      sprintf(mystr, "Axis Y=%d",yAxisI);
      Serial.println(mystr);
    }
    yAxis = yAxisI;
  }
  Joystick.setYAxis(yAxisF);

  int zAxisI = analogRead(A2);
  float zAxisF = (zAxisI*256/1024)-127;
  if (zAxisI != zAxis) {
    if (logging) {
      sprintf(mystr, "Axis Z=%d",zAxisI);
      Serial.println(mystr);
    }
    zAxis = zAxisI;
  }
  Joystick.setZAxis(zAxisF);

  // trim hat x
  int rxAxisI = analogRead(A3);
  int rxAxisD = rxAxisI - rxAxisC; // offset to middle

  if (rxAxis == -1) rxAxis = rxAxisI;
  
  if (abs(rxAxisD) > 5) { 
    sprintf(mystr, "Axis rXd=%d", rxAxisD);
    Serial.println(mystr);

    rxAxis = rxAxis + rxAxisD/100;
    if (rxAxis < 0) rxAxis = 0;
    if (rxAxis > 1024) rxAxis = 1024;

    float rxAxisF = (rxAxis*256/1024)-127;
    Joystick.setRxAxis(rxAxisF);
  }

  // trim hat y
  int ryAxisI = analogRead(A4);
  int ryAxisD = ryAxisI - ryAxisC; // offset to middle

  if (ryAxis == -1) ryAxis = ryAxisI;
  
  if (abs(ryAxisD) > 5) {
    sprintf(mystr, "Axis rYd=%d", ryAxisD);
    Serial.println(mystr);

  // reverse axis
    ryAxis = ryAxis - ryAxisD/100;
    if (ryAxis < 0) ryAxis = 0;
    if (ryAxis > 1024) ryAxis = 1024;
    
    float ryAxisF = (ryAxis*256/1024)-127;
    Joystick.setRyAxis(ryAxisF);
  }

  int rzAxisI = analogRead(A5);
  float rzAxisF = (rzAxisI*256/1024)-127;
  if (logging && rzAxisI != rzAxis) {
    sprintf(mystr, "Axis rZ=%d", rzAxisI);
    Serial.println(mystr);
    zAxis = rzAxisI;
  }
  Joystick.setRzAxis(rzAxisF);

  // right hand switch, collective lock
  uint8_t button1I = digitalRead(D2);
  int throttleI = analogRead(D8);
  float throttleF = (throttleI*256/1024);

  if (button1I != button1) {
    //Joystick.setButton(button1J, 1^button1I);
    button1 = button1I;
    if (button1 == 0) {
      // lock throttle
      Joystick.setThrottle(throttleF);
      sprintf(mystr, "Axis throttle=%d",throttleI);
      Serial.println(mystr);    
    }
    else {
      // slowly return throttle to new
      
    }
    sprintf(mystr, "Button 1 = %d", button1I);
    Serial.println(mystr);    
  }

  if (logging && throttleI != throttle) {
    sprintf(mystr, "Axis throttle=%d",throttleI);
    Serial.println(mystr);
    throttle = throttleI;
  }
  if (button1 == 1) {
    // unlocked throttle
    Joystick.setThrottle(throttleF);
  }

  Joystick.sendState();

}
