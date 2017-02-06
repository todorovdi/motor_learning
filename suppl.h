#ifndef SUPPL_H
#define SUPPL_H

#include "BG_model.h"
// suppl.h 

void movingAverageFilter(float * input, unsigned int len, unsigned int windowSize, float * output);
inline float weightRpre(unsigned int tau,float expCoefRpre, float normFactor) // number of steps back. Should be positive
{ return exp(-expCoefRpre * tau)*normFactor; }

#endif
