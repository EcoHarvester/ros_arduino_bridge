#include "stepper_driver.h"

StepperConfig steppers[MAX_STEPPERS] = {
    // {STEPPER1_STEP, STEPPER1_DIR, STEPPER1_ENABLE, LIMIT_SWITCH_1, 0, 0, false, CALIBRATING},
    // {STEPPER2_STEP, STEPPER2_DIR, STEPPER2_ENABLE, LIMIT_SWITCH_2, 0, 0, false, CALIBRATING}
    { STEPPER1_STEP, STEPPER1_DIR, LIMIT_SWITCH_1, 0, 0, false, LOW, 0, CALIBRATING },
    { STEPPER2_STEP, STEPPER2_DIR, LIMIT_SWITCH_2, 0, 0, false, LOW, 0, CALIBRATING }
};

volatile bool limit1Triggered = false;
volatile bool limit2Triggered = false;
unsigned long lastTriggerTime_Limit1 = 0;
unsigned long lastTriggerTime_Limit2 = 0;
const unsigned long stepDelay = 1500; // microseconds between steps

#define DEBOUNCE_DELAY 500

void
initSteppers ()
{
    // Initialize pins
    for (int i = 0; i < MAX_STEPPERS; i++)
    {
        pinMode (steppers[i].step_pin, OUTPUT);
        pinMode (steppers[i].dir_pin, OUTPUT);
        // pinMode(steppers[i].enable_pin, OUTPUT);
        // digitalWrite(steppers[i].enable_pin, HIGH); // Disable drivers initially
    }

    // Initialize limit switches
    pinMode (LIMIT_SWITCH_1, INPUT_PULLUP);
    pinMode (LIMIT_SWITCH_2, INPUT_PULLUP);

    // Attach interrupts for limit switches (external interrupt)
    attachInterrupt (digitalPinToInterrupt (LIMIT_SWITCH_1), limit1ISR, FALLING);
    attachInterrupt (digitalPinToInterrupt (LIMIT_SWITCH_2), limit2ISR, FALLING);

    // Start calibration
    calibrateSteppers ();
}

void
limit1ISR ()
{
    limit1Triggered = true;
}

void
limit2ISR ()
{
    limit2Triggered = true;
}

void
calibrateSteppers ()
{
    // Stepper homing loop
    Serial.println ("Calibrating the steppers");
    while (!(steppers[0].homed && steppers[1].homed))
    {
        // Serial.println("Stepper 1");
        // Serial.println(limit1Triggered);
        // Serial.println(steppers[0].homed);
        // Serial.println(millis() - lastTriggerTime_Limit1);

        if (limit1Triggered && !steppers[0].homed
            && millis () - lastTriggerTime_Limit1 >= DEBOUNCE_DELAY)
        {
            steppers[0].homed = true;
            steppers[0].current_pos = 0;
            lastTriggerTime_Limit1 = millis ();
            limit1Triggered = false;
            Serial.println ("Stepper 1 calibrated");
        }

        // Serial.println("Stepper 2");
        // Serial.println(limit2Triggered);
        // Serial.println(steppers[1].homed);
        // Serial.println(millis() - lastTriggerTime_Limit2);
        if (limit2Triggered && !steppers[1].homed
            && millis () - lastTriggerTime_Limit2 >= DEBOUNCE_DELAY)
        {
            steppers[1].homed = true;
            steppers[1].current_pos = 0;
            lastTriggerTime_Limit2 = millis ();
            limit2Triggered = false;
            Serial.println ("Stepper 2 calibrated");
        }

        // Check each stepper, if not homed then move toward home, else READY
        for (int i = 0; i < MAX_STEPPERS; i++)
        {
            // digitalWrite(steppers[i].enable_pin, LOW); // Enable driver
            if (!(steppers[i].homed))
            {
                // when the steppers are not homed
                digitalWrite (steppers[i].dir_pin, LOW); // Move towards limit switch

                if (micros () - steppers[i].lastStepTime > stepDelay)
                {
                    steppers[i].lastStepTime = micros ();
                    steppers[i].stepState = !steppers[i].stepState;
                    digitalWrite (steppers[i].step_pin, steppers[i].stepState);
                }

                steppers[i].state = CALIBRATING;
            }
            else
            {
                // when the steppers are homed
                steppers[i].state = READY;
            }
        }
    }
    Serial.println ("Stepper calibration finished");
}

void
setStepperPosition (uint8_t stepper_num, float degrees)
{
    // Function to update the target position and direction of the steppers before actuation
    // Serial.println ("Setting position");
    if (stepper_num >= MAX_STEPPERS)
    {
        return;
    }

    // long target_step = degrees / DEG_PER_STEP;
    // steppers[stepper_num].target_pos = target_step;
    steppers[stepper_num].target_pos = degrees / DEG_PER_STEP;

    // Adjust the direction according to the desired step position
    bool direction
        = steppers[stepper_num].target_pos > steppers[stepper_num].current_pos ? HIGH : LOW;
    digitalWrite (steppers[stepper_num].dir_pin, direction);

    steppers[stepper_num].state = MOVING;
    // Serial.println (steppers[stepper_num].target_pos);
    // Serial.println(steppers[stepper_num].state);
}

void
updateSteppers ()
{
    Serial.println ("Updating steppers");

    // Handle normal movement
    int count[2] = {};
    while (steppers[0].state == MOVING || steppers[1].state == MOVING)
    {
        for (int i = 0; i < MAX_STEPPERS; i++)
        {
            if (steppers[i].current_pos != steppers[i].target_pos)
            {
                if (micros () - steppers[i].lastStepTime > stepDelay)
                {
                    // Serial.print("Moving stepper "); Serial.println(i);
                    // digitalWrite(steppers[i].step_pin, HIGH);
                    // delayMicroseconds(2);
                    // digitalWrite(steppers[i].step_pin, LOW);
                    steppers[i].lastStepTime = micros ();
                    steppers[i].stepState = !steppers[i].stepState;
                    digitalWrite (steppers[i].step_pin, steppers[i].stepState);

                    count[i]++;

                    steppers[i].current_pos
                        += (steppers[i].target_pos > steppers[i].current_pos) ? 1 : -1;
                }
            }
            else
            {
                steppers[i].state = READY;
                // digitalWrite(steppers[i].enable_pin, HIGH); // Disable after movement
            }
        }
    }
    // Serial.println ("Finished stepper motions");
    // Serial.println(steppers[0].current_pos);
    // Serial.println(steppers[1].current_pos);
    // Serial.println (count[0]);
    // Serial.println (count[1]);
}
