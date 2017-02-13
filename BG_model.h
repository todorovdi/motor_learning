#ifndef BG_MODEL_H
#define BG_MODEL_H


#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <map>
#include <string>
//#include <random>

using std::string;
using std::map;
typedef map<string,string> parmap;

using namespace std;

// forward class declarations. Definitions in separate files
class CB_model;
class Exporter;
class Environment;
class Hand;
class MotorLearning;

// Basal ganglia
class BG_model   
{
    private:
    float A_exp; // exploratory constant [0, 1]

    float y_drive;
    float y_d1, y_d2;
    float d1_gpi, init_gpi_drive, gpi_y;
    float gpi_drive; 
    float d2_gpe, gpe_drive, gpe_gpi;
    float tau; // time constant for Wilson-Cowan

    float inh1 ; // lateral inhibition in D1
    float inh2 ; // lateral inhibition in D2
    float inh21; // inhibition of D1 by D2
    float inh12; // inhibition of D2 by D1
    float inhy ; // lateral inhibition in M1  
    //bition in M1 should be not that big -- it control the depth of potential wells but
    //hey are too deep the exploration (which is uniform, not Gaussian) would not be 
    // to jump from one well to another

    float Q;  // amplitude of random init cond for dyn var

    float normFactor; // to be initalized in main. Used to weight learning
    float expCoefRpre;

    float gam1;
    float gam2;

    float lam1; // learning rate
    float lam2; // learning rate

    float mgam;
    float mlam;

    float W1max;
    float W2max; 
    float WMmax;

    float *d1,*d2;
    float *gpe, *gpi, *stn;
    float **w1, **w2, **wm;
    float *expl;
    float *y,*x;   // actions, cues

    float stn_drive; 
    float gpe_stn;
    float stn_gpi;

    float d1_drive;
    float d2_drive;

    float d1_ltp, d2_ltp, dm_ltp;

    /////////// system vars
    float amplInitRand;
    float **wmBackup, **w2Backup, **w1Backup;
    float old_A_exp;
    float old_gpi_drive;
    bool d2activity;
    bool BGactive; 

    unsigned int memoryLen;


    int na;
    int nc;

    Exporter * exporter;
///////////////////////////////////////
////////////////// Functions to be called by your experiment
///////////////////////////////////////

    public:
        void learn(float DA);    // calls bg_learn
        float do_step();         // calls bg_step

        // controls whether gpi output is nonzero
        void inactivate();  
        void activate();

        void activate_disease_PD();
        void activate_disease_HD();
        void inactivate_disease_PD();
        void inactivate_disease_HD();

        // set weights to default values. 
        // sometimes you have to do it explicitly depending on the protocol of the experiment
        void flushWeights(bool wmToo);

        // used mainly for prelearning -- you backup weights after prelearning 
        // (you may be interested in backing up only habitual weights or w1,w2 weights as well)
        // this method backs up everything but then you can choose what to restore with the next 
        // method
        void backupWeights();
        void restoreWeights(bool w12too=false);

        // export via exporter pointer
        void exportBGstate(int k, float* addInfo);
        void exportCuesState(int k);
        void exportWeightsOnce();

        void resetForTrialBegin();  // do random resetting, usually in the trial beginning 
        void setCues(float * x_); 
        void getPMC(float * y_);  // get current PMC neurons activities

        void setwm(int cue, int action, float val);

        // sets to max firing all PMC that have habit associations
        // used to speed up computation when basal ganglia activity is not interesting
        // (when testing cerebellum separately)
        void habit2PMCdirectly(int cueActive);

        void init(parmap & params,Exporter *exporter);
        BG_model();
        BG_model(Exporter * exporter_);
        ~BG_model();

/////////////// internal functions of the model
    private:
        void bg_learn(float **w1,float **w2,float* x,float* y,float DA,float **wm);
        float bg_step(float **w1,float **w2,float **wm,float *x,float *y,float *expl);


        void copyWeights(float** wfrom, float ** wto);
        void allocMemory();
        void freeMemory();

        // currently not used
        //void initWeightNormFactor(unsigned int memoryLen);  // should be called ONLY ONCE
};




///////////////////////////////////////
////////////////// supplementary stuff
///////////////////////////////////////

extern const float EPS; // to compare floats with zero

inline bool fzero(float t){return fabs(t) < EPS;}
inline float s(float x) { return x>0?tanh(x):0; }
float gauss();

#endif // BG_MODEL_H
