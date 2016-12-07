#ifndef GALEA_H
#define GALEA_H


#include "learn.h"
#include "file_export.h"
#include "suppl.h"
#include "par.h"

///////////////////////////////////////////
//// experiment-specific constants
///////////////////////////////////////////

enum experimentPhases {PRE1=0, PRE2=1, ADAPT1=2, POST1=3,  ADAPT2=4, POST2=5, PRELEARN=6};

extern const int numBasicCues;
extern int experimentPhase;   // 0 means prelearning, 1 means showing RC, 2 means showing NC


#endif // GALEA 
