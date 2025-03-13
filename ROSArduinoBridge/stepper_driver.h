/* Stepper driver definitions */
#ifndef STEPPER_DRIVER_H
#define STEPPER_DRIVER_H

// Define stepper parameters
#define STEPS_PER_REV 400    // 400 steps/rev full stepping
#define DEG_PER_STEP (360.0/STEPS_PER_REV)
#define MAX_STEPPERS 2

// Limit switch pins
#define LIMIT_SWITCH_1 2    // digital pin 2
#define LIMIT_SWITCH_2 3    // digital pin 3

// Stepper control pins
#define STEPPER1_STEP A0
#define STEPPER1_DIR A1
// #define STEPPER1_ENABLE 4

#define STEPPER2_STEP A2
#define STEPPER2_DIR A3
// #define STEPPER2_ENABLE 7

// Stepper states
enum StepperState {
  CALIBRATING,
  READY,
  MOVING
};

// Stepper configuration structure
struct StepperConfig {
  uint8_t step_pin;
  uint8_t dir_pin;
  // uint8_t enable_pin;
  uint8_t limit_switch;
  long current_pos;
  long target_pos;
  bool homed;                 // Is the stepper homed?
  bool stepState;             // The current state of the step pin
  long lastStepTime;          // Keep track of the previous moment the step pin was toggled
  StepperState state;
};

// Function declarations
void initSteppers();
void calibrateSteppers();
void setStepperPosition(uint8_t stepper_num, float degrees);
void updateSteppers();
void processStepperCommand(int argc, char *argv[]);

extern StepperConfig steppers[MAX_STEPPERS];
extern unsigned long lastStepTime;
extern volatile bool limit1Triggered;
extern volatile bool limit2Triggered;

#endif