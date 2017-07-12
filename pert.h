#include "CB_model.h"
#include "motor_learning.h"
#include "suppl.h"
#include "exporter.h"
#include "environment.h"
#include "percept.h"

class cueListItem
{
  public:
  int cue,numShows,feedbackOn;
};

class cueFeedback
{
  public:
  int cue, feedbackOn;
};

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
  bool exitIfExplore;

  // only makes sense for a special parameter called cueList in the ini file
  // done for Izawa-Shadmehr paper to generate generalization graph
  vector<cueListItem> cueList;    
  vector<cueFeedback> cueSeq;    

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
    exitIfExplore=0;
  }

  // standard Fisher-Yates shuffle algorithm
  void shuffle(vector<cueFeedback> & elements)
  {
  ////  rng.seed(0);

    // the counter to be used to generate the random number
  int currentIndexCounter = elements.size();
  auto currentIndex = elements.rbegin();
  for (auto currentIndex = elements.rbegin(); currentIndex != elements.rend() - 1;
         currentIndex++ , --currentIndexCounter)
        {
            int randomIndex = rand() % currentIndexCounter;
            // if its not pointing to the same index      
            //if (*currentIndex != elements.at(randomIndex))
            {
                //then swap the elements
                swap(elements.at(randomIndex), *currentIndex);
            }
        }
  }

  // generate a random seq of cues demonstrations. 
  // first create a stupid seq and then shuffle it
  void genCueSeq()
  {
    int totalNum = 0;
    cueSeq.reserve(numTrials);
    for(int i = 0; i<cueList.size(); i++)
    {
      cueFeedback cf;
      cf.cue        = cueList[i].cue;
      cf.feedbackOn = cueList[i].feedbackOn;
      // insert numShows[i] times cueList[i] at the end
      cueSeq.insert(cueSeq.end(), cueList[i].numShows, cf);  
      totalNum += cueList[i].numShows;
    }
    if (totalNum != numTrials)
    {
      string estr = "Wrong cueList contents, trial numbers don't add upp ";
      cout<<"---------- ERROR: "<<totalNum<<estr<<endl;
      throw estr;
    }
    shuffle(cueSeq);

    ////for debug
    //for(int i=0;i<cueSeq.size();i++)
    //{
    //  cout<<"cue seq "<<i<<" "<<cueSeq[i].cue<<"  "<<cueSeq[i].feedbackOn<<endl;
    //}
  }

  void parseCueList(std::string str)
  {
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("|");
    tokenizer tokens(str, sep);

    //cues.reserve(tokens.size());
    //numShows.reserve(tokens.size());
    //feedbackOn.reserve(tokens.size());
    //cues.resize(0);
    //numShows.resize(0);
    //feedbackOn.resize(0);
    //cueList.reserve(tokens.size());
    cueList.resize(0);

    boost::char_separator<char> sep2(":");
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer2;
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
    {
      tokenizer2 subtokens(*tok_iter, sep2);
      tokenizer2::iterator it=subtokens.begin();
      cueListItem cli;
      cli.cue= stoi(*it); it++;
      cli.numShows= stoi(*it); it++;
      cli.feedbackOn= stoi(*it); 

      cueList.push_back(cli);
    }

  //std::vector<std::string> cueNumList;
  //strtk::parse(str,"|",cueNumList);

  //cues.reserve(cueNumList.size());
  //cues.resize(0);
  //numShows.reserve(cueNumList.size());
  //numShows.resize(0);
  //feedbackOn.reserve(cueNumList.size());
  //feedbackOn.resize(0);

  //std::vector<std::string>::iterator it = cueNumList.begin();
  //for(;it != cueNumList.end(); it++)
  //{
  //  std::vector<int> sublist;
  //  strtk::parse(str,":",sublist);
  //  cue.push_back(sublist[0]);
  //  numShows.push_back(sublist[1]);
  //  feedbackOn.push_back(sublist[2]);
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
