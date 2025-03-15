/* *************************************************************
   Encoder definitions
   
   Add an "#ifdef" block to this file to include support for
   a particular encoder board or library. Then add the appropriate
   #define near the top of the main ROSArduinoBridge.ino file.
   
   ************************************************************ */
   
#ifdef USE_BASE

#ifdef ROBOGAIA
  /* The Robogaia Mega Encoder shield */
  #include "MegaEncoderCounter.h"

  /* Create the encoder shield object */
  MegaEncoderCounter encoders = MegaEncoderCounter(4); // Initializes the Mega Encoder Counter in the 4X Count mode
  
  /* Wrap the encoder reading function */
  long readEncoder(int i) {
    if (i == LEFT) return encoders.YAxisGetCount();\category\coordinate-transforms-for-robotics
    else return encoders.XAxisGetCount();
  }

  /* Wrap the encoder reset function */
  void resetEncoder(int i) {
    if (i == LEFT) return encoders.YAxisReset();
    else return encoders.XAxisReset();
  }
#elif defined(ARDUINO_ENC_COUNTER)
  // volatile long left_enc_pos = 0L;
  // volatile long right_enc_pos = 0L;
  
  volatile long rear_right_enc_pos = 0L;
  volatile long rear_left_enc_pos = 0L;
  volatile long front_left_enc_pos = 0L;
  volatile long front_right_enc_pos = 0L;
  
  static const int8_t ENC_STATES [] = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};  //encoder lookup table

  /* Arduino MEGA2560 encoder interrupts */
  #ifdef ARDUINO_MEGA_2560
    /* Interrupt routine for LEFT encoder, taking care of actual counting */
    ISR (REAR_RIGHT_ENC_PCINT_VECT){    // Interrupt on port B
      static uint8_t enc_last=0;
          
      enc_last <<=2; //shift previous state two places
      // enc_last |= PINB & 0x03; //read the current state into lowest 2 bits
      enc_last |= PINB & ((1 << REAR_RIGHT_ENC_PIN_A) | (1 << REAR_RIGHT_ENC_PIN_B));
      rear_right_enc_pos += ENC_STATES[(enc_last & 0x0f)];
    }
    
    /* Interrupt routine for RIGHT encoder, taking care of actual counting */
    ISR (REAR_LEFT_ENC_PCINT_VECT){  // Interrupt on port K
      static uint8_t enc_last=0;
              
      enc_last <<=2; //shift previous state two places
      enc_last |= PINK & ((1 << REAR_LEFT_ENC_PIN_A) | (1 << REAR_LEFT_ENC_PIN_B)); //read the current state into lowest 2 bits
    
      rear_left_enc_pos += ENC_STATES[(enc_last & 0x0f)];
    }

    ISR (FRONT_LEFT_ENC_PCINT_VECT) {  // Interrupt on port J
      static uint8_t enc_last=0;
              
      enc_last <<=2; //shift previous state two places
      enc_last |= PINJ & ((1 << FRONT_LEFT_ENC_PIN_A) | (1 << FRONT_LEFT_ENC_PIN_B)); //read the current state into lowest 2 bits
    
      front_left_enc_pos += ENC_STATES[(enc_last & 0x0f)];
    }

    void frontRightEncA() {
      // Determine direction by checking Encoder B
      if (digitalRead(FRONT_RIGHT_ENC_PIN_A) == digitalRead(FRONT_RIGHT_ENC_PIN_B)) {
          front_right_enc_pos++;  // Forward
      } else {
          front_right_enc_pos--;  // Backward
      }
    }

    void frontRightEncB() {
      // Determine direction by checking Encoder A
      if (digitalRead(FRONT_RIGHT_ENC_PIN_A) == digitalRead(FRONT_RIGHT_ENC_PIN_B)) {
          front_right_enc_pos--;  // Backward
      } else {
          front_right_enc_pos++;  // Forward
      }
    }
  
  #elif defined(ARDUINO_UNO)
    /* Interrupt routine for LEFT encoder, taking care of actual counting */
      ISR (PCINT0_vect){    // Interrupt on port B
        static uint8_t enc_last=0;
            
        enc_last <<=2; //shift previous state two places
        enc_last |= PINB & 0x03; //read the current state into lowest 2 bits
      
        left_enc_pos += ENC_STATES[(enc_last & 0x0f)];
      }
      
      /* Interrupt routine for RIGHT encoder, taking care of actual counting */
      ISR (PCINT2_vect){    // Interrupt on port D
        static uint8_t enc_last=0;
                
        enc_last <<=2; //shift previous state two places
        enc_last |= (PIND & (3 << 2)) >> 2; //read the current state into lowest 2 bits
      
        right_enc_pos += ENC_STATES[(enc_last & 0x0f)];
      }

  #endif
  
  /* Wrap the encoder reading function */
  long readEncoder(int i) {
    if (i == REAR_RIGHT) return rear_right_enc_pos;
    else if (i == REAR_LEFT) return rear_left_enc_pos;
    else if (i == FRONT_LEFT) return front_left_enc_pos;
    else return front_right_enc_pos;
  }

  /* Wrap the encoder reset function */
  void resetEncoder(int i) {
    if (i == REAR_RIGHT){
      rear_right_enc_pos = 0L;
      return;
    } 
    else if (i == REAR_LEFT) { 
      rear_left_enc_pos = 0L;
      return;
    }
    else if (i == FRONT_LEFT) {
      front_left_enc_pos = 0L;      
    }
    else {
      front_right_enc_pos = 0L;
    }
  }
#else
  #error A encoder driver must be selected!
#endif

/* Wrap the encoder reset function */
void resetEncoders() {
  resetEncoder(REAR_RIGHT);
  resetEncoder(REAR_LEFT);
  resetEncoder(FRONT_LEFT);
  resetEncoder(FRONT_RIGHT);
}

#endif

