/*
  6 channel analog PWM controller

  Generic sequencing program for 6 channels of PWM

  // NOTICE
  copyright 2010, 2017 by Steve Boyer
  protected by Creative Commons Attribution-Noncommercial-Share Alike license.
  You are free to share this as long as this notice is included in all copies.
  You may alter the work as long as this notice is included in all derivatives.
  You may not use this for commercial purposes.
  see "http://creativecommons.org/licenses/by-nc-sa/3.0/" for license details
*/

//#define DEBUG
#include "sequences.h"
#include <NewPing.h>

#define TRIGGER_PIN  13// Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     12// Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define heatSensor 8 // heat sensor
#define rogueWhite 7 //white pin non-pwm

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

//#include "tables.h"
//--------------------------------------------------------------------------
// struct channelInfo contains all relevant data concerning each channel
//
typedef struct {
  int           pin;        // Pin number on Arduino of this PWM channel
  int           target;     // target value for PWM
  float         value;      // Current PWM value;
  boolean       analog;     // indicates whether channel is under sequence or analog control
  unsigned int  cmdIndex;   // index into command table (script)
  long           *cmdTable;  // pointer to command table
  float         increment;  // amount to increment value in order to reach target (interpolation increment)
  long          frameCntr;  // Keeps track of individual looping
  long          nextEvent;  // frame count of next event for this channel
} channelInfo;

// Allocate 1 channelInfo structure for each of the 6 PWM channels
channelInfo channels[ NUM_CHANNELS ];
const int interruptPin = 2;


long startFrame;
long frameCntr;
boolean flashingMode;

void setup()                    // run once, when the sketch starts
{
  int i;
  Serial.begin( 115200 ); // for debugging

  // Set the PWM pins to outputs
  channels[ CH_1 ].pin = 10;
  channels[ CH_2 ].pin = 11;
  channels[ CH_3 ].pin = 3;
  channels[ CH_4 ].pin = 5;
  channels[ CH_5 ].pin = 6;
  channels[ CH_6 ].pin = 9;

  // Initialize all channel members
  for ( i = 0; i < NUM_CHANNELS; i++ ) {
    pinMode( channels[ i ].pin, OUTPUT );
    channels[ i ].value = 0;
    channels[ i ].target = 0;
    channels[ i ].increment = 0.0;
    channels[ i ].analog = false;
    channels[ i ].cmdIndex = 0;
    channels[ i ].cmdTable = 0;
    channels[ i ].nextEvent = channels[ i ].cmdTable[ 0 ];
    channels[ i ].frameCntr = 0;

  }
  flashingMode = true;

//on sequence

  assignChannelSequence( CH_1, sequence2 );
  assignChannelSequence( CH_2, sequence2 );
  assignChannelSequence( CH_3, sequence2 );
  assignChannelSequence( CH_4, sequence2 );
  assignChannelSequence( CH_5, sequence2 );
  assignChannelSequence( CH_6, sequence2 );



  pinMode( interruptPin, INPUT );   // Set the interrupt pin to pin 2. pin3 interrupt cannot be used as it is needed for PWM
  // attachInterrupt( 0, cycleSequence, FALLING );

  // Initialize the frame timing system
  frameCntr = 0l;
  startFrame = millis();
  
  pinMode(rogueWhite, OUTPUT);
  digitalWrite(rogueWhite, LOW);
  pinMode(heatSensor, INPUT);

}

//-----------------------------------------------------------------------------
//
// void loop( void )
//
// Arduino-imposed main loop.
// There is an auto-delay feature built-in (like a clock escapement!) that
// locks the loop into 1 millisecond frames. Although it is possible for the
// loop processing to take more than 1 millisicond error is not accumulated and
// delays in processing will only result in he potential for events to occur
// 1 or 2 milliseconds late. Everything else automatically synchs up when there
// is a hiccup.
//
void loop()
{

  int i, j;
  long currentCntr;

  // Checks to see if there are any events to be handled.

  checkEventQueue();

  // Updates the event counters, timers and incrememtn values for each channel.
  updateChannels();

  // Wait for frame to advance
  currentCntr = millis();
  while ( currentCntr == millis() ) {

  }; // wait for next frame
  gameLogic();

  frameCntr++;
}

//-----------------------------------------------------------------------------
//
// void gameLogic( void )
//
// Place your game logic here.
// DO NOT use the Delay() function in this section!
void gameLogic( void ) {
  // place all of your logic here.
  // for example:
  // check for input
  // if switch1 pressed
  //    operatingMode = LASERPULSE;
  //    initiateLaserPulse();
  // if switch 2 pressed
  //    operatingMode = RAPIDFIRE;
  //    initiateRapidFire();
  // etc.



  return;
}



//-----------------------------------------------------------------------------
//
// void checkEventQueue( void )
//
// operation:
//     This is the engine that parses the channels scripts and keeps track
//     of event timing. Each channel object has a member that is the frame # of the
//     next event. This method checks the current frame number and ses if there is an
//     event in the script ready to run.
//     This method then processes the event item in the channel script.
//     The following events are supported:
//
//      START_FADE
//      END_FADE
//      SET_VALUE
//      ANALOG
//      LOOP

void checkEventQueue( void )
{

  int i, command;
  float percent;
  long nextEvent, duration;

  for ( i = 0; i < NUM_CHANNELS; i++ ) {

    if ( channels[ i ].frameCntr >= channels[ i ].nextEvent ) {

      channels[ i ].cmdIndex++;
      command = channels[ i ].cmdTable[ channels[ i ].cmdIndex++ ];
      switch ( command )
      {
        case START_FADE:
          percent = channels[ i ].cmdTable[ channels[ i ].cmdIndex++ ] * 2.55;
          channels[ i ].value = percent;
          nextEvent =  channels[ i ].cmdTable[ channels[ i ].cmdIndex++ ];
          channels[ i ].cmdIndex++; // skip past END_FRAME command
          percent = channels[ i ].cmdTable[ channels[ i ].cmdIndex++ ] * 2.55;
          channels[ i ].target = percent;
          duration = nextEvent - channels[ i ].frameCntr;
          channels[ i ].increment = ( channels[ i ].target - channels[ i ].value ) / duration;

#ifdef DEBUG
          Serial.print( channels[ i ].frameCntr );
          Serial.print( ": value = " );
          Serial.print( channels[ i ].value );
          Serial.print( ", target = " );
          Serial.print( channels[ i ].target );
          Serial.print( ", duration = " );
          Serial.print( duration );
          Serial.print( ", increment = " );
          Serial.print( channels[ i ].increment );
#endif
          break;
        case END_FADE:
          channels[ i ].cmdIndex++;
          nextEvent =  channels[ i ].cmdTable[ channels[ i ].cmdIndex ];
          break;
        case SET_VALUE:
          percent = channels[ i ].cmdTable[ channels[ i ].cmdIndex++ ] * 2.55;
          channels[ i ].value = percent;
          channels[ i ].increment = 0;
          break;
        case LOOP:
          channels[ i ].cmdIndex = 0;
          channels[ i ].frameCntr = 0l;
          channels[ i ].nextEvent = channels[ i ].cmdTable[ channels[ i ].cmdIndex ];
          sequenceSwitcher();
          break;
        case ANALOG:
          channels[ i ].analog = true;
          channels[ i ].cmdIndex++;
          break;
        default:
          break;

      }
      channels[ i ].nextEvent = channels[ i ].cmdTable[ channels[ i ].cmdIndex ];
    }
  }
}

//-----------------------------------------------------------------------------
//
// void updateChannels ( void )
//
void updateChannels( void )
{

  int i, analogValue;

  for ( i = 0; i < NUM_CHANNELS; i++ ) {
    if ( channels[ i ].analog ) {
      analogValue = analogRead( i );
      analogValue >>= 2;
      channels[ i ].value = (byte)analogValue;
    }
    else {
      channels[ i ].value += channels[ i ].increment;
      if ( channels[ i ].increment > 0 ) {
        if ( channels[ i ].value >= channels[ i ].target ) {
          channels[ i ].increment = 0;
          channels[ i ].value = channels[ i ].target;
        }
      }
      else if ( channels[ i ].increment < 0 ) {
        if ( channels[ i ].value <= channels[ i ].target ) {
          channels[ i ].increment = 0;
          channels[ i ].value = channels[ i ].target;
        }
      }
    }

    // Main engine - just updates the PWM value for each channel every millisecond.
    analogWrite( channels[ i ].pin, (unsigned byte)channels[ i ].value );
    channels[ i ].frameCntr++;
  }
}

//-----------------------------------------------------------------------------
//
// assignChannelSequence( channel, sequence )
//
// this function allows you to assign a new sequence to a PWM channel.
// the new sequence will begin executing from the beginning of the sequence.
// to synchronize multiple channels you can assign the sequences at the same time.
// if synchronous operation is not necessary each channel can be assigned asynchronously.
//
void assignChannelSequence( int channel, long *sequence ) {
  channels[ channel ].cmdTable = sequence;
  channels[ channel ].nextEvent = sequence[ 0 ];
  channels[ channel ].cmdIndex = 0;
  channels[ channel ].value = 0;
  channels[ channel ].target = 0;
  channels[ channel ].increment = 0.0;
  channels[ channel ].frameCntr = 0;

}

void sequenceSwitcher() {
  unsigned int uS = sonar.ping();
  Serial.print("Distance: ");
  Serial.println(uS); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  int digitalVal;

  digitalVal = digitalRead(heatSensor); 


  if (uS > 600) {
    assignChannelSequence( CH_1, fast1 );  
    assignChannelSequence( CH_2, fast3 );
    assignChannelSequence( CH_3, fast2 );
    assignChannelSequence( CH_4, fast1 );
    assignChannelSequence( CH_5, fast2 );
    assignChannelSequence( CH_6, fast3 );
    return;
  }
  else if (digitalVal == HIGH) // if temperature threshold reached
  {

    digitalWrite(rogueWhite, HIGH);
    assignChannelSequence( CH_1, 0 );
    assignChannelSequence( CH_2, 0 );
    assignChannelSequence( CH_3, onnoff );
    assignChannelSequence( CH_4, 0 );
    assignChannelSequence( CH_5, 0 );
    assignChannelSequence( CH_6, onnoff );
    return;
  }
  else {
    digitalWrite(rogueWhite, LOW); // turn OFF Arduino's LED
    assignChannelSequence( CH_1, o0 );
    assignChannelSequence( CH_2, o1 );
    assignChannelSequence( CH_3, 0 );
    assignChannelSequence( CH_4, o2 );
    assignChannelSequence( CH_5, o3 );
    assignChannelSequence( CH_6, pingreader );
    return;
  }

}
void cycleSequence( void ) {

  return;
}
