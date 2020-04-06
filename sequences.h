//-----------------------------------------------------------------
//
// sequences.h
//
// How to use this file:
// you may make changes to the sequences below (or you can create new sequences using the commands below)
// By default, when you run this sketch the sequences below are automatically assigned to a PWM channel
//
// These are the 'commands' to which the scripts below can respond.
//

#define START_FADE   0x01
#define END_FADE     0x02
#define SET_VALUE    0x03
#define STOP         0x04
#define LOOP         0x05
#define ANALOG       0x06
#define ANALOG_RATE  0x07
#define END          0x08

// Defines the channel numbers
//
#define CH_1 0
#define CH_2 1
#define CH_3 2
#define CH_4 3
#define CH_5 4
#define CH_6 5

#define NUM_CHANNELS 6
#define START_SEQUENCE long
#define _SEQUENCE [] = {
#define END_SEQUENCE };


// These are the actual sequence scripts that you will modify to produce your animations.
// You may assign any script to any channel in the main loop of your program.
//
// Each line consists of 3 numbers.
// The first number is a time value (in milliseconds, that indicayes when to start the command (relative to the start of the sequence)
// The second 'number' is a command that instructs the code what to do specifically with that channel
// the third number is a value that the command needs to execute.
// in the example below, (sequence1), at the very beginning of the program (0 milliseconds) we begin a fade starting at value 0.
// at 1000 (1 second) we assign an end to the fade with a value of 100. So between 0 and 1000 ms the sequence will fade from 0 to 10 in 1 second.
// at 1001 we start to fade back to 0.
// at 2000 the fade to 0 is complete
// at 2001 we loop back to 0.
// This code will run on Channel 1 forever.
// The default assignments are as follows:
//    CH_1 will run sequence1
//    CH_2 will run sequence2
//    CH_3 will run sequence3
//    CH_4 will run sequence4
//    CH_5 will run sequence5
//    CH_6 will run sequence6
// SEQUENCES START HERE!!!!!

START_SEQUENCE null_sequence _SEQUENCE
0, SET_VALUE, 0,
100, LOOP, 0
END_SEQUENCE

START_SEQUENCE fast1 _SEQUENCE 
0, SET_VALUE, 0,
0, START_FADE, 0,
50, END_FADE, 100,
51, START_FADE, 100,
100, END_FADE, 0,
200, LOOP, 0
END_SEQUENCE

START_SEQUENCE fast2 _SEQUENCE 
0, SET_VALUE, 0,
0, START_FADE, 0,
75, END_FADE, 100,
76, START_FADE, 100,
110, END_FADE, 0,
200, LOOP, 0
END_SEQUENCE

START_SEQUENCE fast3 _SEQUENCE 
0, SET_VALUE, 0,
0, START_FADE, 0,
100, END_FADE, 100,
101, START_FADE, 100,
125, END_FADE, 0,
200, LOOP, 0
END_SEQUENCE

START_SEQUENCE o0 _SEQUENCE
0, SET_VALUE, 0,
0, START_FADE, 0,
1000, END_FADE, 100,
1050, START_FADE, 100,
2050, END_FADE, 0,
5800, LOOP, 0
END_SEQUENCE

START_SEQUENCE o1 _SEQUENCE
0, SET_VALUE, 0,
750, START_FADE, 0,
1750, END_FADE, 100,
1800, START_FADE, 100,
2800, END_FADE, 0,
5800, LOOP, 0

END_SEQUENCE

START_SEQUENCE o2 _SEQUENCE
0, SET_VALUE, 0,
1500, START_FADE, 0,
2500, END_FADE, 100,
2550, START_FADE, 100,
3550, END_FADE, 0,
5800, LOOP, 0
END_SEQUENCE

START_SEQUENCE o3 _SEQUENCE
0, SET_VALUE, 0,
2250, START_FADE, 0,
3250, END_FADE, 100,
3300, START_FADE, 100,
4300, END_FADE, 0,
5800, LOOP, 0
END_SEQUENCE

START_SEQUENCE o4 _SEQUENCE
0, SET_VALUE, 0,
3000, START_FADE, 0,
4000, END_FADE, 100,
4050, START_FADE, 100,
5050, END_FADE, 0,
5800, LOOP, 0
END_SEQUENCE

START_SEQUENCE o5 _SEQUENCE
0, SET_VALUE, 0,
3750, START_FADE, 0,
4750, END_FADE, 100,
4800, START_FADE, 100,
5800, END_FADE, 0,
5801, LOOP, 0
END_SEQUENCE


//-------------------------------------
// SEQUENCE2
START_SEQUENCE sequence2 _SEQUENCE
0, SET_VALUE, 100,
1, LOOP, 0
END_SEQUENCE

//-------------------------------------
// SEQUENCE3

START_SEQUENCE pingreader _SEQUENCE
0, SET_VALUE, 0,
4000, LOOP, 0
END_SEQUENCE


START_SEQUENCE onnoff _SEQUENCE
0, SET_VALUE, 0,
0, START_FADE, 0,
1000, END_FADE, 100,
1050, START_FADE, 100,
2050, END_FADE, 0,
2050, LOOP, 0
END_SEQUENCE
