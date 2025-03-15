/* Functions and type-defs for PID control.

   Taken mostly from Mike Ferguson's ArbotiX code which lives at:
   
   http://vanadium-ros-pkg.googlecode.com/svn/trunk/arbotix/
*/

/* PID setpoint info For a Motor */
typedef struct {
  double TargetTicksPerFrame;    // target speed in ticks per frame
  long Encoder;                  // encoder count
  long PrevEnc;                  // last encoder count

  /*
  * Using previous input (PrevInput) instead of PrevError to avoid derivative kick,
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-derivative-kick/
  */
  int PrevInput;                // last input
  //int PrevErr;                   // last error

  /*
  * Using integrated term (ITerm) instead of integrated error (Ierror),
  * to allow tuning changes,
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
  */
  //int Ierror;
  int ITerm;                    //integrated term

  long output;                    // last motor setting
}
SetPointInfo;

// TO-DO: Add PID for all four wheels
SetPointInfo rearRightPID, rearLeftPID, frontLeftPID, frontRightPID;

/* PID Parameters */
int Kp = 20;
int Kd = 12;
int Ki = 0;
int Ko = 50;

unsigned char moving = 0; // is the base in motion?

/*
* Initialize PID variables to zero to prevent startup spikes
* when turning PID on to start moving
* In particular, assign both Encoder and PrevEnc the current encoder value
* See http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-initialization/
* Note that the assumption here is that PID is only turned on
* when going from stop to moving, that's why we can init everything on zero.
*/
void resetPID(){
  rearRightPID.TargetTicksPerFrame = 0.0;
  rearRightPID.Encoder = readEncoder(REAR_RIGHT);
  rearRightPID.PrevEnc = rearRightPID.Encoder;
  rearRightPID.output = 0;
  rearRightPID.PrevInput = 0;
  rearRightPID.ITerm = 0;

  rearLeftPID.TargetTicksPerFrame = 0.0;
  rearLeftPID.Encoder = readEncoder(REAR_LEFT);
  rearLeftPID.PrevEnc = rearLeftPID.Encoder;
  rearLeftPID.output = 0;
  rearLeftPID.PrevInput = 0;
  rearLeftPID.ITerm = 0;

  frontLeftPID.TargetTicksPerFrame = 0.0;
  frontLeftPID.Encoder = readEncoder(FRONT_RIGHT);
  frontLeftPID.PrevEnc = frontLeftPID.Encoder;
  frontLeftPID.output = 0;
  frontLeftPID.PrevInput = 0;
  frontLeftPID.ITerm = 0;

  frontRightPID.TargetTicksPerFrame = 0.0;
  frontRightPID.Encoder = readEncoder(FRONT_RIGHT);
  frontRightPID.PrevEnc = frontRightPID.Encoder;
  frontRightPID.output = 0;
  frontRightPID.PrevInput = 0;
  frontRightPID.ITerm = 0;

}

/* PID routine to compute the next motor commands */
void doPID(SetPointInfo * p) {
  long Perror;
  long output;
  int input;

  //Perror = p->TargetTicksPerFrame - (p->Encoder - p->PrevEnc);
  input = p->Encoder - p->PrevEnc;
  Perror = p->TargetTicksPerFrame - input;


  /*
  * Avoid derivative kick and allow tuning changes,
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-derivative-kick/
  * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
  */
  //output = (Kp * Perror + Kd * (Perror - p->PrevErr) + Ki * p->Ierror) / Ko;
  // p->PrevErr = Perror;
  output = (Kp * Perror - Kd * (input - p->PrevInput) + p->ITerm) / Ko;
  p->PrevEnc = p->Encoder;

  output += p->output;
  // Accumulate Integral error *or* Limit output.
  // Stop accumulating when output saturates
  if (output >= MAX_PWM)
    output = MAX_PWM;
  else if (output <= -MAX_PWM)
    output = -MAX_PWM;
  else
  /*
  * allow turning changes, see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-tuning-changes/
  */
    p->ITerm += Ki * Perror;

  p->output = output;
  p->PrevInput = input;
}

/* Read the encoder values and call the PID routine */
void updatePID() {
  /* Read the encoders */
  rearRightPID.Encoder = readEncoder(REAR_RIGHT);
  rearLeftPID.Encoder = readEncoder(REAR_LEFT);
  frontLeftPID.Encoder = readEncoder(FRONT_LEFT);
  frontRightPID.Encoder = readEncoder(FRONT_RIGHT);
  
  /* If we're not moving there is nothing more to do */
  if (!moving){
    /*
    * Reset PIDs once, to prevent startup spikes,
    * see http://brettbeauregard.com/blog/2011/04/improving-the-beginner%E2%80%99s-pid-initialization/
    * PrevInput is considered a good proxy to detect
    * whether reset has already happened
    */
    if (rearRightPID.PrevInput != 0 || rearLeftPID.PrevInput != 0 || frontLeftPID.PrevInput != 0 || frontRightPID.PrevInput != 0) resetPID();
    return;
  }

  /* Compute PID update for each motor */
  doPID(&rearRightPID);
  doPID(&rearLeftPID);
  doPID(&frontLeftPID);
  doPID(&frontRightPID);

  /* Set the motor speeds accordingly */
  #ifdef TWO_MOTORS
    setMotorSpeeds(rearLeftPID.output, rearRightPID.output);
  #elif define FOUR_MOTORS
    setMotorSpeeds(frontLeftPID.output, frontRightPID.output, rearLeftPID.output, rearRightPID.output)
  #endif
}

