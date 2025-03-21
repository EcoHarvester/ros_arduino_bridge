/* Define single-letter commands that will be sent by the PC over the
   serial link.
*/

#ifndef COMMANDS_H
#define COMMANDS_H

// // #define ANALOG_READ    'a'
// // #define GET_BAUDRATE   'b'
// // #define PIN_MODE       'c'
// // #define DIGITAL_READ   'd'
// #define READ_ENCODERS  'e'
// #define READ_REVS      'f'
// #define MOTOR_SPEEDS   'm'
// #define MOTOR_RAW_PWM  'o'
// // #define PING           'p'
// #define RESET_ENCODERS 'r'
// #define SERVO_WRITE    's'
// #define SERVO_READ     't'
// #define STEPPER_CALIBRATE 'stc'
// #define STEPPER_WRITE  'stw'
// #define UPDATE_PID     'u'
// // #define DIGITAL_WRITE  'w'
// // #define ANALOG_WRITE   'x'
// #define LEFT            0
// #define RIGHT           1

#define READ_ENCODERS "e"
#define READ_REVS "rev"
#define RESET_ENCODERS "r"
#define MOTOR_SPEEDS "m"
#define MOTOR_RAW_PWM "o"
#define SERVO_WRITE
#define SERVO_READ
#define UPDATE_PID "u"
// #define LEFT              0
// #define RIGHT             1

#define REAR_RIGHT 0
#define REAR_LEFT 1
#define FRONT_LEFT 2
#define FRONT_RIGHT 3

/* STEPPER COMMANDS */
#define STEPPER_CALIBRATE "stc"

// stw <stepper_idx> <angle>
#define STEPPER_WRITE "stw"

// sta <stepper_1_angle> <stepper_2_angle>
#define STEPPER_WRITE_ALL "sta"

#endif
