#ifndef FORCE_FIELD_H
#define FORCE_FIELD_H


#include "learn.h"
#include "file_export.h"
#include "suppl.h"
#include "par.h"

///////////////////////////////////////////
//// experiment-specific constants
///////////////////////////////////////////

const float probReward=0.75; // 0.75; // prob that we get a reward
//const float probReward=0.75; // 0.75; // prob that we get a reward
const float firingRateTreshold = 0.5;
const float cueChoiceProb=0.5;    // when nc=1, this has no effect

enum experimentPhases {PRELEARN, BASELINE, WITH_FORCE, WITH_FORCE_CCW, WASHOUT};
enum directions_enum {TOP=0, BOTTOM=1, LEFT=2, RIGHT=3};
enum directions_num_enum {NTOP=25, NBOTTOM=75, NLEFT=50, NRIGHT=0};
enum basic_cues_enum {VERT=0, HOR=1, NE=2, SE=3, SW=4, NW=5};

extern const int numBasicCues;
extern int experimentPhase;   // 0 means prelearning, 1 means showing RC, 2 means showing NC

int getOppositeAction(int cue, int j);
//int getSuccess(float * x,float * y);  // to be defined in the experiment file

#endif // FORCE_FIELD 
