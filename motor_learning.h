#ifndef MOTOR_LEARNING_H
#define MOTOR_LEARNING_H

#include "exporter.h"
#include "BG_model.h"
#include "CB_model.h"
#include <vector>
//#include "BG_model.h"


class MotorLearning
{
    Arm arm;              // but if we move arm by arm.moveArm, we would bypass cerebellum
    BG_model bg;
    CB_model cb;

    Exporter *exporter;
    Environment* env;

    bool learn_cb;
    bool learn_bg;
    vector<float> Rpre;
    float Rpre_coef;

    parmap paramsML;

    int na; // number of actions
    int nc; // number of cues

    public:
    
    float makeTrials(unsigned int numTrials, float * addInfo, bool flushWeights=true, unsigned int indAdd=0, bool doExport=true);
    void inactivateBG();  
    void activateBG();
    void setBGlearning(bool bglearns);
    void setCBlearning(bool cblearns);

    // sets paritcular habit, does not flush existing habits
    void setHabit(int cue, int action, float strength);
    void moveArm(float * PMC_activity, float * out, float ffield);  // call CB move arm

    void backupWeights();

    //comes from prelearning)
    //if w12too true, we also restore temporary weights -- strengths of PFC->MSN connections
    //It may be required to have smooth transition between prelearning and main task
    void restoreWeights(bool w12too=false);   // , we set habits to be as in backup (usually 
    void flushWeights(bool wmtoo);   // resets w1 and w2 weights. If wmtoo=true, then wm as well

    void setRpre(float * rpre);
    void setRpreMax();
    void flushRpre();
    float updateRpre(unsigned int trialInd, float reward, float * addInfo);  
    //void BG_model::initWeightNormFactor(unsigned int memoryLen)  // should be called ONLY ONCE

    void trainCB(float x0, float y0, float * yy, float coef, bool flushW=true) ;
    void setCBtarget(float x, float y);
    void flushCB();

    void getReachCenterPos(float &x, float&y);

    MotorLearning(Environment* env, Exporter * exporter_, parmap & paramsEnv);
    MotorLearning();
    ~MotorLearning();

    void init(Environment* env, Exporter* exporter_,parmap & paramsEnv);
};

#endif
