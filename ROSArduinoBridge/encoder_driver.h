/* *************************************************************
   Encoder driver function definitions - by James Nugen
   ************************************************************ */
   
   
#ifdef ARDUINO_ENC_COUNTER
  #ifdef ARDUINO_MEGA_2560
    // Using pin 0 and 1 on port B for left encoder reading (PCINT0 and PCINT1 on PCMSK0) 
    #define LEFT_ENC_PIN_A PB0  // digital pin 53
    #define LEFT_ENC_PIN_B PB1  // digital pin 52

    // Using pin 0 and 1 on port K for right encoder reading (PCINT16 and PCINT17 on PCMSK1) 
    #define RIGHT_ENC_PIN_A PK0   // analog pin 8
    #define RIGHT_ENC_PIN_B PK1   // analog pin 9
  #elif defined(ARDUINO_UNO)
    // Using pin 0 and 1 on port B for left encoder reading (PCINT0 and PCINT1 on PCMSK0)
    #define LEFT_ENC_PIN_A PB0  // digital pin 8
    #define LEFT_ENC_PIN_B PB1  // digital pin 9
    
    #define RIGHT_ENC_PIN_A PC4   // analog pin 4
    #define RIGHT_ENC_PIN_B PC5   // analog pin 5

  #endif
#endif
   
long readEncoder(int i);
void resetEncoder(int i);
void resetEncoders();