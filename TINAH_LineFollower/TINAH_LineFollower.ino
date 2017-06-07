// TINAH_LineFollower - Line follower using the TINAH board

#include <phys253.h>
#include <LiquidCrystal.h>

#define TMP_PIN_R 0
#define TMP_PIN_L 5

#define MOTOR_R 1
#define MOTOR_L 0

#define MAX_FWD_SPEED 255
#define MAX_BWD_SPEED -255
#define BRAKE_SPEED 0

#define FWD_OFFSET 20
#define BWD_OFFSET 20
#define TURN_OFFSET_LOW_SLOW 60
#define TURN_OFFSET_HIGH_SLOW 40
#define TURN_OFFSET_LOW_FAST 40
#define TURN_OFFSET_HIGH_FAST 20

#define HIGH_THRESHOLD 300
#define LOW_THRESHOLD 50

#define LOW_LIGHT 0
#define MED_LIGHT 1
#define HIGH_LIGHT 2

#define BWD_RA_DELAY 100
#define TURN_RA_DELAY 100

int currentRightSpeed = 0;
int currentLeftSpeed = 0;

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
  
  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("Press START");
  while(! (startbutton()));
}

void loop() {
  int analog_R = analogRead(TMP_PIN_R);
  int analog_L = analogRead(TMP_PIN_L);

  Serial.print("TMP_R = "); Serial.println(analog_R);
  Serial.print("TMP_L = "); Serial.println(analog_L);

  int tmp_R = compareThreshold(analog_R);
  int tmp_L = compareThreshold(analog_L);

  moveDirection(tmp_R, tmp_L);
}

int compareThreshold(int tmp_value) {
  if (tmp_value < LOW_THRESHOLD)
    return HIGH_LIGHT;
  else if (tmp_value < HIGH_THRESHOLD)
    return MED_LIGHT;
  else 
    return LOW_LIGHT;
}

void moveDirection(int tmp_R, int tmp_L) {
  if (tmp_R == LOW_LIGHT && tmp_L == LOW_LIGHT)
    moveForward();
  else if (tmp_R == LOW_LIGHT && tmp_L == MED_LIGHT)
    moveRightSlow();
  else if (tmp_R == LOW_LIGHT && tmp_L == HIGH_LIGHT)
    moveRightFast();
  else if (tmp_R == MED_LIGHT && tmp_L == LOW_LIGHT)
    moveLeftSlow();
  else if (tmp_R == HIGH_LIGHT && tmp_L == LOW_LIGHT)
    moveLeftFast();
  else if (tmp_R == MED_LIGHT && tmp_L == MED_LIGHT)
    moveBacktrack();
  else if (tmp_R == HIGH_LIGHT && tmp_L == HIGH_LIGHT) 
    moveBacktrack();
  else
    moveBrake(); // ERROR CASE
}

void moveForward() {
  currentRightSpeed = MAX_FWD_SPEED - FWD_OFFSET;
  currentLeftSpeed  = MAX_FWD_SPEED - FWD_OFFSET;
  
  motor.speed(MOTOR_R, currentRightSpeed);
  motor.speed(MOTOR_L, currentLeftSpeed);

  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("FORWARD");
}

void moveLeftSlow() {
  currentRightSpeed = MAX_BWD_SPEED + TURN_OFFSET_LOW_SLOW;
  currentLeftSpeed  = MAX_FWD_SPEED - TURN_OFFSET_HIGH_SLOW;
  
  motor.speed(MOTOR_R, currentRightSpeed);
  motor.speed(MOTOR_L, currentLeftSpeed);

  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("RIGHT SLOW");
}

void moveRightSlow() {
  currentLeftSpeed  = MAX_BWD_SPEED + TURN_OFFSET_LOW_SLOW;
  currentRightSpeed = MAX_FWD_SPEED - TURN_OFFSET_HIGH_SLOW;
  
  motor.speed(MOTOR_L, currentLeftSpeed);
  motor.speed(MOTOR_R, currentRightSpeed);

  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("LEFT SLOW");
}

void moveLeftFast() {
  currentRightSpeed = MAX_BWD_SPEED + TURN_OFFSET_LOW_FAST;
  currentLeftSpeed  = MAX_FWD_SPEED - TURN_OFFSET_HIGH_FAST;
  
  motor.speed(MOTOR_R, currentRightSpeed);
  motor.speed(MOTOR_L, currentLeftSpeed);

  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("RIGHT FAST");
}

void moveRightFast() {
  currentLeftSpeed  = MAX_BWD_SPEED + TURN_OFFSET_LOW_FAST;
  currentRightSpeed = MAX_FWD_SPEED - TURN_OFFSET_HIGH_FAST;
  
  motor.speed(MOTOR_L, currentLeftSpeed);
  motor.speed(MOTOR_R, currentRightSpeed);

  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("LEFT FAST");
}

void moveBacktrack() {
  int speedDiff = currentRightSpeed - currentLeftSpeed;
  
  if (speedDiff > 0) // R > L
    moveLeftFast();
    delay(TURN_RA_DELAY);
  else if (speedDiff < 0) { // L > R
    moveRightFast();
    delay(TURN_RA_DELAY);
  else {
    currentRightSpeed = MAX_BWD_SPEED + BWD_OFFSET; 
    currentLeftSpeed  = MAX_BWD_SPEED + BWD_OFFSET;
    
    motor.speed(MOTOR_R, currentRightSpeed);
    motor.speed(MOTOR_L, currentLeftSpeed);
    delay(BWD_RA_DELAY);
  }

  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("REALIGN");
}

void moveBrake() {
  motor.speed(MOTOR_R, BRAKE_SPEED);
  motor.speed(MOTOR_L, BRAKE_SPEED);

  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("ERROR - BRAKE");
}

