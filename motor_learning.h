#ifndef MOTOR_LEARNING_H
#define MOTOR_LEARNING_H

#include "CB_model.h"
#include "exporter.h"
#include "CB_model.h"
#include "environment.h"
//#include "BG_model.h"

class MotorLearning
{
    BG_model bg;
    CB_model cb;
    Exporter exporter;
    Environment* env;

    bool learn_cb;
    bool learn_bg;
    float * Rpre;
    float Rpre_coef;

    public:
    const static int na = 100; // number of actions
    static int nc; // number of cues

    public:
    
    float makeTrials(unsigned int numTrials, float * addInfo, bool flushWeights=true, unsigned int indAdd=0, bool doExport=true);
    void inactivateBG();  
    void activateBG();
    void setBGlearning(bool bglearns);
    void setCBlearning(bool cblearns);

    void setRpre(float * rpre);
    void setRpreMax();
    void flushRpre();
    float getRpre(unsigned int trialInd, float reward, float * addInfo);  
    //void BG_model::initWeightNormFactor(unsigned int memoryLen)  // should be called ONLY ONCE

    void setCBtarget(float x, float y);
    void flushCB();

    MotorLearning(Hand* hand, Environment* env, int nc);
    ~MotorLearning();
};

#endif
