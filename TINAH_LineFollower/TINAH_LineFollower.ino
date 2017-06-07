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
#define TURN_OFFSET_LOW_SLOW 60 // For turning at low speed
#define TURN_OFFSET_HIGH_SLOW 40
#define TURN_OFFSET_LOW_FAST 40 // For turning at high speed
#define TURN_OFFSET_HIGH_FAST 20

#define HIGH_THRESHOLD 75
#define LOW_THRESHOLD 300

#define LOW_LIGHT 0
#define MED_LIGHT 1
#define HIGH_LIGHT 2

#define REALIGNMENT_DELAY 200

// Global variables to keep track of current (most recent) L and R motor speeds
int currentRightSpeed = 0;
int currentLeftSpeed = 0;

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);
  
  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("Press START");
  while(! (startbutton())); // Do nothing
}

void loop() {
  // Get analog input from light sensors (digital inputs) 
  int analog_R = analogRead(TMP_PIN_R);
  int analog_L = analogRead(TMP_PIN_L);

  // Show analog outputs of sensors on the SERIAL.
  Serial.print("R - "); Serial.println(analog_R);
  Serial.print("L - "); Serial.println(analog_L);

  // Compare values to thresholds
  int tmp_R = compareThreshold(analog_R);
  int tmp_L = compareThreshold(analog_L);

  // Move a direction based off readings
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
    moveBrake();
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
  motor.speed(MOTOR_R, BRAKE_SPEED);
  motor.speed(MOTOR_L, BRAKE_SPEED);
  delay(REALIGNMENT_DELAY);

  int speedDiff = currentRightSpeed - currentLeftSpeed;

  // Right speed > Left speed condition (moving left direction)
  /*if (speedDiff > 0) {
    motor.speed(MOTOR_R, MAX_BWD_SPEED + TURN_OFFSET_LOW_SLOW);
    motor.speed(MOTOR_L, BRAKE_SPEED);

    // Track current speeds
    currentRightSpeed = MAX_BWD_SPEED + TURN_OFFSET_LOW_SLOW; 
    currentLeftSpeed = BRAKE_SPEED;

  // Left speed > Right speed condition (moving right direction)
  } else if (speedDiff < 0) {
    motor.speed(MOTOR_R, BRAKE_SPEED);
    motor.speed(MOTOR_L, MAX_BWD_SPEED + TURN_OFFSET_LOW_SLOW);

    // Track current speeds
    currentRightSpeed = BRAKE_SPEED; 
    currentLeftSpeed = MAX_BWD_SPEED + TURN_OFFSET_LOW_SLOW;
  
  // If both speeds are equal, reverses motors to try and backtrack to where tape was
  } else { */
    // Set speed to 0 to allow realignment process to begin
    motor.speed(MOTOR_R, BRAKE_SPEED);
    motor.speed(MOTOR_L, BRAKE_SPEED);
    delay(100); // Short delay for controlled realignment process

    // Short BWD reverse pulse
    motor.speed(MOTOR_R, MAX_BWD_SPEED);
    motor.speed(MOTOR_L, MAX_BWD_SPEED);
    delay(20); // Short delay for controlled realignment process
    
    currentRightSpeed = MAX_BWD_SPEED + BWD_OFFSET; 
    currentLeftSpeed = MAX_BWD_SPEED + BWD_OFFSET;
  //}

  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("REALIGNING");
}

void moveBrake() {
  motor.speed(MOTOR_R, BRAKE_SPEED);
  motor.speed(MOTOR_L, BRAKE_SPEED);

  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("BRAKE");
}

