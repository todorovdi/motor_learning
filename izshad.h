#include "CB_model.h"
#include "motor_learning.h"
#include "suppl.h"
#include "exporter.h"
#include "environment.h"

class expPhaseParams{
  public:

  string name;
  int numTrials;
  float defTgt;  

  int cue;
  // I would have to compute new target locations in bash script. Or I can set target to be default and just perform listed pertubrbations

  bool action_rotation;          // has not effect, action set by prelearning
  float endpoint_rotation;
  bool endpoint_xreverse;
  float force_field;
  float endpt_xshift;
  float endpt_yshift;
  bool error_clamp;
  bool  learn_bg;
  bool  learn_cb;
  bool cbLRateReset;
  bool resetCBState;
  float cbLRate;

  float target_rotation;
  bool target_xreverse;
  float tgt_xshift;
  float tgt_yshift;

  float errClampDirDeg;

  expPhaseParams()
  {
    name = "";
    numTrials = 0;
    defTgt = 0;
    action_rotation = 0;
    endpoint_rotation = 0;
    endpoint_xreverse = 0;
    force_field = 0.;
    endpt_xshift = 0;
    endpt_yshift = 0;

    target_rotation = 0;
    target_xreverse = 0;
    tgt_xshift = 0;
    tgt_yshift = 0;
    error_clamp = 0;

    cbLRate = -100; // if passed to reset method, changes to init cbLRate

    learn_bg=1;
    learn_cb=1;

    cue = 0;
    cbLRateReset = 1;
    resetCBState = 0;

    errClampDirDeg = 1000;
  }

  void print()
  {
    cout<<"phase "<<name<<" params are"<<endl;
    cout<<numTrials<<endl;
    cout<<defTgt<<endl;
    cout<<action_rotation<<endl;
    cout<<endpoint_rotation<<endl;
  }
};

class phaseParamPrelearn
{
  public:
  int action;
  float wmmax;
  float tempWAmpl;
  vector<float> patPMC;
  float tgt_x;
  float tgt_y;

  phaseParamPrelearn()
  {
    action = -1000;
    wmmax = 0.0;
    tempWAmpl = 0.;
    tgt_x = 1000;
    tgt_y = 1000;
  }
};

class perturbationExperimentEnv: public Environment
{
    //unsigned int numTrialsPrelearn = 1200;
    int numTrialsPrelearn;

    float xcur,ycur;   // percieved endpoint
    float sector_thickness;
    float sector_width;
    float wmmax_fake_prelearn;
    float fake_prelearn_tempWAmpl;
    float armReachRadius;
    int  numPhases;

    bool fake_prelearn;
    bool sector_reward;

    vector<expPhaseParams> phaseParams;

    int learn_cb;                
    int learn_bg;
    float cbLRate;
    int randomCBStateInit;
    float randomCBStateInitAmpl;

    float actRotAngleIncSess;
    float endptRotAngleIncSess;
    float rewardSize;

    unsigned int sess_seed;

    vector<phaseParamPrelearn> cue2prelearnParam;

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

    perturbationExperimentEnv(const parmap & params, int num_sess, unsigned int sess_seed);
    ~perturbationExperimentEnv();

    // experiment-specific
    void initCBdir(float x0, float y0, vector<float> patPMC, bool resetState);
};

void runExperiment(parmap & params);
