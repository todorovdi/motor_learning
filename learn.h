#ifndef LEARN_H
#define LEARN_H


#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <fstream>
//#include <random>

using namespace std;


extern float A_exp; // exploratory constant [0, 1]

extern float y_drive;
extern float y_d1, y_d2;
extern float d1_gpi, init_gpi_drive, gpi_y;
extern float gpi_drive; 
extern float d2_gpe, gpe_drive, gpe_gpi;
extern float tau; // time constant for Wilson-Cowan
 
extern float inh1 ; // lateral inhibition in D1
extern float inh2 ; // lateral inhibition in D2
extern float inh21; // inhibition of D1 by D2
extern float inh12; // inhibition of D2 by D1
extern float inhy ; // lateral inhibition in M1  
// inhibition in M1 should be not that big -- it control the depth of potential wells but
// if they are too deep the exploration (which is uniform, not Gaussian) would not be 
// able to jump from one well to another

extern float Q;  // amplitude of random init cond for dyn var

extern float *d1,*d2;
//float gpe[na],gpi[na];
extern float *gpe, *gpi, *stn;

extern float normFactor; // to be initalized in main. Used to weight learning
extern float expCoefRpre;

extern float gam1;
extern float gam2;
 
extern float lam1; // learning rate
extern float lam2; // learning rate

extern float mgam;
extern float mlam;

extern float W1max;
extern float W2max; 
extern float WMmax;

extern float **w1, **w2, **wm;

extern const float EPS; // to compare floats with zero

extern bool BGactive; 
extern float d1_ltp, d2_ltp;


extern float stn_drive; 
extern float gpe_stn;

extern float d1_drive;
extern float d2_drive;

///////////////////////////////////// 
///////////////   Constants to set in your experiment
////////////////////////////////////
//
extern int nc; // number of cues
extern const int na; // number of actions

extern unsigned int nsessions;
extern unsigned int numTrials;
extern float successDist;  // distance the hand has to reach the success to count
extern float finalNoiseAmpl; // amplitude of noise applied to the final hand position
extern float neuron2armMult;
extern float initAng[4];

extern unsigned int memoryLen; // currently not used

extern float Rpre_coef;

///////////////////////////////////////
////////////////// Functions to be called by your experiment
///////////////////////////////////////

float makeTrials(unsigned int numTrials, unsigned int memoryLen, float * addInfo, bool flushWeights=true, unsigned int indAdd=0, bool doExport=true);
// indAdd -- add this number to the index when 1) calcRpre 2) output results to file
void inactivateBG();  
void activateBG();
void activate_disease_PD();
void activate_disease_HD();
void inactivate_disease_PD();
void inactivate_disease_HD();

void flushWeights(bool wmToo);
void flushRpre();
void setRpre(float * rpre);
void setRpreMax();

void cblearn(float xdif, float ydif);
void initCB(float x0, float y0, float dw, float * yy = 0);

/////////////// internal functions of the model

float getRpre(unsigned int trialInd, float reward, float * addInfo);  

void backupWeights();
void restoreWeights(bool w12too=false);
void allocMemory();
void freeMemory();

void initWeightNormFactor(unsigned int memoryLen);  // should be called ONLY ONCE
float gauss();


inline float rnd() { return 1.*rand()/(RAND_MAX+1.); } 
inline float s(float x) { return x>0?tanh(x):0; }
inline float weightRpre(unsigned int tau) // number of steps back. Should be positive
{ return exp(-expCoefRpre * tau)*normFactor; }

///////////////////////////////////////
//  Functions to implement in your experiment
///////////////////////////////////////
int turnOnCues(float * cues);
float getSuccess(float * x,float * y,unsigned int k,float *addInfo);  // precise meaning of success differs from experiment to experiment
float getReward(float success, float * x,float * y, float & param);  
float moveHand(float * phi0, float * y, float* out);  

void runExperiment(int argc, char** argv);

void exportWeights(unsigned int trialNum,float ** w1,float ** w2,float **wm);
void exportDynData(unsigned int trialNum,float *y,float *d1,float *d2,float * gpe,float *gpi,float t,float reward, float * addInfo);
void exportWeightsOnce(float ** w1,float ** w2,float **wm);

void exportInit(const char * prefix=NULL);
void exportClose();


#endif // LEARN_H



//#define MATCH_SLAVA
