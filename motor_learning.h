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

    // do we do cerebellum and bg learning 
    bool learn_cb;                
    bool learn_bg;
    bool retrainCB_useCurW;
    vector<float> Rpre;
    float Rpre_coef;              // how much of current reward we use for the new reward prediction error

    float T;                      // integration duration max limit
    float rewardSize;
    float rotateErr;
    bool xreverseErr;
    bool modError;
    bool habit2PMCdirectly;

    bool error_clamp_mode;
    //vector<float> prevy;

    //parmap & params;
    float x_cb_target,y_cb_target;

    int na; // number of actions
    int nc; // number of cues
    
    int textExport;
    int trainCBEveryTrial;

    float ffield;

    public:
    
    /////////////////////////////////////
    // main methods
    /////////////////////////////////////
    float makeTrials(unsigned int numTrials, float * addInfo, bool flushWeights=true, unsigned int indAdd=0, bool doExport=true);
    void moveArm(float * PMC_activity, float * out, float ffield);  // call CB move arm
    float updateRpre(unsigned int trialInd, float reward, float * addInfo);  

    // turns off and on BG output (but computation is done anyways)
    void activateBG(int a);

    /////////////////////////////////////
    // control methods 
    /////////////////////////////////////
    
    // it is here because we don't have direct access from Environment to the cerebellum
    void trainCB(float x0, float y0, float * yy, bool flushW=true) ; // CB_model method description
    void resetCBerr();
    void resetCBLRate(float lr=-100);
    void setCBtCDS(float val);
    
    // turns off and on BG nd cerebellum learning, respectively
    void setBGlearning(bool bglearns);
    void setCBlearning(bool cblearns);
    void setFfield(float ff);
    // whether we use distorted error for cb learning
    void setModError(bool me);
    void setErrorClamp(bool ec);

    // sets paritcular habit, does not flush existing habits
    void setHabit(int cue, int action, float strength);
    float getHabit(int cue, int action);
    void getLasty(float * lasty);
    void fakePrelearnReaching(int cue, int action, float habitAmpl, float tempWAmpl);

    // backs up weights (see BG_model methods for more details)
    void backupWeights();

    //comes from prelearning)
    //if w12too true, we also restore temporary weights -- strengths of PFC->MSN connections
    //It may be required to have smooth transition between prelearning and main task
    // see also BG_model method description for details
    void restoreWeights(bool w12too=false);   // , we set habits to be as in backup (usually 
    void flushWeights(bool wmtoo);   // resets w1 and w2 weights. If wmtoo=true, then wm as well

    // sometimes we need to set Rpre forcefully (in the beginning of the experiment 
    // where we did fake prelearning (direct cue-action association 
    // without real prelearning trials), for example)
    void setRpre(float * rpre);
    void setRpreMax();
    void setSingleRPre(int cue, float rpeval);
    void flushRpre();

    void setCBtarget(float x, float y);
    void getCBtarget(float & x, float & y);
    void flushCB();  // CB_model method description 
    void setRandomCBState(float amplRand);  // CB_model method description 

    void getReachCenterPos(float &x, float&y);

    ////////////////////////////////
    // initialization
    ////////////////////////////////

    MotorLearning(Environment* env, Exporter * exporter_, parmap & params);
    MotorLearning();
    ~MotorLearning();

    void init(Environment* env, Exporter* exporter_,parmap & params);
    void initParams(parmap & params);
};

#endif
