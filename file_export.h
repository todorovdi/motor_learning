#ifndef FILE_EXPORT_H
#define FILE_EXPORT_H

#include "learn.h"

extern ofstream foutPerTrial;
extern ofstream foutVarDyn;
extern ofstream foutWeights;

void exportDynDataStd(unsigned int trialNum,float *y,float *d1,float *d2,float * gpe,float *gpi,float t,float reward, float sr);
void exportWeightsStd(unsigned int trialNum,float ** w1,float ** w2,float **wm);
void exportCuesState(unsigned int trialNum, float * cues);
void trialEndExport(float * sumM1freq, unsigned int seed);

#endif // FILE_EXPORT_H
