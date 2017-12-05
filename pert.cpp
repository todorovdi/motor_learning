#include "pert.h"

#ifdef PARALLEL
#include <omp.h>
#endif

#define ACTION_CHANGE_SECOND

void runExperiment(parmap & params)
{
  int nsessions = 1; 
  string nsstr = params["nsessions"];
  if(nsstr != "")
    nsessions = stoi(nsstr); 

    unsigned int seed =   time(NULL);  
    bool presetSeed = false; 

    string s = params["seed"];
    unsigned int t_seed = s != "" ? stoul(s) : 0;
    if(t_seed != 0)
    {
        seed = t_seed;
        presetSeed = true;
    }
    srand(seed);

    params["seed"] = to_string(seed);

    cout<<"seed is "<<seed<<endl;
    if(presetSeed)
        cout<<"WARNING: PRESET SEED IS ACTIVE!!!!"<<endl;

    cout<<"nsessions is "<<nsessions<<endl;

    vector<unsigned int> seeds(nsessions);
    vector<parmap> paramsThreadSeparate(nsessions);   // so that params modifications made in one thread do not affect othere parallel running sessions
    if(nsessions > 1)
    { 
        for(int i = 0; i<nsessions; i++)
        { 
            seeds[i] = rand();
        }
    } 
    else
    {
        seeds[0] = seed;
    }
    for(int i = 0; i<nsessions; i++)
      paramsThreadSeparate[i] = params;


    // parallel works only if compiled with a special flag. At least with gcc
#pragma omp parallel for
    for(int i = 0; i<nsessions; i++)
    { 
#ifdef RND_BOOST    // flag set in suppl.h
        rng.seed(seeds[i]);
#endif
        srand(seeds[i]);   // it happens in parallel threads, I hope
        //cout<<"seed number "<<i<<" is "<<seeds[i]<<endl;
        //cout<<"sess num "<<i<<" rnd is "<<rnd()<<endl;
        perturbationExperimentEnv te(paramsThreadSeparate[i],i,seeds[i]);
        te.runSession();
    }
}


// should output everything to files
void perturbationExperimentEnv::runSession()
{
  //cout<<"runSession thread Num "<<omp_get_thread_num()<<endl;
    ml.flushWeights(true); 
//    if(prelearnEachTime)
    { 
        float * addInfoTemp = new float[numTrialsPrelearn];
        prelearn(0, addInfoTemp); // first arg is unused now
        delete[] addInfoTemp;
    }

        string prefix = params["datPrefix"] + string("_seed_") + to_string(sess_seed);
        params["dat_basename"] = prefix;

        exporter.exportInit(prefix,"","");
        exporter.exportParams(params);

        // if we did prelearn above
        ml.restoreWeights(true);  // false == not restoring w1,w2
        //ml.flushWeights(false); 
        //ml.flushRpre(); // flush all data except wm (we set them from prelearn)
        // it may be overriden for each particular phase below (only important for the very first phase)
        ml.setRpreSame(phaseParams[0].absRwdSz);   

        float cues[nc];
        float x0,y0,angle;
        int offset = 0;

        for(int pc = 0; pc<numPhases; pc++)
        {
          experimentPhase = pc;
          expPhaseParams & p = phaseParams[pc];
          ml.setBGlearning(bool(p.learn_bg) && learn_bg);
          ml.setCBlearning(bool(p.learn_cb) && learn_cb);
          if(p.learn_cb && learn_cb)
          {
            if(p.cbLRateReset)
            { 
              ml.resetCBLRate(p.cbLRate);
            }
            if(p.errHistReset)
            {
              percept.resetErrHist();
            }
            //cout<<"4"<<endl;

            int cueActive = turnOnCues(0,cues);  // it not very good to do like that, since we have random
            // activation of cues sometimes in turnOnCues
            getCurTgt(cues,x0,y0,angle);
            initCBdir(x0,y0,cue2prelearnParam[cueActive].patPMC,
                p.resetCBState); 
          }
          if(p.resetCBState)
          { 
            ml.setRandomCBState(0.);
          }
          percept.setErrorClamp(p.error_clamp);

          if(p.setRPre > -1000.)
          {
            ml.setSingleRPre(p.cue,p.setRPre);
          }

          ml.setRpre_firstR_mode(p.resetRPre);

          //if(p.setRPre > EPS)
          //{
          //  ml.setSingleRPre(p.cue,0.);
          //}
          
          ml.setCBtCDS(p.CBtCDS); 

          cout<<"session num = "<<num_sess<<"  experimentPhase is "<<p.name<<endl;
          ml.makeTrials(p.numTrials,0,false,offset);
          offset += p.numTrials;
        }

          //if(randomCBStateInit)
          //{
          //  //ml.setRandomCBState(randomCBStateInitAmpl);
          //}

        //if(stoi(params["trainWith_force_field1"]))
        //  ml.setFfield(stof(params["force_field1"]));
        //if(fabs(rotateErr) > EPS || xreverseErr)
        //  ml.setModError(true);
        //

        exporter.exportClose();
}

void perturbationExperimentEnv::prelearn(int n, float * addInfo)
{
    ml.flushWeights(true);
    experimentPhase = -1;
    float wmmax=0;
    int wmmax_ind=-1;

    if(fake_prelearn)
    { 
      for(int i =0; i<nc; i++)
      {
        phaseParamPrelearn & p = cue2prelearnParam[i];
        ml.fakePrelearnReaching(i,p.action,p.wmmax,p.tempWAmpl);
        fill(p.patPMC.begin(),p.patPMC.end(), 0.);
        p.patPMC[p.action] = p.wmmax;
        //cout<<"cue "<<i<<" ang "<<p.ang<<" prlearn action  "<<p.action<<endl;

        wmmax = p.wmmax;
        // or deg2action
      }
      cout<<"Fake prelearn max weight is "<<wmmax<<endl;
    }
    else
    { 
        cout<<"num prelearn trials "<<numTrialsPrelearn<<endl;
        cout<<"experimentPhase is "<<" TRUE PRELEARN "<<endl;

        // TODO: careful here!
        ml.setBGlearning(true);
        ml.setCBlearning(false);

        string prefix = string("prelearn_") + params["datPrefix"] + string("_seed_") + to_string(sess_seed);
        //  + string("_numSess_")+std::to_string(num_sess);

        exporter.exportInit(prefix,"","",true);

        ml.flushWeights(true); 
        ml.flushRpre(); // flush all data except wm (we set them from prelearn)
        for(int i =0; i<nc; i++)
        {
          cout<<"prelearning cue "<<i<<endl;
          experimentPhase = -1 - i;

          ml.resetCBLRate(cbLRate);
          phaseParamPrelearn & p = cue2prelearnParam[i];

          ml.setRandomCBState(0.);
          ml.makeTrials(numTrialsPrelearn,addInfo,false,0,false);  
          ml.getLasty(&p.patPMC[0]);

          for(int j = 0; j<na; j++)
          {        
              float wmcur = ml.getHabit(i,j);
              if(wmcur > wmmax)
              { 
                  wmmax = wmcur;
                  wmmax_ind = j;
              } 
          }
          params["wmmaxFP"] = to_string(wmmax);

          //cout<<"True prelearn max weight is (cue "<<i
          //<<") "<<wmmax0<<" for action "<<wmmax_ind0<<endl;
        }

        exporter.exportClose();

        ml.initParams(params);  // restore bg and cb learning params from ini file
        //

    }
    ml.backupWeights();
}

void perturbationExperimentEnv::initCBdir(float x0,float y0, vector<float> patPMC, bool resetState)
{
    //float xc,yc;
    //ml.getReachCenterPos(xc,yc);

    //float radAngle = 2*M_PI*dir/360;
    //float x0=xc+armReachRadius*cos(radAngle);
    //float y0=yc+armReachRadius*sin(radAngle);

    //cout<<"train CB for pt= "<<x0<<"  "<<y0<<endl;

    ml.trainCB(x0,y0,&patPMC[0],resetState);
}

int perturbationExperimentEnv::deg2action(float degAngle)
{
    return int( (float(degAngle) - minActionAngDeg ) / (maxActionAngDeg - minActionAngDeg) * float(na) );
}

int perturbationExperimentEnv::turnOnCues(int k, float * cues, int * addInfo)
{ 
    int cueInd=-100;     
    for(int i=0; i<nc; i++)
        cues[i] = 0.;

    if(experimentPhase >= 0)
    { 
      auto p = phaseParams[experimentPhase];
      if(p.cueSeq.size() == 0)
      { 
        if(p.randomCue)
        {
          cueInd = rand() % nc;
        }
        else
        {
          cueInd = p.cue;
        }
        if(addInfo)
          addInfo[0] = 1; // give full feedback (corresponding to current feedback regime)
      }
      else
      {
        int ind = k % p.cueSeq.size();
        cueInd = p.cueSeq[ind].cue;
        if(addInfo)
        {
          addInfo[0] = p.cueSeq[ind].feedbackOn; // give or not feedback
        }
      }
    }
    else // we do true prelearn here
    {
      cueInd =  - experimentPhase -1;
    }

    cues[cueInd] = 1.;
    return cueInd;
}

void perturbationExperimentEnv::getCurTgt(float * x, float & x0, float & y0, float & tgtAngleDeg)
{
  if(experimentPhase < 0)  // prelearn
  {
    int cue = -experimentPhase -1;
    phaseParamPrelearn & c2p = cue2prelearnParam[cue];
    x0 = c2p.tgt_x;
    y0 = c2p.tgt_y;
    return;
  }

  expPhaseParams & p = phaseParams[experimentPhase];
  float target = angDegAdd(p.defTgt,p.target_rotation);
  if(p.target_xreverse)
    target = 180. - target;

  //cout<<" target is "<<target<<endl;

  float tgt_xshift = p.tgt_xshift;
  float tgt_yshift = p.tgt_yshift;

  float xc,yc;
  ml.getReachCenterPos(xc,yc);

  //float A=.2;
  tgtAngleDeg = target;   

  if(p.randomTgt)
  {
    tgtAngleDeg += p.randomTgtRange * (2.* rnd() - 1.);
  }

  // if we have random cue mode active, we assume that we reach to the target set by random habit
  // not that this function is called after turnOnCues
  if(p.randomCue)
  {
    tgtAngleDeg = cue2prelearnParam[getActiveCueInd(x,nc)].ang;
    //cout<<"cue N "<<getActiveCueInd(x,nc)<<endl;
  }

  float tgtAngleRad = M_PI*tgtAngleDeg/180.;   // in radians

  x0=xc+armReachRadius*cos(tgtAngleRad) + tgt_xshift;
  y0=yc+armReachRadius*sin(tgtAngleRad) + tgt_yshift;

  //cout<<"tgt angle deg "<<tgtAngleDeg<<endl;
  //cout<<" xc,yc is "<<xc<<" "<<yc<<endl;
  //cout<<" x0,y0 is "<<x0<<" "<<y0<<endl;
}

float perturbationExperimentEnv::getSuccess(float * x,float * y,unsigned int k,float *addInfo)
{
  float rot = 0;
  float sign = 1;
  float ffield = 0.;
  float endpt_xshift = 0;
  float endpt_yshift = 0;

  if(experimentPhase > 0)   // if not prelearn
  { 
    expPhaseParams & p = phaseParams[experimentPhase];
    rot = p.endpoint_rotation;
    sign = p.endpoint_xreverse ? -1. : 1.;
    ffield = p.force_field;
    endpt_xshift = p.endpt_xshift;
    endpt_yshift = p.endpt_yshift;
  }

  if(fabs(rot) > 1000.)
    cout<<"-------------- too large rotation ------------ getSuccess"<<endl;

  bool doExport = true;
//        case PRELEARN_0:
//        case PRELEARN_1:
//            doExport = false;
//            break;

  float xc,yc;
  ml.getReachCenterPos(xc,yc);

  float x0,y0,tgtAngleDeg;
  getCurTgt(x,x0,y0,tgtAngleDeg);

  percept.setTgt(x0,y0);

  float out[2];
  ml.moveArm(y,out,ffield);
  float xcur_real, ycur_real;
  xcur_real = out[0], ycur_real = out[1];

  //if(experimentPhase==1)
  //  cout<<"hand coords "<<xcur_real<<" "<<ycur_real<<endl;

  // save "table" point coordinates
  addInfo[3] = xcur_real;
  addInfo[4] = ycur_real;
  xcur = xcur_real;
  ycur = ycur_real;

  float xtmp = xcur_real - xc, ytmp = ycur_real -yc;
  float angle = M_PI/180.*rot;
  xcur = xtmp*cos(angle) - ytmp*sin(angle) + xc;
  ycur = xtmp*sin(angle) + ytmp*cos(angle) + yc;

  xcur = sign*xcur;
  xcur += endpt_xshift;
  ycur += endpt_yshift;
  float sc = rewardDist+0.01;  // to be unrewarded by default
  if(sector_reward)
  { 
      float xd = (xcur-xc);  
      float yd = (ycur-yc);  
      float angleCur0 = atan( yd/xd) / (M_PI) * 180;
      if(xd<0)
          angleCur0 = atan(-xd/yd)/ (M_PI) * 180  + (yd>0?90:-90) ;
      //float angleCur = angleCur1 < 180 ? angleCur1 : angleCur0 - 180;
      float angleCur = angleCur0 > 0 ? angleCur0 : angleCur0+360.;
      float dif = angleCur - tgtAngleDeg;
      float dif1 = (dif < 180. ? dif : dif-360);

      float dist0=hypot(xd,yd); 
      if( fabs(dist0 - armReachRadius)  < sector_thickness )
          sc = dif1;
      else
          sc = fabs(dif1) > sector_width+0.1 ? dif1 : sector_width+0.1;
      addInfo[0] = -dif1;
  } 
  else
  { 
      float dist0=hypot(xcur-x0,ycur-y0);
      sc = dist0;
      //addInfo[0] = -(xcur-x0);
      addInfo[0] = dist0;
  } 
  addInfo[1] = xcur;
  addInfo[2] = ycur;

  percept.setEndpt(xcur,ycur);   // this is endpoint including all perception perturbations
  //if(experimentPhase==1)
  //  cout<<"perceived endpt "<<xcur<<" "<<ycur<<"  error "<<addInfo[0]<<endl;

  float xcbt,ycbt;
  ml.getCBtarget(xcbt,ycbt); // pass params as references
  addInfo[5] = xcbt;
  addInfo[6] = ycbt;

  if(doExport)
    exporter.exportArm(k,xcur,ycur,x0,y0,xc,yc,addInfo);  
  return sc;
}

float perturbationExperimentEnv::getReward(int k, float curErr, float * x,float * y, float & param)
{
    expPhaseParams & p = phaseParams[experimentPhase];

    float absR = 0;
    float relR = 0;

    if(absRewardOn)
    { 
      if(!sector_reward)
      { 
        if(gradedReward)
        {
          absR = p.absRwdSz * 
            ( -  pow(curErr, rwdGradePower) );
        }
        else if( fabs(curErr) < rewardDist) 
        {
          absR = p.absRwdSz;
        }
      }
      else
      {
        if( fabs(curErr) < sector_width/2) 
        {
            absR = p.absRwdSz;
        }
        //else if(ratioBasedReward)
        //{
        //  R = ratio2rwdCoef * p.absRwdSz * (ml.getErrRatio() - 1.) ;
        //  // prev / cur
        //}
      } 
    }
    //R = fmax(0.,  R);
    //R = fmin(p.absRwdSz,  R);
    
    
    float errToCompare = percept.getErr(rwdErrDepth,false);      // if size of hist is less then cbErrDepth, the oldest possible error is returned // here we really want distance to the center                           

//float lastErr =  percept.getErr(1,false);   // here we really want distance to the center
//
    bool b = curErr > cbMotVarEst;
    if(cbMotVarEst)
    { 
      b = b || errToCompare > cbMotVarEst;
    }

    float errReduction = errToCompare - curErr;

    bool bb;
    if(p.cueSeq.size() == 0)
      bb = true;
    else
    {
      int ind = k % p.cueSeq.size();
      bb = p.cueSeq[ind].feedbackOn == 1;
    }

    // if visual feedback is off -- no reward from performance
    if(rwdFromcbLRate && p.learn_cb && bb)
    {
      float lambda = ml.getCBLRate(); 
      relR = perfRewardSize*lambda + rwdFromcbLRate_add;
      //if( lambda > rwdFromcbLRate_thr) 
      //{
      //  relR = perfRewardSize;
      //}
      //else
      //{
      //  // assuming we have only positive rewards (not Slava-like rwd grading)
      //  // then we'd have positive rwd prediction from the baseline
      //  if(absRewardOn) 
      //  { 
      //    relR = 0;
      //  }
      //  else
      //  {
      //    relR = -perfRewardSize;
      //  }
      //}
    }
    else if(perfBasedReward &&  percept.getHistSz() >= 1 && fabs(errReduction) > perfRwdErrChange_threshold && b)
    {
      float sign = errReduction > 0. ? 1 : -1;

      relR = perfRewardSize * sign* pow(sign*errReduction,perfGradePower) * perfRwdMult; 

      if(rwdNormalizeBycbLRate)
      { 
        float errSqReduction = 
          (errToCompare*errToCompare - curErr*curErr) / ml.getCBLRate();
        sign = errSqReduction > 0. ? 1 : -1;

        relR = perfRewardSize * sign* pow(sign*errSqReduction,perfGradePower) *  perfRwdMult; 

      //  cout<<" toComp "<<errToCompare<<" curErr "<<curErr
      //    <<" errSqReduction "<< errSqReduction
      //    <<" rate "<<ml.getCBLRate()<<" relR "<<relR<<endl;
      }

      //relR = sign * p.absRwdSz * perfRwdMult;
      //relR = errReduction/errToCompare / ml.getCBLRate()  * p.absRwdSz * perfRwdMult;
      //relR = sign * fmin (sign * relR, perfRewardSize);
    }

    if(perfFromAC )
    {
      relR= 0.;
      if(percept.getHistSz() > 1 )
      { 
        float real, expected;
        int acAns = ml.get_cbACHappiness();
        
        float f = errReduction > 0 ? pow(errReduction,perfGradePower) : 0.;
        if(acAns==0)       // means succesful correction. Then correct more!
        {
          relR = perfRewardSize * f * perfRwdMult;
        }
        else if(acAns==2)  // if we reduced error not due to CB
        {
          relR = perfRewardSize/4. * f * perfRwdMult;
        }
        else
        {
          relR = perfRewardSize/2. * f * perfRwdMult;
        }
      }
    }


    float R = 0.;
    if(p.error_clamp)
    {
      //R = p.absRwdSz;
      absR = p.absRwdSz; 
    }
    //else
    //{
    //  R = absR + relR; 
    //}
    
    R = absR + relR; 

    return R;
}  

//    string paramsEnvFile, int num_sess_,float tgt,int learn_cb_,float cbLRate_,unsigned int seed
perturbationExperimentEnv::perturbationExperimentEnv(parmap & params_,int num_sess_,unsigned int sess_seed_):Environment(params_,num_sess_)
{
    sess_seed = sess_seed_;
    params["sess_seed"] = to_string(sess_seed);

    numPhases         = stoi(params["numPhases"]);
    fake_prelearn     = stoi(params["fake_prelearn"]);

    string s;
    s = params["learn_cb"];
    learn_cb = s!="" ? stoi(s) : 1;

    s = params["learn_bg"];
    learn_bg = s!="" ? stoi(s) : 1;
        
    ml.init(this,&exporter,&percept,params);  // we did it once in the Environemnt constructor but we might have changed learn_cb so do it again

    string key;
    parmap::iterator iter;

    s = params["minActionAngDeg"];
    minActionAngDeg = s!="" ? stof(s) : 0;

    s = params["maxActionAngDeg"];
    maxActionAngDeg = s!="" ? stof(s) : 360;

    key = string("actRotAngleIncSess");
    iter = params.find(key);
    if(iter!=params.end())
    {
      actRotAngleIncSess = stof(iter->second);   
    }
    else
    {
      actRotAngleIncSess = 0.;
    }
    key = string("endptRotAngleIncSess");
    iter = params.find(key);
    if(iter!=params.end())
    {
      endptRotAngleIncSess = stof(iter->second);   
    }
    else
    {
      endptRotAngleIncSess = 0.;
    }

    s = params["absRewardOn"];
    absRewardOn = s!="" ? stoi(s) : 1;

    s = params["gradedReward"];
    gradedReward = s!="" ? stoi(s) : 0;

    s = params["rwdGradePower"];
    rwdGradePower = s!="" ? stof(s) : 2.;

    s = params["perfRwdErrChange_threshold"];
    perfRwdErrChange_threshold = s!="" ? stof(s) : 0;

    s = params["perfBasedReward"];
    perfBasedReward = s!="" ? stoi(s) : 0;

    s = params["perfGradePower"];
    perfGradePower = s!="" ? stof(s) : 1.;

    s = params["perfRwdMult"];
    perfRwdMult = s!="" ? stof(s) : 1.;

    s = params["rwdErrDepth"];
    rwdErrDepth = s!="" ? stoi(s) : 1;

    s = params["defTgt_all"];
    float defTgt_all = s!="" ? stof(s) : 0.;
 
    s = params["perfRewardSize"];
    perfRewardSize = s!="" ? stof(s) : 3.;

    s = params["perfFromAC"];
    perfFromAC = s!="" ? stof(s) : 0.;

    s = params["sector_reward"];
    sector_reward = s!="" ? stoi(s) : 0;

    s = params["sector_thickness"];
    sector_thickness = s!="" ? stof(s) : 0.03;

    s = params["sector_width"];
    sector_width = s!="" ? stof(s) : 30;

    s = params["numTrialsPrelearn"];
    numTrialsPrelearn = s!="" ? stoi(s) : 700;
    
    s = params["wmmaxFP"];
    wmmaxFP = s!="" ? stof(s) : 0.5;

    s = params["w1maxFP"];
    w1maxFP = s!="" ? stof(s) : 0.7;

    s = params["armReachRadius"];
    armReachRadius = s!="" ? stof(s) : 0.2;

    s = params["randomCBStateInit"];
    randomCBStateInit = s!="" ? stoi(s) : 0;

    s = params["randomCBStateInitAmpl"];
    randomCBStateInitAmpl = s!="" ? stof(s) : 0;

    s = params["rewardSize"];
    rewardSize = s!="" ? stof(s) : 3;

    s = params["cbLRate"];
    cbLRate = s!="" ? stof(s) : 0.5;

    s = params["vectorErrTgtBorder"];
    vectorErrTgtBorder = s!="" ? stoi(s) : 0;

    s = params["rwdNormalizeBycbLRate"];
    rwdNormalizeBycbLRate = s!="" ? stoi(s) : 0;

    s = params["rwdFromcbLRate"];
    rwdFromcbLRate = s!="" ? stoi(s) : 0;

    s = params["cbMotVarEst"];
    cbMotVarEst = s!="" ? stof(s) : 0;

    //s = params["rwdFromcbLRate_thr"];
    //rwdFromcbLRate_thr = s!="" ? stof(s) : 0;

    //s = params["rwdFromcbLRate_mult"];
    //rwdFromcbLRate_mult = s!="" ? stof(s) : 0;

    s = params["rwdFromcbLRate_add"];
    rwdFromcbLRate_add = s!="" ? stof(s) : 0;

    s = params["randomCue_all"];
    bool randomCue_all = s!="" ? stoi(s) : 0;

    s = params["randomTgt_all"];
    bool randomTgt_all = s!="" ? stoi(s) : 0;

    s = params["randomTgtRange_all"];
    float randomTgtRange_all = s!="" ? stof(s) : 0;

    numTrials = 0;
    phaseParams.resize(numPhases);
    for(int i=0;i<numPhases;i++)
    {
      expPhaseParams & p = phaseParams[i];
      p.numTrials = stoi(params[string("numTrials") + to_string(i)] );
      numTrials += p.numTrials;

        
      key = string("name") + to_string(i);
      iter = params.find(key);
      if(iter != params.end())
        p.name = params[key];
      else
        p.name = string("unnamed_phase, phase # = ")+to_string(i);

      key = string("percept_rot") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() && fabs(stof(iter->second) ) > EPS )
      { 
        string val = ((iter->second) );
        //params["actcue_rot"+to_string(i)] = val;
        params["target_rotation"+to_string(i)] = val;
        params["endpoint_rotation"+to_string(i)] = val;
      }

      key = string("percept_xrev") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() && stoi(iter->second) )
      { 
        //params["actcue_rot"+to_string(i)] = "180";
        params["target_xreverse"+to_string(i)] = "1";
        params["endpoint_xreverse"+to_string(i)] = "1";
      }

      key = string("percept_xshift") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() && fabs(stof(iter->second)) > EPS)
      { 
        string val = ((iter->second) );
        //params["actcue_rot"+to_string(i)] = "45";  // we use real prelearn
        // anyway
        params["endpt_xshift"+to_string(i)] = val;
        params["tgt_xshift"+to_string(i)] =   val;
      }

      key = string("percept_yshift") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() && fabs(stof(iter->second)) > EPS)
      { 
        string val = (iter->second) ;
        //params["actcue_rot"+to_string(i)] = "45";
        params["endpt_yshift"+to_string(i)] = val;
        params["tgt_yshift"+to_string(i)]   = val;
      }

      key = string("actcue_rot") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() && fabs(stof(iter->second)) > EPS)
      { 
        //params["cue"+to_string(i)] = "1";
        params["action_rotation"+to_string(i)] = iter->second;
      }

      key = string("action_rotation") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.action_rotation = stof(iter->second);
      }

      key = string("endpoint_rotation") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.endpoint_rotation = stof(iter->second);
        if(fabs(p.endpoint_rotation) > 1000.) 
          cout<<"-------------- too large rotation ------------ init"<<endl;
      }

      key = string("target_rotation") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.target_rotation = stof(iter->second);
      }

      key = string("endpoint_xreverse") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.endpoint_xreverse = stoi(iter->second);
      }

      key = string("cue") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.cue = stoi(iter->second);
      }

      key = string("cueList") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.parseCueList(iter->second);
        p.genCueSeq();
      }

      key = string("target_xreverse") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.target_xreverse = stoi(iter->second);
      }

      key = string("force_field") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.force_field = stof(iter->second);
      }

      key = string("endpt_xshift") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.endpt_xshift = stof(iter->second);
      }

      key = string("endpt_yshift") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.endpt_yshift = stof(iter->second);
      }

      key = string("tgt_xshift") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.tgt_xshift = stof(iter->second);
      }

      key = string("tgt_yshift") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.tgt_yshift = stof(iter->second);
      }

      key = string("defTgt") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.defTgt = fmodAng( stof(iter->second) );
      }
      else
      {
        p.defTgt = defTgt_all;
      }

      key = string("error_clamp") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.error_clamp = stoi(iter->second);
      }

      key = string("learn_bg") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.learn_bg = stoi(iter->second);
      }

      key = string("learn_cb") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      {
        p.learn_cb = stoi(iter->second);
      }

      key = string("cbLRateReset") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        bool val = stoi(iter->second);
        p.cbLRateReset = val;
      }
      
      key = string("errHistReset") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        bool val = stoi(iter->second);
        p.errHistReset = val;
      }

      key = string("resetCBState") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        bool val = stoi(iter->second);
        p.resetCBState = val;
      }

      key = string("setRPre") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        bool val = stoi(iter->second);
        p.setRPre = val;
      }

      key = string("CBtCDS") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        bool val = stoi(iter->second);
        p.CBtCDS = val;
      }

      key = string("resetRPre") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        bool val = stoi(iter->second);
        p.resetRPre = val;
      }

      key = string("cbLRate") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float val = stof(iter->second);
        p.cbLRate = val;
      }
      else
      {
        p.cbLRate = cbLRate;
      }

      key = string("randomCue") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        bool val = stoi(iter->second);
        p.randomCue = val;
      }
      else
      {
        p.randomCue = randomCue_all;
      }

      key = string("randomTgt") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        bool val = stoi(iter->second);
        p.randomTgt = val;
      }
      else
      {
        p.randomTgt = randomTgt_all;
      }

      key = string("randomTgtRange") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float val = stof(iter->second);
        p.randomTgtRange = val;
      }
      else
      {
        p.randomTgtRange = randomTgtRange_all;
      }

      key = string("absRwdSz") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float val = stof(iter->second);
        p.absRwdSz = val;
      }
      else
      {
        p.absRwdSz = rewardSize;
      }
    }

    if(fabs(actRotAngleIncSess) > EPS)
    {
      phaseParams[1].action_rotation += actRotAngleIncSess * num_sess;
    }
    
    if(fabs(endptRotAngleIncSess) > EPS)
    {
      phaseParams[1].endpoint_rotation += endptRotAngleIncSess * num_sess;
    }

    // experiment phase params -- one of the pert (or several), name of the phase, numTrials


    cue2prelearnParam.resize(nc);
    for(int i=0; i<nc; i++)
    {
      phaseParamPrelearn & c2p = cue2prelearnParam[i];
      key = string("actPrelearn") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        int action = stoi(iter->second);
        c2p.action = action;
      }

      key = string("angPrelearn") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float val = stof(iter->second);
        c2p.ang = val;
      }
      else
      {
        c2p.ang = -1000;
      }
      
      key = string("actPrelearnAng") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float angDeg = stof(iter->second);
        angDeg = fmodAng(angDeg); 
        c2p.ang = angDeg;
        c2p.action = deg2action(angDeg);
        //cout<<"init: cue "<<i<<" ang "<<c2p.ang<<" action  "<<c2p.action<<
        //    " angreconv "<<float(c2p.action)/100.*((maxActionAngDeg - minActionAngDeg)) +
        //   minActionAngDeg<<endl;
      }

      //override wmmaxFP, working globally
      key = string("actPrelearn_wmmaxFP") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float wm = stof(iter->second);
        c2p.wmmax = wm;
      }
      else
      {
        c2p.wmmax = wmmaxFP;
      }

      key = string("tgt_xPrelearn") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float val = stof(iter->second);
        c2p.tgt_x = val;
      }

      key = string("tgt_yPrelearn") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float val = stof(iter->second);
        c2p.tgt_y = val;
      }


      if( fabs(c2p.ang) < 360 && fabs(c2p.tgt_x) > 900 )
      { 
        float xc,yc;
        ml.getReachCenterPos(xc,yc);

        float tgtAngleRad = 2.*M_PI*c2p.ang/360.;   // in radians
        c2p.tgt_x = xc+armReachRadius*cos(tgtAngleRad);
        c2p.tgt_y = yc+armReachRadius*sin(tgtAngleRad);
      }

      c2p.patPMC.resize(na);
      fill(c2p.patPMC.begin(),c2p.patPMC.end(),0.);
      c2p.tempWAmpl = w1maxFP;
    }
}

perturbationExperimentEnv::~perturbationExperimentEnv()
{
}
