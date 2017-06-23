#include "CB_model.h"
#include "motor_learning.h"
#include "suppl.h"
#include "exporter.h"
#include "environment.h"
#include "percept.h"

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
  float absRwdSz;
  bool error_clamp;
  bool  learn_bg;
  bool  learn_cb;
  bool cbLRateReset;
  bool errHistReset;
  bool resetCBState;
  bool randomTgt;

  bool resetRPre;
  float setRPre;
  float cbLRate;
  float CBtCDS;
  float randomTgtRange;

  float target_rotation;
  bool target_xreverse;
  float tgt_xshift;
  float tgt_yshift;

  bool randomCue;

  // only makes sense for a special parameter called cueList in the ini file
  // done for Izawa-Shadmehr paper to generate generalization graph
  vector<int> cueList,numShows,feedbackOn, cueSeq;    

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

    errHistReset = 0;
    cue = 0;
    cbLRateReset = 0;
    resetCBState = 0;
    setRPre = -2000;
    resetRPre = 0;
  
    CBtCDS = 0.;
    randomCue=false;
 
    randomTgtRange=0.;
  }

  // generate a random seq of cues demonstrations. 
  // first create a stupid seq and then shuffle it
  void genCueSeq()
  {
    int totalNum = 0;
    cueSeq.reserve(numTrials);
    for(int i = 0; i<numShows.size(); i++)
    {
      // insert numShows[i] times cueList[i] at the end
      cueSeq.insert(cueSeq.end(), numShows[i], cueList[i]);  
      totalNum += numShows[i];
    }
    if (totalNum != numTrials)
    {
      string estr = "Wrong cueList contents, trial numbers don't add upp";
      cout<<"---------- ERROR: "<<estr<<endl;
      throw estr;
    }
    shuffle(cueSeq);

    //for debug
    //for(int i=0;i<cueSeq.size();i++)
    //{
    //  cout<<"cue seq "<<i<<" "<<cueSeq[i]<<endl;
    //}
  }

  void print()
  {
    cout<<"phase "<<name<<" params are"<<endl;
    //cout<<numTrials<<endl;
    //cout<<defTgt<<endl;
    //cout<<action_rotation<<endl;
    cout<<endpoint_rotation<<endl;
  }
};

class phaseParamPrelearn
{
  public:
  int action;
  float ang;
  float wmmax;
  float tempWAmpl;
  vector<float> patPMC;
  float tgt_x;
  float tgt_y;

  phaseParamPrelearn()
  {
    action = -1000;
    ang = -1000;
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
    float wmmaxFP;
    float w1maxFP;
    float armReachRadius;
    int  numPhases;

    bool fake_prelearn;
    bool sector_reward;
    bool absRewardOn;
    bool rwdNormalizeBycbLRate;
    bool rwdFromcbLRate;
 
    float cbLRateUpdAbsErr_threshold;

    vector<expPhaseParams> phaseParams;

    int learn_cb;                
    int learn_bg;
    float cbLRate;
    int randomCBStateInit;
    float randomCBStateInitAmpl;

    float actRotAngleIncSess;
    float endptRotAngleIncSess;
    float rewardSize;
    float perfRewardSize;

    float minActionAngDeg;
    float maxActionAngDeg;
 
    float rwdGradePower;
    float perfGradePower;
    float perfRwdMult;
    float perfRwdErrChange_threshold;
    float acUpdCoefThr;
    float rwdFromcbLRate_thr;
    //float rwdFromcbLRate_mult;
    float rwdFromcbLRate_add;

    int rwdErrDepth;

    unsigned int sess_seed;
 
    int gradedReward;
    int perfBasedReward;
    bool perfFromAC;
    bool vectorErrTgtBorder;

    vector<phaseParamPrelearn> cue2prelearnParam;

    public:
    int turnOnCues(int trialNum, float * cues, int * addInfo = 0);
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
