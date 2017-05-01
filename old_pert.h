#include "CB_model.h"
#include "motor_learning.h"
#include "suppl.h"
#include "exporter.h"
#include "environment.h"

class perturbationExperimentEnv: public Environment
{
    float targetPre1; // in degrees
    float targetPre2;  // in degrees
    float dirShift;  // = in degrees

    int numTrialsPre;
    int numTrialsAdapt;
    int numTrialsAdapt2;
    int numTrialsPost;
    //unsigned int numTrialsPrelearn = 1200;
    int numTrialsPrelearn;

    float xcur,ycur;   // percieved endpoint
    float sector_thickness;
    float sector_width;
    float wmmaxFP;
    float w2maxFP;
    float armReachRadius;
    int  numPhases;

    bool fake_prelearn;
    bool sector_reward;

    bool action_change1;
    bool endpoint_rotation1;
    bool target_rotation1;
    bool target_xreverse1;
    float endpt_xshift1;
    float endpt_yshift1;
    float tgt_xshift1;
    float tgt_yshift1;
    bool endpoint_xreverse1;
    float force_field1;

    bool action_change2;
    bool endpoint_rotation2;
    bool target_rotation2;
    bool cue_change1;
    bool cue_change2;

    int learn_cb;                
    int learn_bg;
    float cbLRate;
    int randomCBStateInit;
    float randomCBStateInitAmpl;

    int dirShiftIncSess;
    unsigned int sess_seed;

    vector<float> prelearn_PMC_0;
    vector<float> prelearn_PMC_1;

    public:
    int turnOnCues(int trialNum, float * cues);
    float getSuccess(float * x,float * y,unsigned int k,float *addInfo);
    float getReward(float success, float * x,float * y, float & param);  
    void setParams(int argc, char** argv){}
    void prelearn(int nTrials, float * addInfo);
    void runSession();

    //void setParams();
    int deg2action(float degAngle);
    void getCurTgt(float * x, float & x0, float & y0, float & tgtAngleDeg);

    perturbationExperimentEnv(parmap & params, int num_sess, unsigned int sess_seed);
    ~perturbationExperimentEnv();

    // experiment-specific
    void initCBdir(float x0, float y0, vector<float> patPMC, bool resetState);
};

void runExperiment(parmap & params);

enum experimentPhases {PRE1=0, PRE2=1, ADAPT1=2, POST1=3,  ADAPT2=4, POST2=5, PRELEARN_0=6, PRELEARN_1=7};
