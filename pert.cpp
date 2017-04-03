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
    //     Maybe we have to do longer prelarn
    { 
        float * addInfoTemp = new float[numTrialsPrelearn];
        prelearn(numTrialsPrelearn, addInfoTemp);
        delete[] addInfoTemp;
    }

        string prefix = params["datPrefix"] + string("_seed_") + to_string(sess_seed);
        params["dat_basename"] = prefix;

        //cout<<"1"<<endl;

        exporter.exportInit(prefix,"","");
        exporter.exportParams(params);

        //cout<<"2"<<endl;

        // disabled:
        // rotateErr, xreverseErr, randomCBStateInit, trainWithForceField
        //

        // if we did prelearn above
        ml.restoreWeights(true);  // false == not restoring w1,w2
        //ml.flushWeights(false); 
        //flushRpre(); // flush all data except wm (we set them from prelearn)
        //float rpre = 3.;
        ml.setRpreMax();   // if we use only CB not need this

        float cues[nc];
        int cue;
        float x0,y0,angle;
        int offset = 0;

        //cout<<"3"<<endl;

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
              ml.resetCBerr();
              ml.resetCBLRate(p.cbLRate);
            }
            //cout<<"4"<<endl;

            cue = turnOnCues(0,cues);
            getCurTgt(cues,x0,y0,angle);
            initCBdir(x0,y0,cue2prelearnParam[cue].patPMC,
                p.resetCBState); 
          }
          if(p.resetCBState)
          { 
            ml.setRandomCBState(0.);
          }
          ml.setErrorClamp(p.error_clamp);

          if(p.resetRPre)
          {
            ml.setSingleRPre(p.cue,0.);
          }
          
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
    float wmmax0=0,wmmax1=0;
    int wmmax_ind0=-1,wmmax_ind1 = -1;
    if(fake_prelearn)
    { 
      for(int i =0; i<nc; i++)
      {
        phaseParamPrelearn & p = cue2prelearnParam[i];
        ml.fakePrelearnReaching(i,p.action,p.wmmax,p.tempWAmpl);
        fill(p.patPMC.begin(),p.patPMC.end(), 0.);
        p.patPMC[p.action] = p.wmmax;
        //cout<<"prlearn action  "<<p.action<<endl;

        wmmax0 = p.wmmax;
        // or deg2action
      }
      cout<<"Fake prelearn max weight is "<<wmmax0<<endl;
    }
    else
    { 
        cout<<"num prelearn trials "<<numTrialsPrelearn<<endl;
        cout<<"experimentPhase is "<<phaseParams[experimentPhase].name<<endl;

        // TODO: careful here!
        ml.setBGlearning(true);
        ml.setCBlearning(false);

        string prefix = string("prelearn_") + params["datPrefix"] + string("_seed_") + to_string(sess_seed);
        //  + string("_numSess_")+std::to_string(num_sess);

        exporter.exportInit(prefix,"","",true);
        ml.makeTrials(n/2,addInfo,true,0,false);  // last arg is whether we do export, or not (except final weights -- we export them anyway)
       // ml.getLasty(&prelearn_PMC[0][0]);

        experimentPhase = -2;
        cout<<"experimentPhase is "<<phaseParams[experimentPhase].name<<endl;

        ml.makeTrials(n/2,addInfo,false,0,false);  // last arg is whether we do export, or not (except final weights -- we export them anyway)
      //  ml.getLasty(&prelearn_PMC[1][0]);
        exporter.exportClose();

        ml.initParams(params);  // restore bg and cb learning params from ini file
        //
        int i = 0;
        for(int j = 0; j<na; j++)
        {        
            float wmcur = ml.getHabit(i,j);
            if(wmcur > wmmax0)
            { 
                wmmax0 = wmcur;
                wmmax_ind0 = j;
            } 
        }
        i = 1;
        for(int j = 0; j<na; j++)
        {        
            float wmcur = ml.getHabit(i,j);
            if(wmcur > wmmax1)
            { 
                wmmax1 = wmcur;
                wmmax_ind1 = j;
            } 
        }

        cout<<"True prelearn max weight is (cue 0) "<<wmmax0<<" for action "<<wmmax_ind0<<endl;
        cout<<"True prelearn max weight is (cue 1) "<<wmmax1<<" for action "<<wmmax_ind1<<endl;
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

int perturbationExperimentEnv::turnOnCues(int k, float * cues)
{ 
    int cueInd=-100;     
    for(int i=0; i<nc; i++)
        cues[i] = 0.;

    cueInd = phaseParams[experimentPhase].cue;

    cues[cueInd] = 1.;
    return cueInd;
}

void perturbationExperimentEnv::getCurTgt(float * x, float & x0, float & y0, float & tgtAngleDeg)
{
  expPhaseParams & p = phaseParams[experimentPhase];
  float target = p.defTgt + p.target_rotation;
  if(p.target_xreverse)
    target = 180. - target;

  float tgt_xshift = p.tgt_xshift;
  float tgt_yshift = p.tgt_yshift;

  float xc,yc;
    ml.getReachCenterPos(xc,yc);

    //float A=.2;
    tgtAngleDeg = target;   
    float tgtAngleRad = 2*M_PI*tgtAngleDeg/360;   // in radians

    x0=xc+armReachRadius*cos(tgtAngleRad) + tgt_xshift;
    y0=yc+armReachRadius*sin(tgtAngleRad) + tgt_yshift;
}

float perturbationExperimentEnv::getSuccess(float * x,float * y,unsigned int k,float *addInfo)
{
    float rot = 0;
    float sign = 1;
    float ffield = 0.;
    float endpt_xshift = 0;
    float endpt_yshift = 0;

    expPhaseParams & p = phaseParams[experimentPhase];
    rot = p.endpoint_rotation;
    sign = p.endpoint_xreverse ? -1. : 1.;
    ffield = p.force_field;
    endpt_xshift = p.endpt_xshift;
    endpt_yshift = p.endpt_yshift;

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

    //setCBtarget(x0,y0);

    float out[2];
    ml.moveArm(y,out,ffield);
    float xcur_real, ycur_real;
    xcur_real = out[0], ycur_real = out[1];

    // save "table" point coordinates
    addInfo[3] = xcur_real;
    addInfo[4] = ycur_real;
    xcur = xcur_real;
    ycur = ycur_real;

    float xtmp = xcur_real - xc, ytmp = ycur_real -yc;
    float angle = 2.*M_PI/360.*rot;
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
        float angleCur0 = atan( yd/xd) / (2*M_PI) * 360;
        if(xd<0)
            angleCur0 = atan(-xd/yd)/ (2*M_PI) * 360  + (yd>0?90:-90) ;
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

    float xcbt,ycbt;
    ml.getCBtarget(xcbt,ycbt); // pass params as references
    addInfo[5] = xcbt;
    addInfo[6] = ycbt;

    if(doExport)
      exporter.exportArm(k,xcur,ycur,x0,y0,xc,yc,addInfo);  
    return sc;
}

float perturbationExperimentEnv::getReward(float sc, float * x,float * y, float & param)
{
    float R = 0;

    if(!sector_reward)
    { 
        if( fabs(sc) < rewardDist) 
        {
            R = rewardSize;
        }
    }
    else
    {
        if( fabs(sc) < sector_width/2) 
        {
            R = rewardSize;
        }
    } 

    return R;
}  

//    string paramsEnvFile, int num_sess_,float tgt,int learn_cb_,float cbLRate_,unsigned int seed
perturbationExperimentEnv::perturbationExperimentEnv(parmap & params_,int num_sess_,unsigned int sess_seed_):Environment(params_,num_sess_)
{
    sess_seed = sess_seed_;
    params["sess_seed"] = to_string(sess_seed);

    numPhases         = stoi(params["numPhases"]);
    fake_prelearn     = stoi(params["fake_prelearn"]);

    sector_reward = stoi(params["sector_reward"]);

    learn_cb = stoi(params["learn_cb"]); // it may look stupid to do so right after prev. line but note that the same parameters object are passed to MotorLearning
    learn_bg = stoi(params["learn_bg"]); 
        
    ml.init(this,&exporter,params);  // we did it once in the Environemnt constructor but we might have changed learn_cb so do it again

    string key;
    parmap::iterator iter;

    key = string("minActionAngDeg");
    iter = params.find(key);
    if(iter!=params.end())
    {
      minActionAngDeg = stof(iter->second);   
    }
    else
    {
      minActionAngDeg = 0;
    }

    key = string("maxActionAngDeg");
    iter = params.find(key);
    if(iter!=params.end())
    {
      maxActionAngDeg = stof(iter->second);   
    }
    else
    {
      maxActionAngDeg = 360.;
    }

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

      key = string("percept_rot") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() && fabs(stof(iter->second) ) > EPS )
      { 
        string val = ((iter->second) );
        params["actcue_rot"+to_string(i)] = val;
        params["target_rotation"+to_string(i)] = val;
        params["endpoint_rotation"+to_string(i)] = val;
      }

      key = string("percept_xrev") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() && stoi(iter->second) )
      { 
        params["actcue_rot"+to_string(i)] = "180";
        params["target_xreverse"+to_string(i)] = "1";
        params["endpoint_xreverse"+to_string(i)] = "1";
      }

      key = string("percept_xshift") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() && fabs(stof(iter->second)) > EPS)
      { 
        string val = ((iter->second) );
        params["actcue_rot"+to_string(i)] = "45";  // we use real prelearn
        // anyway
        params["endpt_xshift"+to_string(i)] = val;
        params["tgt_xshift"+to_string(i)] =   val;
      }

      key = string("percept_yshift") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() && fabs(stof(iter->second)) > EPS)
      { 
        string val = (iter->second) ;
        params["actcue_rot"+to_string(i)] = "45";
        params["endpt_yshift"+to_string(i)] = val;
        params["tgt_yshift"+to_string(i)]   = val;
      }

      key = string("actcue_rot") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() && fabs(stof(iter->second)) > EPS)
      { 
        params["cue"+to_string(i)] = "1";
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
        p.defTgt = stof(iter->second);
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
        float val = stoi(iter->second);
        p.cbLRateReset = val;
      }

      key = string("resetCBState") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float val = stoi(iter->second);
        p.resetCBState = val;
      }

      key = string("resetRPre") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float val = stoi(iter->second);
        p.resetRPre = val;
      }

      key = string("CBtCDS") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float val = stoi(iter->second);
        p.CBtCDS = val;
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
        p.cbLRate = stof(params["cbLRate"]);
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

    sector_thickness = stof(params["sector_thickness"]);
    sector_width = stof(params["sector_width"]);
    wmmax_fake_prelearn = stof(params["wmmax_fake_prelearn"]);
    fake_prelearn_tempWAmpl = stof(params["fake_prelearn_tempWAmpl"]);
    armReachRadius = stof(params["armReachRadius"]);

    randomCBStateInit = stoi(params["randomCBStateInit"]);
    randomCBStateInitAmpl = stof(params["randomCBStateInitAmpl"]);
    rewardSize = stof(params["rewardSize"]);

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
      
      key = string("actPrelearnAng") + to_string(i);
      iter = params.find(key);
      if(iter != params.end() )
      { 
        float angDeg = stof(iter->second);
        c2p.action = deg2action(angDeg);
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

      c2p.patPMC.resize(na);
      fill(c2p.patPMC.begin(),c2p.patPMC.end(),0.);
      c2p.wmmax = wmmax_fake_prelearn;
      c2p.tempWAmpl = fake_prelearn_tempWAmpl;
    }
}

perturbationExperimentEnv::~perturbationExperimentEnv()
{
}
