#include "motor_learning.h"
#include "environment.h"
#include <cstdlib>

//int MotorLearning::nc=1;

void MotorLearning::activateBG(int a)
{
    bg.activate(a);
    //BGactive = true;
}

void MotorLearning::setSingleRPre(int cue, float rpeval)
{
    Rpre[cue] = rpeval;    
}

void MotorLearning::setRpre(float * rpre)
{
  for(int i=0;i<nc;i++)
  {
    setSingleRPre(i,rpre[i]);
  }
}

void MotorLearning::setRpreSame(float r)
{
    for(int i=0;i<nc;i++)
    {
        Rpre[i] = r;
    }
}

void MotorLearning::flushRpre()
{
    for(int i=0;i<nc;i++)
    {
        Rpre[i] = 0.;
    }
}

float MotorLearning::updateRpre(unsigned int k, float R, float * addInfo)  // to be called AFTER bg_learn
{
    { 
        Rpre[k]=Rpre_coef*Rpre[k]+(1-Rpre_coef)*R;
    }
    
    return Rpre[k];
}

void MotorLearning::setBGlearning(bool bglearns)
{
    learn_bg = bglearns;
}

void MotorLearning::setCBlearning(bool cblearns)
{
    learn_cb = cblearns;
}

void MotorLearning::setCBtarget(float x, float y)
{
    cb.setCBtarget(x,y);
}

void MotorLearning::getCBtarget(float & x, float & y)
{
    x = x_cb_target;
    y = y_cb_target;
}

void MotorLearning::setRandomCBState(float a)
{
    cb.setRandomState(a);
    if (fabs(a) <EPS)
    {
      cout<<"flushing CB state"<<endl;
    }
}

void MotorLearning::resetCBLRate(float lr)
{
  cb.resetLearnRate(lr);
}

float MotorLearning::getCBLRate()
{
  cb.getLearnRate();
}

// calls getSuccess, which calls moveArm
float MotorLearning::makeTrials(unsigned int ntrials, float * addInfo, bool flushData, unsigned int indAdd, bool doExport)
{

    if(flushData)
    {
        bg.flushWeights(true);
        cb.flushTuning();
        flushRpre();
    }

    float x[nc];
    float y[na];

    // cycle over trials
    for(int k=indAdd;k<(ntrials+indAdd);k++)
    {
      if(debug_printTrialNumbers)
      {
        cout<<" DEBUG: "<<k<<" trial started"<<endl;
      }

      bg.resetForTrialBegin();

      int addInfoCue;
      int cueActive = env->turnOnCues(k,x,&addInfoCue);
      bg.setCues(x);
      int feedbackGiven = addInfoCue;   
      // 0 means nos vis feedback, but reward may be given!
      //


      if(learn_bg || !habit2PMCdirectly)
      {


        int nsteps = 0;
        float dt = 0;
        bool CONT_OUT = false;
              // integrate the equations

        if (CONT_OUT) {bg.exportContOpen(k); }

          for(float t=0; t<T; )
          {   
            dt = bg.do_step();
            if (CONT_OUT) {bg.exportContState(t);}
                        nsteps++;
                        t+= dt;
                        if(dt >= bgStepsizeIntStopThr)
              break;
          };
        if (CONT_OUT) {bg.exportContClose();}
      } 
      else if(!learn_bg && habit2PMCdirectly )
      {
        bg.habit2PMCdirectly(cueActive);   // works only for non-composite actions
      }

      bg.getPMC(y);

      //if(exitIfExplore && y[cueActive] < 0.2 ) 
      //{
      //  std::exit(1);
      //}

      float cbTgt_x, cbTgt_y, ang;
      env->getCurTgt(x,cbTgt_x, cbTgt_y,ang);
      //cb.setCBtarget(cbTgt_x,cbTgt_y);
      if(learn_cb && trainCBEveryTrial)
      {
        if(!cbRetrainSpeedup || cb.trainNeeded(y,cbTgt_x,cbTgt_y))
        {
          cb.train(cbTgt_x, cbTgt_y, y,false,retrainCB_useCurW,ffield);  // flushW= false, useCurW = true
        }
        else
        {
          cb.setCBtarget(cbTgt_x,cbTgt_y);
          //cout<<"cbTgt_x "<<cbTgt_x<<" cbTgt_y "<<cbTgt_y<<endl;
        }
      }

      float addInfoItem[7];
      float sc = env->getSuccess(x,y,k,addInfoItem);   // here arm export happens
      //float endpt_percieved_x = addInfoItem[1];
      //float endpt_percieved_y = addInfoItem[2];
      float endpt_x = addInfoItem[3];
      float endpt_y = addInfoItem[4];

      float t; // is may be set in the following function (it passes as a reference argument) -- originally thought to be randomness, determining whether we get reward or not, based on success (Piron setup)
      percept->saveCurErr();

      float R = env->getReward(sc,x,y,t);
      if( k==indAdd && setRpre_firstR )
      {
        setSingleRPre(cueActive,R);
      }

      if(doExport )
      { 
        bg.exportBGstate(k,0);
        cb.CBExport(k);
        exporter->exportTrial(k,x,R,Rpre[cueActive]);
      } 

      //rnd();  // just to follow same seed as Slava's code
      if(learn_bg && feedbackGiven!=2)
      { 
        float RPE = R- Rpre[cueActive];
        if( exploreAlways )
        {
          RPE = -rewardSize;
          //bg.flushWeights(false);
        }
        bg.learn(RPE);
      } 

      //cout<<"trial"<<endl;
      if(learn_cb && feedbackGiven==1)
      { 
        cb.learn();
        cb.predictNextErr(y);  // try to predict next error as if we were using same motor program and no perception perturbation
      }
      else
      {
        exporter->exportCBMisc(cb.getLearnRate(),sc,0,0,0,0,0);
      }
      cb.stateDegradeStep();    // should happen even with cb turned off!
      if(feedbackGiven == 0)
      {
        percept->resetErrHist();
      }

      updateRpre(cueActive,R,0);   
      //std::copy(y, y+na, prevy.begin());

    }
    //if(doExport)
    bg.exportWeightsOnce();
    exporter->exportLasty(y);

    //    if(doExport)
    //        trialEndExport(sumM1freq, 0);

    return 0;
}

void MotorLearning::backupWeights()
{
    bg.backupWeights();
}

void MotorLearning::setHabit(int cue, int action, float strength)
{
    bg.setwm(cue,action,strength);
}

void MotorLearning::fakePrelearnReaching(int cue, int action, float habitAmpl, float tempWAmpl)
{
  setHabit(cue,action,habitAmpl);
  bg.setw1(cue,action,tempWAmpl);
}

float MotorLearning::getHabit(int cue, int action)
{
    bg.getwm(cue,action);
}

void MotorLearning::restoreWeights(bool w12too)
{
    bg.restoreWeights(w12too);
}

MotorLearning::MotorLearning(Environment * env_, Exporter * exporter_, Percept * percept_, parmap & params ) 
{
  ffield = 0;
  init(env_,exporter_,percept_,params);
  x_cb_target = 0;
  y_cb_target = 0;
  modError = false;
  //prevy.resize(na);
  //std::fill(prevy.begin(),prevy.end(),0.);
}

MotorLearning::MotorLearning()
{
  ffield = 0.;
  x_cb_target = 0;
  y_cb_target = 0;
  modError = false;
}

MotorLearning::~MotorLearning()
{
}

void MotorLearning::initParams(parmap & params)
{
    //expCoefRpre = paramsML["expCoefRpre"]; 
    Rpre_coef = stof(params["Rpre_coef"]); 
    T = stof(params["T"]); 

    learn_cb = stoi(params["learn_cb"]);
    learn_bg = stoi(params["learn_bg"]); 

    trainCBEveryTrial = stoi(params["trainCBEveryTrial"]);
    retrainCB_useCurW = stoi(params["retrainCB_useCurW"]);
    textExport=stoi(params["textExport"]);

 
    habit2PMCdirectly=stoi(params["habit2PMCdirectly"]);
    if(habit2PMCdirectly)
      cout<<"habit2PMCdirectly mode is active!"<<endl;
 
    string s = params["cbRetrainSpeedup"];
    cbRetrainSpeedup = s != "" ? stoi(s) : 0; 

    s = params["bgStepsizeIntStopThr"];
    bgStepsizeIntStopThr = s != "" ? stof(s) : 10; 

    s = params["rotateErr"];
    rotateErr = s != "" ? stoi(s) : 0; 

    s = params["xreverseErr"];
    xreverseErr = s != "" ? stoi(s) : 0; 

    s = params["exploreAlways"];
    exploreAlways = s != "" ? stoi(s) : 0; 

    s = params["rewardSize"];
    rewardSize = s != "" ? stof(s) : 3; 

    s = params["debug_printTrialNumbers"];
    debug_printTrialNumbers = s != "" ? stoi(s) : 0; 

    nc=stoi(params["nc"]);
    na=stoi(params["na"]);
}

void MotorLearning::setModError(bool me)
{
  modError = me;
}

void MotorLearning::setRpre_firstR_mode(bool sr)
{
  setRpre_firstR = sr;
}

void MotorLearning::init(Environment* env_, Exporter* exporter_,Percept * percept_,parmap & params)
{
    initParams(params);
    env = env_;

    exporter = exporter_;
    percept = percept_;

    //if( stoi(params["learn_bg"]) == 0 )
    //{
    //  params["updateCBStateDist"] = to_string(10.);
    //}

    arm.init(params);
    cb.init(params,exporter,&arm,percept);
    bg.init(params,exporter);

    Rpre.resize(nc);
}

void MotorLearning::getReachCenterPos(float &x, float&y)
{
    arm.getReachCenterPos(x,y);
}

void MotorLearning::getLasty(float * lasty)
{
  bg.getPMC(lasty);
}

void MotorLearning::moveArm(float * PMC_activity, float * out, float ffield)
{
    cb.moveArm(PMC_activity,out,ffield);
}

void MotorLearning::trainCB(float x0, float y0, float * yy, bool flushW) 
{
  x_cb_target = x0;
  y_cb_target = y0;
  cb.train(x0,y0,yy,flushW,retrainCB_useCurW);
}

void MotorLearning::flushWeights(bool wmtoo)
{
    bg.flushWeights(wmtoo);
}

void MotorLearning::setFfield(float ff)
{
  ffield = ff;
}

void MotorLearning::setCBtCDS(float val)
{
  cb.set_tCDS(val);
}

int MotorLearning::get_cbACHappiness()
{
  return cb.get_ACHappiness(0,0);
}

//void initWeightNormFactor(unsigned int memoryLen)  // should be called ONLY ONCE
//{
//    float nf = 0.;
//    for(unsigned int i = 0; i<memoryLen; i++)
//    {
//        nf += weightRpre(i,expCoefRpre,normFactor); // when we compute it, normFactor should be = 1.
//    }
//    normFactor = 1/nf;
//}
