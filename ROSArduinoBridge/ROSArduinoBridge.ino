/*********************************************************************
 *  ROSArduinoBridge

    A set of simple serial commands to control a differential drive
    robot and receive back sensor and odometry data. Default
    configuration assumes use of an Arduino Mega + Pololu motor
    controller shield + Robogaia Mega Encoder shield.  Edit the
    readEncoder() and setMotorSpeed() wrapper functions if using
    different motor controller or encoder method.

    Created for the Pi Robot Project: http://www.pirobot.org
    and the Home Brew Robotics Club (HBRC): http://hbrobotics.org

    Authors: Patrick Goebel, James Nugen

    Inspired and modeled after the ArbotiX driver by Michael Ferguson

    Software License Agreement (BSD License)

    Copyright (c) 2012, Patrick Goebel.
    All rights reserved.
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

     * Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above
       copyright notice, this list of conditions and the following
       disclaimer in the documentation and/or other materials provided
       with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#define USE_BASE // Enable the base controller code
//#undef USE_BASE     // Disable the base controller code

#define ARDUINO_MEGA_2560
// #define ARDUINO_UNO_R3

#define TWO_MOTORS
// #define FOUR_MOTORS

/* Define the motor controller and encoder library you are using */
#ifdef USE_BASE
/* The Pololu VNH5019 dual motor driver shield */
//#define POLOLU_VNH5019

/* The Pololu MC33926 dual motor driver shield */
//#define POLOLU_MC33926

/* The RoboGaia encoder shield */
//#define ROBOGAIA

/* Encoders directly attached to Arduino board */
#define ARDUINO_ENC_COUNTER

/* L298 Motor driver*/
#define L298_MOTOR_DRIVER
#endif

//#define USE_SERVOS  // Enable use of PWM servos as defined in servos.h
#undef USE_SERVOS // Disable use of PWM servos

/* Serial port baud rate */
#define BAUDRATE 57600

/* Maximum PWM signal */
#define MAX_PWM 255

/* Count per revolution */
#define CPR 1320

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/* Include definition of serial commands */
#include "commands.h"

/* Sensor functions */
#include "sensors.h"

/* Include servo support if required */
#ifdef USE_SERVOS
#include "servos.h"
#include <Servo.h>
#endif

#ifdef USE_BASE
/* Motor driver function definitions */
#include "motor_driver.h"

/* Encoder driver function definitions */
#include "encoder_driver.h"

/* PID parameters and functions */
#include "diff_controller.h"

/* Run the PID loop at 30 times per second */
#define PID_RATE 30 // Hz

/* Convert the rate into an interval */
const int PID_INTERVAL = 1000 / PID_RATE;

/* Track the next time we make a PID calculation */
unsigned long nextPID = PID_INTERVAL;

/* Stop the robot if it hasn't received a movement command
 in this number of milliseconds */
#define AUTO_STOP_INTERVAL 2000
long lastMotorCommand = AUTO_STOP_INTERVAL;
#endif

// Include the stepper motor and limit switch stuff
#include "stepper_driver.h"

/* Variable initialization */

// A pair of varibles to help parse serial commands (thanks Fergs)
int arg = 0;
int index = 0;

// Variable to hold an input character
char chr;

// Variable to hold the current single-character command
char cmd[16];

// Character arrays to hold the first and second arguments
char argv1[16];
char argv2[16];
char argv3[16];
char argv4[16];

// The arguments converted to integers
long arg1;
long arg2;
long arg3;
long arg4;
// long cmd;

/* Clear the current command parameters */
void
resetCommand ()
{
    memset (cmd, 0, sizeof (cmd));
    memset (argv1, 0, sizeof (argv1));
    memset (argv2, 0, sizeof (argv2));
    memset (argv3, 0, sizeof (argv3));
    memset (argv4, 0, sizeof (argv4));
    arg1 = 0;
    arg2 = 0;
    arg3 = 0;
    arg4 = 0;
    arg = 0;
    index = 0;
}

/* Run a command.  Commands are defined in commands.h */
int
runCommand ()
{
    int i = 0;
    char *p = argv1;
    char *str;
    int pid_args[4];
    arg1 = atoi (argv1);
    arg2 = atoi (argv2);
    arg3 = atoi (argv3);
    arg4 = atoi (argv4);

    if (strcmp (cmd, STEPPER_CALIBRATE) == 0)
    {
        Serial.println ("Calibrating the steppers");
        for (int i = 0; i < MAX_STEPPERS; i++)
        {
            steppers[i].homed = false;
        }
        calibrateSteppers ();
    }
    else if (strcmp (cmd, STEPPER_WRITE) == 0)
    {
        Serial.println (arg1);
        Serial.println (arg2);
        if (strlen (argv1) > 0 && strlen (argv2) > 0)
        {
            Serial.print ("Actuating stepper ");
            Serial.println (argv1);
            setStepperPosition (arg1, arg2);
            updateSteppers ();
        }
        else
        {
            Serial.println ("Invalid command");
        }
    }
    else if (strcmp (cmd, STEPPER_WRITE_ALL) == 0)
    {
        Serial.println (arg1);
        Serial.println (arg2);
        if (strlen (argv1) > 0 && strlen (argv2) > 0)
        {
            Serial.println ("Actuating all steppers");
            setStepperPosition (0, arg1);
            setStepperPosition (1, arg2);
            updateSteppers ();
        }
        else
        {
            Serial.println ("Argument must not be empty");
        }
    }
  #ifdef USE_BASE
      else if (strcmp (cmd, READ_ENCODERS) == 0)
      {
          Serial.print (readEncoder (FRONT_LEFT));
          Serial.print (" ");
          Serial.print (readEncoder (FRONT_RIGHT));
          Serial.print (" ");
          Serial.print (readEncoder (REAR_LEFT));
          Serial.print (" ");
          Serial.println (readEncoder (REAR_RIGHT));
      }
      else if (strcmp (cmd, READ_REVS) == 0)
      {
          Serial.print (readEncoder (REAR_RIGHT) / CPR);
          Serial.print (" ");
          Serial.print (readEncoder (REAR_LEFT) / CPR);
          Serial.print (" ");
          Serial.print (readEncoder (FRONT_LEFT) / CPR);
          Serial.print (" ");
          Serial.println (readEncoder (FRONT_RIGHT) / CPR);
      }
      else if (strcmp (cmd, RESET_ENCODERS) == 0)
      {
          resetEncoders ();
          resetPID ();
          Serial.println ("OK");
      }
      else if (strcmp (cmd, MOTOR_SPEEDS) == 0)
      {
        /* Reset the auto stop timer */
        lastMotorCommand = millis ();
        #ifdef TWO_MOTORS
            if (arg1 == 0 && arg2 == 0)
            {
                setMotorSpeeds (0, 0);
                resetPID ();
                moving = 0;
            }
            else
            {
                moving = 1;
            }
            // Using rearLeftPID for left side in two-motor case
            rearLeftPID.TargetTicksPerFrame = arg1;
            // using rearRightPID for the right side in two-motor case
            rearRightPID.TargetTicksPerFrame = arg2;
            Serial.println ("OK");
        #elif define FOUR_MOTORS
            if (arg1 == 0 && arg2 == 0 && arg3 == 0 && arg4 == 0)
            {
                setMotorSpeeds (0, 0, 0, 0);
                resetPID ();
                moving = 0;
            }
            else
            {
                moving = 1;
            }
            frontLeftPID.TargetTicksPerFrame = arg1;
            frontRightPID.TargetTicksPerFrame = arg2;
            rearLeftPID.TargetTicksPerFrame = arg1;
            rearRightPID.TargetTicksPerFrame = arg2;
            Serial.println ("OK");
        #endif
      }
      else if (strcmp (cmd, MOTOR_RAW_PWM) == 0)
      {
          /* Reset the auto stop timer */
          lastMotorCommand = millis ();
          resetPID ();
          moving = 0; // Sneaky way to temporarily disable the PID

          #ifdef TWO_MOTORS
              setMotorSpeeds (arg1, arg2);
          #elif define FOUR_MOTORS
              setMotorSpeeds (arg1, arg2, arg3, arg4)
          #endif
              Serial.println ("OK");
      }
      else if (strcmp (cmd, UPDATE_PID) == 0)
      {
          while ((str = strtok_r (p, ":", &p)) != '\0')
          {
              pid_args[i] = atoi (str);
              i++;
          }
          Kp = pid_args[0];
          Kd = pid_args[1];
          Ki = pid_args[2];
          Ko = pid_args[3];
          Serial.println ("OK");
      }
  #endif
    else
    {
        Serial.println ("Invalid command");
    }
}

/* Setup function--runs once at startup. */
void
setup ()
{
    Serial.begin (BAUDRATE);
    Serial.println ("Setting things up");
    delay (1000);

// Initialize the motor controller if used */
#if defined(USE_BASE) && defined(ARDUINO_ENC_COUNTER)
    #if defined(ARDUINO_MEGA_2560)
        // set as inputs
        DDRB &= ~(1 << REAR_RIGHT_ENC_PIN_A);
        DDRB &= ~(1 << REAR_RIGHT_ENC_PIN_B);
        DDRJ &= ~(1 << FRONT_LEFT_ENC_PIN_A);
        DDRJ &= ~(1 << FRONT_LEFT_ENC_PIN_B);
        DDRK &= ~(1 << REAR_LEFT_ENC_PIN_A);
        DDRK &= ~(1 << REAR_LEFT_ENC_PIN_B);
        pinMode (FRONT_RIGHT_ENC_PIN_A, INPUT_PULLUP);
        pinMode (FRONT_RIGHT_ENC_PIN_B, INPUT_PULLUP);

        // enable pull up resistors
        PORTB |= (1 << REAR_RIGHT_ENC_PIN_A);
        PORTB |= (1 << REAR_RIGHT_ENC_PIN_B);
        PORTJ |= (1 << FRONT_LEFT_ENC_PIN_A);
        PORTJ |= (1 << FRONT_LEFT_ENC_PIN_B);
        PORTK |= (1 << REAR_LEFT_ENC_PIN_A);
        PORTK |= (1 << REAR_LEFT_ENC_PIN_B);

        // // NOTE:
        // To use the PCINT pins on port J, we must
        // disable the hardware UART TX3 and RX3 pins.
        UCSR3B &= ~(_BV (TXEN3));  // disable UART TX
        UCSR3B &= ~(_BV (RXEN3));  // disable UART RX
        UCSR3B &= ~(_BV (TXCIE3)); // disable UART TX Interrupt
        UCSR3B &= ~(_BV (RXCIE3)); // disable UART RX Interrupt

        PCICR |= (1 << PCIE0);
        PCICR |= (1 << PCIE1);
        PCICR |= (1 << PCIE2);

        // tell pin change mask to listen to left encoder pins
        PCMSK0 |= (1 << REAR_RIGHT_ENC_PIN_A_PCINT) | (1 << REAR_RIGHT_ENC_PIN_B_PCINT);
        PCMSK1 |= (1 << FRONT_LEFT_ENC_PIN_A_PCINT) | (1 << FRONT_LEFT_ENC_PIN_B_PCINT);
        PCMSK2 |= (1 << REAR_LEFT_ENC_PIN_A_PCINT) | (1 << REAR_LEFT_ENC_PIN_B_PCINT);

        // Attach interrupts on the external interrupt pins
        attachInterrupt(digitalPinToInterrupt(FRONT_RIGHT_ENC_PIN_A),frontRightEncA,CHANGE);
        attachInterrupt(digitalPinToInterrupt(FRONT_RIGHT_ENC_PIN_B),frontRightEncB,CHANGE);
    #elif defined ARDUINO_UNO_R3
        // set as inputs
        DDRB &= ~(1 << LEFT_ENC_PIN_A);
        DDRB &= ~(1 << LEFT_ENC_PIN_B);
        DDRD &= ~(1 << RIGHT_ENC_PIN_A);
        DDRD &= ~(1 << RIGHT_ENC_PIN_B);

        // enable pull up resistors
        PORTB |= (1 << LEFT_ENC_PIN_A);
        PORTB |= (1 << LEFT_ENC_PIN_B);
        PORTD |= (1 << RIGHT_ENC_PIN_A);
        PORTD |= (1 << RIGHT_ENC_PIN_B);

        // tell pin change mask to listen to left encoder pins
        PCMSK0 |= (1 << LEFT_ENC_PIN_A) | (1 << LEFT_ENC_PIN_B);
        // tell pin change mask to listen to right encoder pins
        PCMSK2 |= (1 << RIGHT_ENC_PIN_A) | (1 << RIGHT_ENC_PIN_B);

        // enable interrupt in the general interrupt mask
        PCICR |= (1 << PCIE0) | (1 << PCIE2);
#endif
    // initSteppers ();
    initMotorController ();
    resetPID ();
#endif

    /* Attach servos if used */
#ifdef USE_SERVOS
    int i;
    for (i = 0; i < N_SERVOS; i++)
    {
        servos[i].initServo (servoPins[i], stepDelay[i], servoInitPosition[i]);
    }
#endif
}

/* Enter the main loop.  Read and parse input from the serial port
   and run any valid commands. Run a PID calculation at the target
   interval and check for auto-stop conditions.
*/
void
loop ()
{
    static unsigned long lastPrint = 0;
    // if (millis() - lastPrint > 2000) { // Print once every 2 seconds
    //   Serial.println("Waiting for command");
    //   lastPrint = millis();
    // }
    while (Serial.available () > 0)
    {
        // Read the next character
        chr = Serial.read ();

        // Terminate a command with a CR
        if (chr == 13)
        {
            if (arg == 0)
                cmd[index] = '\0';
            else if (arg == 1)
                argv1[index] = '\0';
            else if (arg == 2)
                argv2[index] = '\0';
            else if (arg == 3)
                argv3[index] = '\0';
            else if (arg == 4)
                argv4[index] = '\0';
            // Serial.print (cmd);
            // Serial.print (" | ");
            // Serial.print (argv1);
            // Serial.print (" | ");
            // Serial.print (argv2);
            // Serial.print (" | ");
            // Serial.print (argv3);
            // Serial.print (" | ");
            // Serial.print (argv4);
            runCommand ();
            resetCommand ();
        }

        // Use spaces to delimit parts of the command
        else if (chr == ' ')
        {
            // Step through the arguments
            if (arg == 0)
            {
                cmd[index] = '\0';
                arg = 1;
                index = 0;
            }
            else if (arg == 1)
            {
                argv1[index] = '\0';
                arg = 2;
                index = 0;
            }
            else if (arg == 2)
            {
                argv2[index] = '\0';
                arg = 3;
                index = 0;
            }
            else if (arg == 3)
            {
                argv3[index] = '\0';
                arg = 4;
                index = 0;
            }
            continue;
        }

        // Normal parsing
        else
        {
            if (arg == 0)
            {
                // The first arg is the single-letter command
                // cmdv = chr;
                if (index < sizeof (cmd) - 1)
                {
                    cmd[index++] = chr;
                }
            }
            else if (arg == 1)
            {
                // Subsequent arguments can be more than one character
                argv1[index] = chr;
                index++;
            }
            else if (arg == 2)
            {
                argv2[index] = chr;
                index++;
            }
            else if (arg == 3)
            {
                argv3[index] = chr;
                index++;
            }
            else if (arg == 4)
            {
                argv4[index] = chr;
                index++;
            }
        }
    }

// If we are using base control, run a PID calculation at the appropriate intervals
#ifdef USE_BASE
    if (millis () > nextPID)
    {
        updatePID ();
        nextPID += PID_INTERVAL;
    }

    // Check to see if we have exceeded the auto-stop interval
    if ((millis () - lastMotorCommand) > AUTO_STOP_INTERVAL)
    {
        ;
#ifdef TWO_MOTORS
        setMotorSpeeds (0, 0);
#elif define FOUR_MOTORS
        setMotorSpeeds (0, 0, 0, 0);
#endif
        moving = 0;
    }
#endif

// Sweep servos
#ifdef USE_SERVOS
    int i;
    for (i = 0; i < N_SERVOS; i++)
    {
        servos[i].doSweep ();
    }
#endif
}
