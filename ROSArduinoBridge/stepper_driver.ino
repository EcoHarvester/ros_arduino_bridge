#include "stepper_driver.h"

StepperConfig steppers[MAX_STEPPERS] = {
  // {STEPPER1_STEP, STEPPER1_DIR, STEPPER1_ENABLE, LIMIT_SWITCH_1, 0, 0, false, CALIBRATING},
  // {STEPPER2_STEP, STEPPER2_DIR, STEPPER2_ENABLE, LIMIT_SWITCH_2, 0, 0, false, CALIBRATING}
  {STEPPER1_STEP, STEPPER1_DIR, LIMIT_SWITCH_1, 0, 0, false, CALIBRATING},
  {STEPPER2_STEP, STEPPER2_DIR, LIMIT_SWITCH_2, 0, 0, false, CALIBRATING}
};

volatile bool limit1Triggered = false;
volatile bool limit2Triggered = false;
unsigned long lastStepTime = 0;
const unsigned long stepDelay = 500; // microseconds between steps

void initSteppers() {
  // Initialize pins
  for (int i = 0; i < MAX_STEPPERS; i++) {
    pinMode(steppers[i].step_pin, OUTPUT);
    pinMode(steppers[i].dir_pin, OUTPUT);
    // pinMode(steppers[i].enable_pin, OUTPUT);
    // digitalWrite(steppers[i].enable_pin, HIGH); // Disable drivers initially
  }
  
  // Initialize limit switches
  pinMode(LIMIT_SWITCH_1, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_2, INPUT_PULLUP);
  
  // Attach interrupts for limit switches (external interrupt)
  attachInterrupt(digitalPinToInterrupt(LIMIT_SWITCH_1), limit1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(LIMIT_SWITCH_2), limit2ISR, FALLING);
  
  // Start calibration
  calibrateSteppers();
}

void limit1ISR() { limit1Triggered = true; }
void limit2ISR() { limit2Triggered = true; }

void calibrateSteppers() {
  for (int i = 0; i < MAX_STEPPERS; i++) {
    digitalWrite(steppers[i].enable_pin, LOW); // Enable driver
    digitalWrite(steppers[i].dir_pin, LOW); // Move towards limit switch
    steppers[i].state = CALIBRATING;
  }
}

void setStepperPosition(uint8_t stepper_num, float degrees) {
  if (stepper_num >= MAX_STEPPERS) return;
  
  long target_steps = degrees / DEG_PER_STEP;
  steppers[stepper_num].target_pos = target_steps;
  
  digitalWrite(steppers[stepper_num].dir_pin, 
              target_steps > steppers[stepper_num].current_pos ? HIGH : LOW);
  
  steppers[stepper_num].state = MOVING;
}

void updateSteppers() {
  static bool calibrating = true;
  
  // Handle calibration first
  if (calibrating) {
    if (limit1Triggered && limit2Triggered) {
      for (int i = 0; i < MAX_STEPPERS; i++) {
        digitalWrite(steppers[i].enable_pin, HIGH); // Disable driver
        steppers[i].current_pos = 0;
        steppers[i].homed = true;
        steppers[i].state = READY;
      }
      calibrating = false;
    }
    return;
  }

  // Handle normal movement
  unsigned long currentMicros = micros();
  
  if (currentMicros - lastStepTime >= stepDelay) {
    for (int i = 0; i < MAX_STEPPERS; i++) {
      if (steppers[i].state == MOVING) {
        if (steppers[i].current_pos != steppers[i].target_pos) {
          digitalWrite(steppers[i].step_pin, HIGH);
          delayMicroseconds(2);
          digitalWrite(steppers[i].step_pin, LOW);
          
          steppers[i].current_pos += (steppers[i].target_pos > steppers[i].current_pos) ? 1 : -1;
        } else {
          steppers[i].state = READY;
          digitalWrite(steppers[i].enable_pin, HIGH); // Disable after movement
        }
      }
    }
    lastStepTime = currentMicros;
  }
}

void processStepperCommand(int argc, char *argv[]) {
  if (argc != 3) return;
  
  float pos1 = atof(argv[1]);
  float pos2 = atof(argv[2]);
  
  digitalWrite(steppers[0].enable_pin, LOW);
  digitalWrite(steppers[1].enable_pin, LOW);
  
  setStepperPosition(0, pos1);
  setStepperPosition(1, pos2);
}