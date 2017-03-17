#include "pert.h"

#ifdef PARALLEL
#include <omp.h>
#endif

#define ACTION_CHANGE_SECOND

void runExperiment(parmap & params)
{
    int nsessions = stoi(params["nsessions"]); 

    unsigned int seed =   time(NULL);  
    bool presetSeed = false; 

    unsigned int t_seed = stoul(params["seed"]);
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
        float addInfoTemp[numTrialsPrelearn]; //to be used in prelearn

        prelearn(numTrialsPrelearn, addInfoTemp);
    }

        string prefix = params["datPrefix"] + string("_numSess_")+std::to_string(num_sess);
        exporter.exportInit(prefix,"","");

        exporter.exportParams(params);

        params["dat_basename"] = prefix;

        float rotateErr=stof(params["rotateErr"]); ;
        bool xreverseErr=stoi(params["xreverseErr"]); ;

        // if we did prelearn above
        ml.restoreWeights(true);  // false == not restoring w1,w2
        //ml.flushWeights(false); 
        //flushRpre(); // flush all data except wm (we set them from prelearn)
        //float rpre = 3.;
        ml.setRpreMax();   // if we use only CB not need this

        float cues[nc];
        float x0,y0,angle;
        int offset = 0;
        if(numTrialsPre > 0)
        { 
          experimentPhase = PRE1;
          if(learn_cb)
          { 
            ml.resetCBerr();
            ml.resetCBLRate();
            
            turnOnCues(0,cues);
            getCurTgt(cues,x0,y0,angle);
            initCBdir(x0,y0,prelearn_PMC_0,true); 
          }
          cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
          ml.makeTrials(numTrialsPre,0,false,0);
          offset = numTrialsPre;
        }


        experimentPhase = ADAPT1;
        if(learn_cb)
        { 
          ml.resetCBerr();
          ml.resetCBLRate();

          turnOnCues(0,cues);
          getCurTgt(cues,x0,y0,angle);
          initCBdir(x0,y0,prelearn_PMC_1,false); 
          //if(target_rotation1)
          //{ 
          //  initCBdir(angDegAdd(targetPre1,dirShift),prelearn_PMC_0,false);
          //} 
          //else if(target_xreverse1)
          //{
          //  initCBdir(180-targetPre1,prelearn_PMC_0,false);
          //}
          if(randomCBStateInit)
          {
            ml.setRandomCBState(randomCBStateInitAmpl);
          }
        } 

        if(stoi(params["trainWith_force_field1"]))
          ml.setFfield(stof(params["force_field1"]));
        if(fabs(rotateErr) > EPS || xreverseErr)
          ml.setModError(true);
        cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        ml.makeTrials(numTrialsAdapt,0,false,offset);
        offset += numTrialsAdapt;

        if(numTrialsPost > 0)
        { 
          experimentPhase = POST1;
          if(stoi(params["trainWith_force_field1"]))
            ml.setFfield(0.);
          if(learn_cb)
          { 
            ml.resetCBerr();
            ml.resetCBLRate();
            turnOnCues(0,cues);
            getCurTgt(cues,x0,y0,angle);
            initCBdir(x0,y0,prelearn_PMC_0,false); 
          }
          ml.setModError(false);
          cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
          ml.makeTrials(numTrialsPost,0,false,offset);
          offset += numTrialsPost;
        } 

//#ifdef TWO_PARTS
        if(numPhases > 3)
        { 
            experimentPhase = PRE2;
            if(learn_cb)
            { 
              turnOnCues(0,cues);
              getCurTgt(cues,x0,y0,angle);
              initCBdir(x0,y0,prelearn_PMC_0,true); 
            }
            cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
            ml.makeTrials(numTrialsPre,0,false,offset);
            offset+= numTrialsPre;

            experimentPhase = ADAPT2;
            if(learn_cb)
            { 
              turnOnCues(0,cues);
              getCurTgt(cues,x0,y0,angle);
              initCBdir(x0,y0,prelearn_PMC_1,false); 
            }
            cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
            ml.makeTrials(numTrialsAdapt,0,false,offset);
            offset += numTrialsAdapt;
            
            experimentPhase = POST2;
            if(learn_cb)
            { 
              turnOnCues(0,cues);
              getCurTgt(cues,x0,y0,angle);
              initCBdir(x0,y0,prelearn_PMC_0,false); 
            }
            cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
            ml.makeTrials(numTrialsPost,0,false,offset);
        } 
//#endif

        exporter.exportClose();
}

void perturbationExperimentEnv::prelearn(int n, float * addInfo)
{
    ml.flushWeights(true);
    experimentPhase = PRELEARN_0;
    float wmmax0=0,wmmax1=0;
    int wmmax_ind0=-1,wmmax_ind1 = -1;
    if(fake_prelearn)
    { 
        wmmax0 = wmmax_fake_prelearn;
        wmmax1 = wmmax_fake_prelearn;
        int dirIndPre1 = deg2action(targetPre1);
        int dirIndAdapt1 = deg2action(angDegAdd(targetPre1,dirShift));
        int dirIndPre2 = deg2action(targetPre2);
        int dirIndAdapt2 = deg2action(angDegAdd(targetPre2,dirShift));


        float tempWAmpl = fake_prelearn_tempWAmpl;

        ml.fakePrelearnReaching(0,dirIndPre1,wmmax0,tempWAmpl);
        ml.fakePrelearnReaching(2,dirIndPre2,wmmax0,tempWAmpl);

        if(action_change1)
            ml.fakePrelearnReaching(1,dirIndAdapt1,wmmax0,tempWAmpl);
        else
            ml.fakePrelearnReaching(1,dirIndPre1,wmmax0,tempWAmpl);

        if(action_change2)
            ml.fakePrelearnReaching(3,dirIndAdapt2,wmmax0,tempWAmpl);
        else
            ml.fakePrelearnReaching(3,dirIndPre2,wmmax0,tempWAmpl);

        xcur = 0.2;
        ycur = 0.4;
        cout<<"Fake prelearn max weight is "<<wmmax0<<endl;
    
        wmmax_ind0 = dirIndPre1;
        wmmax_ind1 = dirIndAdapt1;

        prelearn_PMC_0[wmmax_ind0] = wmmax0;
        prelearn_PMC_1[wmmax_ind1] = wmmax1;
    }
    else
    { 
        cout<<"num prelearn trials "<<numTrialsPrelearn<<endl;
        cout<<"experimentPhase is "<<phasesNames[experimentPhase]<<endl;

        // TODO: careful here!
        ml.setBGlearning(true);
        ml.setCBlearning(false);

        string prefix = string("prelearn_") + params["datPrefix"] + string("_numSess_")+std::to_string(num_sess);

        exporter.exportInit(prefix,"","",true);
        ml.makeTrials(n/2,addInfo,true,0,false);  // last arg is whether we do export, or not (except final weights -- we export them anyway)
        ml.getLasty(&prelearn_PMC_0[0]);

        experimentPhase = PRELEARN_1;
        cout<<"experimentPhase is "<<phasesNames[experimentPhase]<<endl;

        ml.makeTrials(n/2,addInfo,false,0,false);  // last arg is whether we do export, or not (except final weights -- we export them anyway)
        ml.getLasty(&prelearn_PMC_1[0]);
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
    params["wmmax0"] = to_string(wmmax0);
    params["wmmax_action0"] = to_string(wmmax_ind0);
    params["wmmax1"] = to_string(wmmax1);
    params["wmmax_action1"] = to_string(wmmax_ind1);

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
    return int(float(degAngle) / 360. * 100.);
}

int perturbationExperimentEnv::turnOnCues(int k, float * cues)
{ 
    int cueInd=-100;     
    for(int i=0; i<nc; i++)
        cues[i] = 0.;

    if(experimentPhase == PRE1 || experimentPhase == POST1) 
        cueInd = 0;
    else if(experimentPhase == ADAPT1) 
    { 
        if(cue_change1)
            cueInd = 1;
        else
            cueInd = 0;
    }
    else if (experimentPhase == PRE2 || experimentPhase == POST2) 
        cueInd = 2;
    else if (experimentPhase == ADAPT2) 
    { 
        if(cue_change1)
            cueInd = 3;
        else
            cueInd = 2;
    } 
    else if (experimentPhase == PRELEARN_0)
    {
      cueInd = 0;
      if(numPhases == 6)  // then we use two cues
      {
        cueInd = 2*(k < numTrialsPrelearn/2 ? 0 : 1);
      }
    }
    else if (experimentPhase == PRELEARN_1)
    {
      cueInd = 1;
      if(numPhases == 6)  // then we use two cues
      {
        cueInd = 1 + 2*(k < numTrialsPrelearn/2 ? 0 : 1);
      }
    }

    cues[cueInd] = 1.;
    return cueInd;
}

void perturbationExperimentEnv::getCurTgt(float * x, float & x0, float & y0, float & tgtAngleDeg)
{
  float target = 0;
  float tgt_xshift = 0;
  float tgt_yshift = 0;
  switch (experimentPhase)
  { 
        case PRE1:
            target = targetPre1;
            break;
        case PRE2:
            // Do something to mimic this simuations, occurin at random
            target = targetPre2; 
            break;
        case ADAPT1:
            // Do something to mimic this simuations, occurin at random
            if(target_rotation1)
                target = angDegAdd(targetPre1,dirShift);
            else if(target_xreverse1)   // here we suppose tragetPre1 < 180
                target = angDegAdd(180,-targetPre1);
            else
                target = targetPre1; 
            tgt_xshift = tgt_xshift1;
            tgt_yshift = tgt_yshift1;
            break;
        case POST1:
            target = targetPre1;
            break;
        case ADAPT2:
            if(target_rotation2)
                target = angDegAdd(targetPre2,dirShift);
            else
                target = targetPre2; 
            break;
        case POST2:
            target = targetPre2;
            break;
        case PRELEARN_0:
            target = targetPre1;
            break;
        case PRELEARN_1:
            target = targetPre1;
            if(nc>1 && x[1] > EPS && action_change1)
            { 
              if(target_rotation1)
                  target = angDegAdd(targetPre1,dirShift);
              else if(target_xreverse1)   // here we suppose tragetPre1 < 180
                  target = angDegAdd(180,-targetPre1);
              else
                  target = targetPre1; 

              tgt_xshift = tgt_xshift1;
              tgt_yshift = tgt_yshift1;
            }
            break;
    }

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

    bool doExport = true;
    switch(experimentPhase)
    {
        case ADAPT1:
            // Do something to mimic this simuations, occurin at random
            if(endpoint_rotation1)
                rot = dirShift;
            endpt_xshift = endpt_xshift1;
            endpt_yshift = endpt_yshift1;
            if(endpoint_xreverse1)
              sign = -1;
            ffield = force_field1;
            break;
        case ADAPT2:
            if(endpoint_rotation1)
                rot = dirShift;
            break;
        case PRELEARN_0:
        case PRELEARN_1:
            doExport = false;
            break;
    }

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

    if( endpoint_rotation1 || endpoint_rotation2)
    { 
        float xtmp = xcur_real - xc, ytmp = ycur_real -yc;
        float angle = 2.*M_PI/360.*rot;
        xcur = xtmp*cos(angle) - ytmp*sin(angle) + xc;
        ycur = xtmp*sin(angle) + ytmp*cos(angle) + yc;
    } 
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
            R = 3;
        }
    }
    else
    {
        if( fabs(sc) < sector_width/2) 
        {
            R = 3;
        }
    } 

    return R;
}  

//    string paramsEnvFile, int num_sess_,float tgt,int learn_cb_,float cbLRate_,unsigned int seed
perturbationExperimentEnv::perturbationExperimentEnv(const parmap & params_,int num_sess_,unsigned int sess_seed_):Environment(params_,num_sess_)
{
    sess_seed = sess_seed_;
    params["sess_seed"] = to_string(sess_seed);

    numTrialsPre      = stoi(params["numTrialsPre"]);
    numTrialsAdapt    = stoi(params["numTrialsAdapt"]);
    numTrialsPost     = stoi(params["numTrialsPost"]);
    numTrialsPrelearn = stoi(params["numTrialsPrelearn"]);
    numPhases         = stoi(params["numPhases"]);

    fake_prelearn     = stoi(params["fake_prelearn"]);

    int percept_rot1=stoi(params["percept_rot1"]);
    if(percept_rot1)
    { 
      params["actcue_change1"] = "1";
      params["target_rotation1"] = "1";
      params["endpoint_rotation1"] = "1";
    }
    int percept_xrev1=stoi(params["percept_xrev1"]);
    if(percept_xrev1)
    { 
      params["actcue_change1"] = "1";
      params["target_xreverse1"] = "1";
      params["dirShift"] = "180";
      params["endpoint_xreverse1"] = "1";
    }

    float percept_xshift1=stof(params["percept_xshift1"]);
    if(fabs(percept_xshift1) > EPS)
    { 
      params["actcue_change1"] = "1";
      params["endpt_xshift1"] = to_string(percept_xshift1);
      params["tgt_xshift1"] = to_string(percept_xshift1);
    }

    float percept_yshift1=stof(params["percept_yshift1"]);
    if(fabs(percept_yshift1) > EPS)
    { 
      params["actcue_change1"] = "1";
      params["endpt_yshift1"] = to_string(percept_yshift1);
      params["tgt_yshift1"] = to_string(percept_yshift1);
    }

    int actcue_change1=stoi(params["actcue_change1"]);
    if(actcue_change1)
    { 
      params["cue_change1"] = "1";
      params["action_change1"] = "1";
    }

    action_change1 = stoi(params["action_change1"]);
    action_change2 = stoi(params["action_change2"]);
    endpoint_rotation1 = stoi(params["endpoint_rotation1"]);
    endpoint_rotation2 = stoi(params["endpoint_rotation2"]);
    target_rotation2 = stoi(params["target_rotation2"]);
    target_rotation1 = stoi(params["target_rotation1"]);
    target_xreverse1 = stoi(params["target_xreverse1"]);

    sector_reward = stoi(params["sector_reward"]);
    cue_change1 = stoi(params["cue_change1"]);
    cue_change2 = stoi(params["cue_change2"]);
    endpoint_xreverse1 = stoi(params["endpoint_xreverse1"]);
    force_field1 = stof(params["force_field1"]);
    endpt_xshift1 = stof(params["endpt_xshift1"]);
    endpt_yshift1 = stof(params["endpt_yshift1"]);
    tgt_xshift1 = stof(params["tgt_xshift1"]);
    tgt_yshift1 = stof(params["tgt_yshift1"]);

    dirShift          = stof(params["dirShift"]);
    dirShiftIncSess       = stof(params["dirShiftIncSess"]);

    dirShift += num_sess * dirShiftIncSess;

    // Note that we than save all the params with modifications, to the output
    params["dirShift"] = to_string(dirShift);

    targetPre1        = stof(params["targetPre1"]);
    targetPre2        = stof(params["targetPre2"]);

    learn_cb = stoi(params["learn_cb"]); // it may look stupid to do so right after prev. line but note that the same parameters object are passed to MotorLearning
    learn_bg = stoi(params["learn_bg"]); 
    cbLRate = stof(params["cbLRate"]);

        
    ml.init(this,&exporter,params);  // we did it once in the Environemnt constructor but we might have changed learn_cb so do it again

    numTrials = numTrialsPre+numTrialsAdapt+numTrialsPost;
    if(numPhases == 6)
      numTrials = numTrialsPre*2+numTrialsAdapt*2+numTrialsPost*2;

    // experiment phase params -- one of the pert (or several), name of the phase, numTrials

    sector_thickness = stof(params["sector_thickness"]);
    sector_width = stof(params["sector_width"]);
    wmmax_fake_prelearn = stof(params["wmmax_fake_prelearn"]);
    fake_prelearn_tempWAmpl = stof(params["fake_prelearn_tempWAmpl"]);
    armReachRadius = stof(params["armReachRadius"]);

    randomCBStateInit = stoi(params["randomCBStateInit"]);
    randomCBStateInitAmpl = stof(params["randomCBStateInitAmpl"]);

    prelearn_PMC_0.resize(na);
    prelearn_PMC_1.resize(na);

    phasesNames.push_back("PRE1");
    phasesNames.push_back("PRE2");
    phasesNames.push_back("ADAPT1");
    phasesNames.push_back("POST1");
    phasesNames.push_back("ADAPT2");
    phasesNames.push_back("POST2");
    phasesNames.push_back("PRELEARN_0");
    phasesNames.push_back("PRELEARN_1");
}

perturbationExperimentEnv::~perturbationExperimentEnv()
{
}
