#include "stepper_driver.h"

StepperConfig steppers[MAX_STEPPERS] = {
  // {STEPPER1_STEP, STEPPER1_DIR, STEPPER1_ENABLE, LIMIT_SWITCH_1, 0, 0, false, CALIBRATING},
  // {STEPPER2_STEP, STEPPER2_DIR, STEPPER2_ENABLE, LIMIT_SWITCH_2, 0, 0, false, CALIBRATING}
  {STEPPER1_STEP, STEPPER1_DIR, LIMIT_SWITCH_1, 0, 0, false, CALIBRATING},
  {STEPPER2_STEP, STEPPER2_DIR, LIMIT_SWITCH_2, 0, 0, false, CALIBRATING}
};

volatile bool limit1Triggered = false;
volatile bool limit2Triggered = false;
unsigned long lastTriggerTime_Limit1 = 0;
unsigned long lastTriggerTime_Limit2 = 0;
static bool stepperCalibrated = false;  // Whether both steppers are calibrated decides when to exit calibration
unsigned long lastStepTime = 0;
const unsigned long stepDelay = 500; // microseconds between steps

#define DEBOUNCE_DELAY 500

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

void limit1ISR() { 
  limit1Triggered = true; 
}
void limit2ISR() { 
  limit2Triggered = true;
}

void calibrateSteppers() {
  // Stepper homing loop
  Serial.println("Entering stepper calibration");
  while (!(stepperCalibrated)) {

    if (limit1Triggered && !steppers[0].homed && millis() - lastTriggerTime_Limit1 >= DEBOUNCE_DELAY) {
      steppers[0].homed = true;
      lastTriggerTime_Limit1 = millis();
      limit1Triggered = false;
      Serial.println("Stepper 1 calibrated");
    }

    if (limit2Triggered && !steppers[1].homed && millis() - lastTriggerTime_Limit2 >= DEBOUNCE_DELAY) {
      steppers[1].homed = true;
      lastTriggerTime_Limit2 = millis();
      limit2Triggered = false;
      Serial.println("Stepper 2 calibrated");
    }


    // Check each stepper
    for (int i = 0; i < MAX_STEPPERS; i++) {
      // digitalWrite(steppers[i].enable_pin, LOW); // Enable driver
      if (!(steppers[i].homed)) {
        // when the steppers are not homed
        digitalWrite(steppers[i].dir_pin, LOW); // Move towards limit switch

        digitalWrite(steppers[i].step_pin, HIGH);
        delayMicroseconds(stepDelay);
        digitalWrite(steppers[i].step_pin, LOW);
        delayMicroseconds(stepDelay);
        steppers[i].state = CALIBRATING;
      }
      else {
        // when the steppers are homed
        steppers[i].state = READY;
      }
    }

    if (steppers[0].state == READY && steppers[1].state == READY) {
      stepperCalibrated = true;
    }
  }
  Serial.println("Stepper calibration finished");
}



// void setStepperPosition(uint8_t stepper_num, float degrees) {
//   if (stepper_num >= MAX_STEPPERS) return;
  
//   long target_steps = degrees / DEG_PER_STEP;
//   steppers[stepper_num].target_pos = target_steps;
  
//   digitalWrite(steppers[stepper_num].dir_pin, 
//               target_steps > steppers[stepper_num].current_pos ? HIGH : LOW);
  
//   steppers[stepper_num].state = MOVING;
// }

// void updateSteppers() {
//   static bool calibrating = true;
  
//   // Handle calibration first
//   if (calibrating) {
//     if (limit1Triggered && limit2Triggered) {
//       for (int i = 0; i < MAX_STEPPERS; i++) {
//         digitalWrite(steppers[i].enable_pin, HIGH); // Disable driver
//         steppers[i].current_pos = 0;
//         steppers[i].homed = true;
//         steppers[i].state = READY;
//       }
//       calibrating = false;
//     }
//     return;
//   }

//   // Handle normal movement
//   unsigned long currentMicros = micros();
  
//   if (currentMicros - lastStepTime >= stepDelay) {
//     for (int i = 0; i < MAX_STEPPERS; i++) {
//       if (steppers[i].state == MOVING) {
//         if (steppers[i].current_pos != steppers[i].target_pos) {
//           digitalWrite(steppers[i].step_pin, HIGH);
//           delayMicroseconds(2);
//           digitalWrite(steppers[i].step_pin, LOW);
          
//           steppers[i].current_pos += (steppers[i].target_pos > steppers[i].current_pos) ? 1 : -1;
//         } else {
//           steppers[i].state = READY;
//           digitalWrite(steppers[i].enable_pin, HIGH); // Disable after movement
//         }
//       }
//     }
//     lastStepTime = currentMicros;
//   }
// }

// void processStepperCommand(int argc, char *argv[]) {
//   if (argc != 3) return;
  
//   float pos1 = atof(argv[1]);
//   float pos2 = atof(argv[2]);
  
//   digitalWrite(steppers[0].enable_pin, LOW);
//   digitalWrite(steppers[1].enable_pin, LOW);
  
//   setStepperPosition(0, pos1);
//   setStepperPosition(1, pos2);
// }