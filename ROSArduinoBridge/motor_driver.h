/***************************************************************
   Motor driver function definitions - by James Nugen
   *************************************************************/

#ifdef L298_MOTOR_DRIVER
  #ifdef TWO_MOTORS
    #define REAR_RIGHT_MOTOR_BACKWARD 5
    #define REAR_LEFT_MOTOR_BACKWARD  6
    #define REAR_RIGHT_MOTOR_FORWARD  9
    #define REAR_LEFT_MOTOR_FORWARD   10
    #define REAR_RIGHT_MOTOR_ENABLE 12
    #define REAR_LEFT_MOTOR_ENABLE 13
  # elif defined FOUR_MOTORS
    #define REAR_RIGHT_MOTOR_BACKWARD 6
    #define REAR_RIGHT_MOTOR_FORWARD  7
    #define REAR_LEFT_MOTOR_BACKWARD  8
    #define REAR_LEFT_MOTOR_FORWARD   9
    #define REAR_RIGHT_MOTOR_ENABLE 25
    #define REAR_LEFT_MOTOR_ENABLE 23

    #define FRONT_RIGHT_MOTOR_BACKWARD 10
    #define FRONT_RIGHT_MOTOR_FORWARD  11
    #define FRONT_LEFT_MOTOR_BACKWARD  12
    #define FRONT_LEFT_MOTOR_FORWARD   13
    #define FRONT_RIGHT_MOTOR_ENABLE 22
    #define FRONT_LEFT_MOTOR_ENABLE 24
  #endif
#endif

void initMotorController();
void setMotorSpeed(int i, int spd);
void setMotorSpeeds(int leftSpeed, int rightSpeed);
