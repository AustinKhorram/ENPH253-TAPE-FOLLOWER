#include <phys253.h>
#include <LiquidCrystal.h>

#define MAX_FWD_SPEED 255
#define MAX_BWD_SPEED -255
#define BRAKE_SPEED 0

#define TMP_R_PIN 0
#define TMP_L_PIN 5

#define MOTOR_R 1
#define MOTOR_L 0

#define SPEED_OFFSET 70 // needs calibration

#define THRESHOLD_R 400 // needs calibration
#define THRESHOLD_L 400 // needs calibration

// haha fk u keeno.

// Global variables to keep track of current (most recent) L and R motor speeds
int currentLeftSpeed = 0;
int currentRightSpeed = 0;

void setup() {
  #include <phys253setup.txt>
  Serial.begin(9600);

  LCD.clear(); LCD.home();
  LCD.setCursor(0,0); LCD.print("Press START");
  while( !(startbutton()) );
}

void loop() {   
  // Get analog input from light sensors (digital inputs) 
  // (LOW is light detected, HIGH is no light detected)
  double analog_R = analogRead(TMP_R_PIN);
  double analog_L = analogRead(TMP_L_PIN);
  
  // Show analog outputs of sensors on the SERIAL.
  Serial.print("R - "); Serial.println(analog_R);
  Serial.print("L - "); Serial.println(analog_L);
  
  // Determines whether the analog outputs of the light sensors are considered HIGH or LOW
  int tmp_R = compareThreshold(analog_R, THRESHOLD_R);
  int tmp_L = compareThreshold(analog_L, THRESHOLD_L);

  // Commands the bot to move based on the HIGH or LOW values of each motor variable
  moveDirection(tmp_R, tmp_L);
}

/* 
 * @param int tmp_R - HIGH or LOW (depends on light detected)
 * @param int tmp_L - HIGH or LOW (depends on light detected)
 * @returns void
 */
void moveDirection(int tmp_R, int tmp_L) {
  
  // Straight FWD move condition
  if(tmp_R == LOW && tmp_L == LOW) {
    motor.speed(MOTOR_R, MAX_FWD_SPEED - SPEED_OFFSET); // Set R and L motor speeds equal
    motor.speed(MOTOR_L, MAX_FWD_SPEED - SPEED_OFFSET);

    currentRightSpeed = (MAX_FWD_SPEED - SPEED_OFFSET); // Adjust the current speeds
    currentLeftSpeed = (MAX_FWD_SPEED - SPEED_OFFSET);
     
    LCD.clear(); LCD.home();
    LCD.setCursor(0,0); LCD.print("FORWARD"); // TINAH display current action
  } 

  // Right move condition (too far left)
  else if(tmp_R == LOW && tmp_L == HIGH) {
    
    motor.speed(MOTOR_R, BRAKE_SPEED);
    motor.speed(MOTOR_L, BRAKE_SPEED);
    delay(20); // Short delay for controlled realignment process

    // Set left motor FASTER than right motor to realign with tape (move right)
    motor.speed(MOTOR_L, MAX_BWD_SPEED + SPEED_OFFSET); // DIFFERENCES MAY NEED CALIBRATION
    motor.speed(MOTOR_R, MAX_FWD_SPEED - SPEED_OFFSET);
    delay(100); // Short delay for controlled realignment process

    LCD.clear(); LCD.home();
    LCD.setCursor(0,0); LCD.print("RIGHT"); // TINAH display current action
  }

  // Left move condition (too far right)
  else if(tmp_R == HIGH && tmp_L == LOW) { 

    motor.speed(MOTOR_R, BRAKE_SPEED);
    motor.speed(MOTOR_L, BRAKE_SPEED);
    delay(20); // Short delay for controlled realignment process

    // Set right motor FASTER than left motor to realign with tape (move left)
    motor.speed(MOTOR_R, MAX_BWD_SPEED + SPEED_OFFSET); // DIFFERENCES MAY NEED CALIBRATION
    motor.speed(MOTOR_L, MAX_FWD_SPEED - SPEED_OFFSET);
    delay(100); // Short delay for controlled realignment process

    LCD.clear(); LCD.home();
    LCD.setCursor(0,0); LCD.print("LEFT"); // TINAH display current action
  } 

  // Realignment move condition (no tape detected)
  else {

    // Set speed to 0 to allow realignment process to begin
    motor.speed(MOTOR_R, BRAKE_SPEED);
    motor.speed(MOTOR_L, BRAKE_SPEED);
    delay(100); // Short delay for controlled realignment process

    // Short BWD reverse pulse
    motor.speed(MOTOR_R, MAX_BWD_SPEED);
    motor.speed(MOTOR_L, MAX_BWD_SPEED);
    delay(20); // Short delay for controlled realignment process

    LCD.clear(); LCD.home();
    LCD.setCursor(0,0); LCD.print("REALIGNING"); // TINAH display current action
  }
}

/* 
 * @param int analog - the analog voltage output of a light sensor 
 * @param int threshold - the constant voltage threshold to determine whether tape is detected or not
 * @returns int HIGH or LOW
 */
int compareThreshold(int analog, int threshold) {
  if (analog < threshold) {
    return HIGH;
  } else {
    return LOW;
  }
}

