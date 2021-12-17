#include <Arduino.h>
#include <Adafruit_SSD1306.h>

#define RIGHT_BACKWARDS PB_8
#define RIGHT_FORWARDS PB_9

#define LEFT_FORWARDS PB_6
#define LEFT_BACKWARDS PB_7

#define SERVO_DROP PA_8
#define SERVO_SLOPE PA_9

#define HOLSTER_ONE PA_6
#define HOLSTER_TWO PA_7

#define RAMP PA_0

#define POT_SPEED_LEFT PB0
#define POT_SPEED_RIGHT PB1

#define POT_THRESHOLD PA_1
#define POT_CONSTANT PA_2

#define LEFT_REFLECT_SENSOR PA_4
#define RIGHT_REFLECT_SENSOR PA_5

#define BOX_SWITCH PA11

#define RIGHT_ON 0
#define LEFT_ON 1

#define BOTH_OFF 0
#define NOT_BOTH_OFF 1

#define FIRST_HOLSTER_ANGLE 145
#define SECOND_HOLSTER_ANGLE 120
#define THIRD_HOLSTER_ANGLE 105

#define CLOSED_ANGLE 20
#define OPEN_ANGLE 108

#define MOTORFREQ 100
#define RAMPFREQ 50
#define SERVOFREQ 50


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET  -1 // This display does not have a reset pin accessible
TwoWire Wire1(PB11, PB10);// Use STM32 I2C2
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);


int slopeServoSpeed(float angle);
void handle_interrupt();

volatile int speedLeft = 1100;
//975
volatile int speedRight = 1095;
//970
volatile int leftSensor = 0;
volatile int rightSensor = 0;
volatile int constant = 605;
//420
volatile int threshold = 200;
volatile int lastOn = 5;
volatile int lastState = 5;
volatile int bothOffConstant = 2200;
//2060
volatile int bothOffConstantOuter = 1500;
//1350

volatile int rampSpeed = 3700;
// was 3900

volatile int holsterOne =0;
volatile int holsterTwo =0;
volatile bool haveOneCan = false;
volatile bool haveTwoCans = false;
volatile int detectCanThresh = 100;
volatile bool notActivated = true ;

volatile int servo_speed = 400;
volatile float servo_angle = 155.0;


void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(10);
 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Let's gooo boys!");
  display.display();
  display.clearDisplay();
  delay(10);

  pinMode(RIGHT_BACKWARDS, OUTPUT);
  pinMode(RIGHT_FORWARDS, OUTPUT);
  pinMode(LEFT_FORWARDS, OUTPUT);
  pinMode(LEFT_BACKWARDS, OUTPUT);
  pinMode(RAMP, OUTPUT);
  pinMode(SERVO_SLOPE,OUTPUT);
  pinMode(SERVO_DROP,OUTPUT);

  pinMode(POT_SPEED_LEFT, INPUT);
  pinMode(POT_SPEED_RIGHT, INPUT);
  pinMode(POT_THRESHOLD, INPUT);
  pinMode(POT_CONSTANT, INPUT);
  pinMode(LEFT_REFLECT_SENSOR, INPUT);
  pinMode(RIGHT_REFLECT_SENSOR, INPUT);
  pinMode(HOLSTER_ONE,INPUT);
  pinMode(HOLSTER_TWO,INPUT);
  pinMode(BOX_SWITCH, INPUT_PULLUP);

 attachInterrupt(digitalPinToInterrupt(BOX_SWITCH), handle_interrupt, RISING);


 pwm_start(RIGHT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
 pwm_start(RIGHT_FORWARDS,MOTORFREQ, 0,RESOLUTION_12B_COMPARE_FORMAT);
 pwm_start(LEFT_FORWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
 pwm_start(LEFT_BACKWARDS,MOTORFREQ,0,RESOLUTION_12B_COMPARE_FORMAT);
 pwm_start(RAMP,MOTORFREQ,rampSpeed,RESOLUTION_12B_COMPARE_FORMAT);
 pwm_start(SERVO_SLOPE, SERVOFREQ, slopeServoSpeed(FIRST_HOLSTER_ANGLE), MICROSEC_COMPARE_FORMAT);
 pwm_start(SERVO_DROP, SERVOFREQ, slopeServoSpeed(CLOSED_ANGLE), MICROSEC_COMPARE_FORMAT);
}


void loop() {
  if(!notActivated){
  pwm_start(SERVO_SLOPE, SERVOFREQ, slopeServoSpeed(FIRST_HOLSTER_ANGLE), MICROSEC_COMPARE_FORMAT);
  pwm_start(RAMP,MOTORFREQ,rampSpeed,RESOLUTION_12B_COMPARE_FORMAT);
  haveOneCan = false;
  haveTwoCans = false;
  while(digitalRead(BOX_SWITCH)==LOW){
  }
  delay(500);
   pwm_start(SERVO_DROP, SERVOFREQ, slopeServoSpeed(CLOSED_ANGLE), MICROSEC_COMPARE_FORMAT);
   notActivated = true;
  }


//speedLeft = analogRead(POT_SPEED_LEFT);
//speedLeft = analogRead(POT_SPEED_RIGHT);
//speedRight = analogRead(POT_SPEED_RIGHT);
// rampSpeed = 4* analogRead(POT_THRESHOLD);

// bothOffConstant = 4* analogRead(POT_SPEED_RIGHT);
  
 //constant = 4*analogRead(POT_THRESHOLD);
 //bothOffConstantOuter = 4*analogRead(POT_CONSTANT);
 //threshold = analogRead(POT_THRESHOLD);


  //display.clearDisplay();
  //display.setCursor(0,0);
  /**
  display.print("Left Speed: ");
  display.println( speedLeft );
  display.print("Right Speed: ");
  display.println( speedRight);

  display.print("Left Sensor: ");
  display.println(leftSensor);
  display.print("Right Sensor: ");
  display.println(rightSensor);
  display.print("Constant: ");
  display.println(constant);
  display.print("Threshold: ");
  display.println(threshold);
  display.print("Both Off K: ");
  display.println(bothOffConstant);
  display.print("Both Off K Outer: ");
  display.println(bothOffConstantOuter);
   **/

  
  /**
  display.print("Ramp Speed: ");
  display.println( rampSpeed );
  **/
  //display.display();

  //pwm_start(RAMP,RAMPFREQ, rampSpeed,RESOLUTION_12B_COMPARE_FORMAT); 

  leftSensor = analogRead(LEFT_REFLECT_SENSOR);
  rightSensor = analogRead(RIGHT_REFLECT_SENSOR);

  if(rightSensor > threshold && leftSensor  > threshold){
    lastState = NOT_BOTH_OFF;
    pwm_start(RIGHT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
    pwm_start(LEFT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
    pwm_start(LEFT_FORWARDS, MOTORFREQ, speedLeft, RESOLUTION_12B_COMPARE_FORMAT);
    pwm_start(RIGHT_FORWARDS, MOTORFREQ, speedRight, RESOLUTION_12B_COMPARE_FORMAT);
  }

  else if(rightSensor <= threshold && leftSensor > threshold){
    lastOn = LEFT_ON;
    lastState = NOT_BOTH_OFF;

    pwm_start(RIGHT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
    pwm_start(LEFT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
    pwm_start(LEFT_FORWARDS,MOTORFREQ, speedLeft - constant, RESOLUTION_12B_COMPARE_FORMAT);
    pwm_start(RIGHT_FORWARDS,MOTORFREQ, speedRight + constant, RESOLUTION_12B_COMPARE_FORMAT);
  }

  else if(rightSensor > threshold && leftSensor <= threshold){
    lastOn = RIGHT_ON;
    lastState = NOT_BOTH_OFF;

   pwm_start(RIGHT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
    pwm_start(LEFT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
    pwm_start(LEFT_FORWARDS, MOTORFREQ, speedLeft + constant, RESOLUTION_12B_COMPARE_FORMAT);
    pwm_start(RIGHT_FORWARDS, MOTORFREQ, speedRight - constant, RESOLUTION_12B_COMPARE_FORMAT);
  }

  else if(rightSensor <= threshold && leftSensor <= threshold){
    lastState = BOTH_OFF;
    if(lastOn == RIGHT_ON){
      pwm_start(RIGHT_FORWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
       pwm_start(LEFT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
       pwm_start(RIGHT_BACKWARDS, MOTORFREQ, bothOffConstant, RESOLUTION_12B_COMPARE_FORMAT);
      pwm_start(LEFT_FORWARDS, MOTORFREQ, bothOffConstantOuter, RESOLUTION_12B_COMPARE_FORMAT);
    }
    else if(lastOn == LEFT_ON){
      pwm_start(LEFT_FORWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
      pwm_start(RIGHT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
      pwm_start(LEFT_BACKWARDS, MOTORFREQ, bothOffConstant, RESOLUTION_12B_COMPARE_FORMAT);
      pwm_start(RIGHT_FORWARDS, MOTORFREQ, bothOffConstantOuter, RESOLUTION_12B_COMPARE_FORMAT);
    }
    else{
      pwm_start(RIGHT_FORWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
      pwm_start(LEFT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
      pwm_start(LEFT_FORWARDS, MOTORFREQ, speedLeft, RESOLUTION_12B_COMPARE_FORMAT);
      pwm_start(RIGHT_FORWARDS, MOTORFREQ, speedRight, RESOLUTION_12B_COMPARE_FORMAT);
    }
  }
  else{
      pwm_start(RIGHT_FORWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
      pwm_start(LEFT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
      pwm_start(LEFT_FORWARDS, MOTORFREQ, speedLeft, RESOLUTION_12B_COMPARE_FORMAT);
      pwm_start(RIGHT_FORWARDS, MOTORFREQ, speedRight, RESOLUTION_12B_COMPARE_FORMAT);
    }
if(lastState!=BOTH_OFF){
holsterOne = analogRead(HOLSTER_ONE);
holsterTwo = analogRead(HOLSTER_TWO);
if(holsterOne<=detectCanThresh && !haveOneCan){
     pwm_start(SERVO_SLOPE, SERVOFREQ, slopeServoSpeed(SECOND_HOLSTER_ANGLE), MICROSEC_COMPARE_FORMAT);
     haveOneCan = true;
 }
else if(holsterOne<=detectCanThresh && holsterTwo <= detectCanThresh && !haveTwoCans){
    pwm_start(SERVO_SLOPE, SERVOFREQ, slopeServoSpeed(THIRD_HOLSTER_ANGLE), MICROSEC_COMPARE_FORMAT);
    haveTwoCans = true;
}
}
}
int slopeServoSpeed(float angle) {
  return (int) angle * (2200.0/180.0) + 400.0;
}
void handle_interrupt(){
  // how to check that it is still pressed down
  //digital read pin and if low then do it, should it be a while that actively checks? 
  // if one can 
  delay(15);
  if((haveOneCan || haveTwoCans) && digitalRead(BOX_SWITCH)==LOW){
 pwm_start(RIGHT_BACKWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
 pwm_start(LEFT_BACKWARDS,MOTORFREQ,0,RESOLUTION_12B_COMPARE_FORMAT);
 pwm_start(RIGHT_FORWARDS,MOTORFREQ, 0,RESOLUTION_12B_COMPARE_FORMAT);
 pwm_start(LEFT_FORWARDS, MOTORFREQ, 0, RESOLUTION_12B_COMPARE_FORMAT);
 pwm_start(RAMP,MOTORFREQ,0,RESOLUTION_12B_COMPARE_FORMAT);
 delay(1000);
 pwm_start(SERVO_DROP, SERVOFREQ, slopeServoSpeed(OPEN_ANGLE), MICROSEC_COMPARE_FORMAT);
 pwm_start(RIGHT_BACKWARDS, MOTORFREQ, 3500, RESOLUTION_12B_COMPARE_FORMAT);
 pwm_start(LEFT_FORWARDS, MOTORFREQ, 2800, RESOLUTION_12B_COMPARE_FORMAT);
 delay(1000);
 notActivated = false;
  }

}