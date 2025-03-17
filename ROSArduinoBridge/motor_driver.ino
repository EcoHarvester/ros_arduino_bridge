/***************************************************************
   Motor driver definitions
   
   Add a "#elif defined" block to this file to include support
   for a particular motor driver.  Then add the appropriate
   #define near the top of the main ROSArduinoBridge.ino file.
   
   *************************************************************/

#ifdef USE_BASE
   
#ifdef POLOLU_VNH5019
  /* Include the Pololu library */
  #include "DualVNH5019MotorShield.h"

  /* Create the motor driver object */
  DualVNH5019MotorShield drive;
  
  /* Wrap the motor driver initialization */
  void initMotorController() {
    drive.init();
  }

  /* Wrap the drive motor set speed function */
  void setMotorSpeed(int i, int spd) {
    if (i == REAR_LEFT) drive.setM1Speed(spd);
    else drive.setM2Speed(spd);
  }

  // A convenience function for setting both motor speeds
  void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    setMotorSpeed(REAR_LEFT, leftSpeed);
    setMotorSpeed(REAR_RIGHT, rightSpeed);
  }
#elif defined(POLOLU_MC33926)
  /* Include the Pololu library */
  #include "DualMC33926MotorShield.h"

  /* Create the motor driver object */
  DualMC33926MotorShield drive;
  
  /* Wrap the motor driver initialization */
  void initMotorController() {
    drive.init();
  }

  /* Wrap the drive motor set speed function */
  void setMotorSpeed(int i, int spd) {
    if (i == LEFT) drive.setM1Speed(spd);
    else drive.setM2Speed(spd);
  }

  // A convenience function for setting both motor speeds
  void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    setMotorSpeed(REAR_LEFT, leftSpeed);
    setMotorSpeed(REAR_RIGHT, rightSpeed);
  }
#elif defined(L298_MOTOR_DRIVER)
  #ifdef TWO_MOTORS
    void initMotorController() {
      digitalWrite(REAR_RIGHT_MOTOR_ENABLE, HIGH);
      digitalWrite(REAR_LEFT_MOTOR_ENABLE, HIGH);
    }
    
    void setMotorSpeed(int i, int spd) {
      unsigned char reverse = 0;
    
      if (spd < 0)
      {
        spd = -spd;
        reverse = 1;
      }
      if (spd > 255)
        spd = 255;
      
      if (i == REAR_LEFT) { 
        if      (reverse == 0) { analogWrite(REAR_LEFT_MOTOR_FORWARD, spd); analogWrite(REAR_LEFT_MOTOR_BACKWARD, 0); }
        else if (reverse == 1) { analogWrite(REAR_LEFT_MOTOR_BACKWARD, spd); analogWrite(REAR_LEFT_MOTOR_FORWARD, 0); }
      }
      else { // Rear right
        if      (reverse == 0) { analogWrite(REAR_RIGHT_MOTOR_FORWARD, spd); analogWrite(REAR_RIGHT_MOTOR_BACKWARD, 0); }
        else if (reverse == 1) { analogWrite(REAR_RIGHT_MOTOR_BACKWARD, spd); analogWrite(REAR_RIGHT_MOTOR_FORWARD, 0); }
      }
    }
    
    void setMotorSpeeds(int leftSpeed, int rightSpeed) {
      setMotorSpeed(REAR_LEFT, leftSpeed);
      setMotorSpeed(REAR_RIGHT, rightSpeed);
    }


  #elif defined FOUR_MOTORS
    void initMotorController() {
      digitalWrite(REAR_RIGHT_MOTOR_ENABLE, HIGH);
      digitalWrite(REAR_LEFT_MOTOR_ENABLE, HIGH);
      digitalWrite(FRONT_LEFT_MOTOR_ENABLE, HIGH);
      digitalWrite(FRONT_RIGHT_MOTOR_ENABLE, HIGH);
    }
    
    void setMotorSpeed(int i, int spd) {
      unsigned char reverse = 0;
    
      if (spd < 0)
      {
        spd = -spd;
        reverse = 1;
      }
      if (spd > 255)
        spd = 255;
      
      if (i == REAR_LEFT) { 
        if      (reverse == 0) { analogWrite(REAR_LEFT_MOTOR_FORWARD, spd); analogWrite(REAR_LEFT_MOTOR_BACKWARD, 0); }
        else if (reverse == 1) { analogWrite(REAR_LEFT_MOTOR_BACKWARD, spd); analogWrite(REAR_LEFT_MOTOR_FORWARD, 0); }
      }
      else if (i == REAR_RIGHT) {
        if      (reverse == 0) { analogWrite(REAR_RIGHT_MOTOR_FORWARD, spd); analogWrite(REAR_RIGHT_MOTOR_BACKWARD, 0); }
        else if (reverse == 1) { analogWrite(REAR_RIGHT_MOTOR_BACKWARD, spd); analogWrite(REAR_RIGHT_MOTOR_FORWARD, 0); }
      }
      else if (i == FRONT_RIGHT) {
        if      (reverse == 0) { analogWrite(FRONT_RIGHT_MOTOR_FORWARD, spd); analogWrite(FRONT_RIGHT_MOTOR_BACKWARD, 0); }
        else if (reverse == 1) { analogWrite(FRONT_RIGHT_MOTOR_BACKWARD, spd); analogWrite(FRONT_RIGHT_MOTOR_FORWARD, 0); }
      }
      else {
        if      (reverse == 0) { analogWrite(FRONT_LEFT_MOTOR_FORWARD, spd); analogWrite(FRONT_LEFT_MOTOR_BACKWARD, 0); }
        else if (reverse == 1) { analogWrite(FRONT_LEFT_MOTOR_BACKWARD, spd); analogWrite(FRONT_LEFT_MOTOR_FORWARD, 0); }
      }
    }

    void setMotorSpeeds(int frontLeftSpeed, int frontRightSpeed, int rearLeftSpeed, int rearRightSpeed) {
      setMotorSpeed(FRONT_LEFT, frontLeftSpeed);
      setMotorSpeed(FRONT_RIGHT, frontRightSpeed);
      setMotorSpeed(REAR_LEFT, rearLeftSpeed);
      setMotorSpeed(REAR_RIGHT, rearRightSpeed);
    }
  #endif
#else
  #error A motor driver must be selected!
#endif

#endif
