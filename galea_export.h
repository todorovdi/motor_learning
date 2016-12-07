#ifndef GALEA_EXPORT_H
#define GALEA_EXPORT_H

#include "learn.h"
#include "file_export.h"
#include "galea.h"

extern ofstream foutVarDyn2;
extern ofstream foutWeights2;
extern ofstream foutWeightsOnce;
extern ofstream foutArm;
extern ofstream foutErr;


//void exportDynData(unsigned int trialNum,float *y,float *d1,float *d2,float * gpe,float *gpi,float t,float reward, float sr);
//void exportWeights(unsigned int trialNum,float ** w1,float ** w2,float **wm);
void exportWeightsOnce(float ** w1,float ** w2,float **wm);
void exportArm(unsigned int trialNum,float xcur,float ycur, float x0, float y0, float xc, float yc, float* addInfo);

#endif //
