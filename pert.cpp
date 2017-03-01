#include "pert.h"
#include <boost/program_options.hpp>

#ifdef PARALLEL
#include <omp.h>
#endif

#define ACTION_CHANGE_SECOND

void runExperiment(int argc, char** argv)
{
    namespace po = boost::program_options;
    po::options_description desc("Options");

    // possible command line params are taken from pert.ini file. Even if you supply another iniEnv later
    parmap paramsPre;
    readIni("pert.ini",paramsPre);
    parmap::iterator it = paramsPre.begin();
    for(;it!=paramsPre.end();it++)
    {
      desc.add_options()(it->first.c_str(),po::value<std::string>()->default_value(it->second),it->first.c_str());
    }
    desc.add_options()("iniEnv", po::value<string>()->default_value(""), "Ini params for the environment file name");
    desc.add_options()("help,h", "Help screen");

    // read command line parameters. Most imortant -- number of sessions and name of the iniFile
//    desc.add_options()
//      ("help,h", "Help screen")
//      ("n", po::value<int>()->default_value(1), "Number of experiment runs (sessions)")
//      ("prefix", po::value<string>(), "prefix")
//      ("targetPre1", po::value<float>(), "targetPre1")
//      ("learn_cb", po::value<int>(), "learn_cb")
//      ("learn_bg", po::value<int>(), "learn_bg")
//      ("cb_learn_rate", po::value<float>(), "cb_learn_rate")
//      ("endpoint_rotation1", po::value<int>(), "endpoint_rotation1")
//      ("endpoint_xreverse1", po::value<int>(), "endpoint_xreverse1")
//      ("action_change1", po::value<int>(), "action_change1")
//      ("cue_change1", po::value<int>(), "cue_change1")
//      ("force_field1", po::value<float>(), "force_field1")
//      ("seed", po::value<unsigned int>(), "seed")
//      ("pdfSuffix", po::value<string>(), "pdfSuffix")
//      ("suffix", po::value<string>(), "Suffix");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    //notify(vm);
    //
    cout<<"cmd line args are "<<endl;
    for(int i=0;i<argc;i++)
    {
      cout<<argv[i]<<endl;
    }

    if (vm.count("help"))
      std::cout << desc << '\n';

    string paramsEnvFile;
    if (vm.count("iniEnv"))
    { 
        paramsEnvFile =vm["iniEnv"].as<string>();
        if(!paramsEnvFile.length())
            paramsEnvFile = "pert.ini";
    }

    parmap params;
    readIni(paramsEnvFile,params);
    
    /////////   before I used several files to store parameters
    // so just to maintain backward compatibility
    parmap::iterator i;
    i = params.find("iniBG");
    if(i!= params.end())
        readIni(i->second,params);

    i = params.find("iniCB");
    if(i!= params.end())
        readIni(i->second,params);

    i = params.find("iniArm");
    if(i!= params.end())
        readIni(i->second,params);

    i = params.find("iniML");
    if(i!= params.end())
        readIni(i->second,params);

    for(it = params.begin();it!=params.end();it++)
    {
      string s = it->first;
      if(vm.count(s) )
      { 
        params[s] = vm[s].as<string>();
      }
    }


    cout<<"pdfSuffix="<<params["pdfSuffix"]<<endl;
    params["datPrefix"] = params["pdfSuffix"]; 

    //int nsessions = vm["n"].as<int>(); 
    //params["nsessions"] = to_string(nsessions); 
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
        cout<<"seed number "<<i<<" is "<<seeds[i]<<endl;
        cout<<"sess num "<<i<<" rnd is "<<rnd()<<endl;
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

        int offset = 0;
        if(numTrialsPre > 0)
        { 
          if(learn_cb)
          { 
              initCBdir(targetPre1,true);
          }
          experimentPhase = PRE1;
          cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
          ml.makeTrials(numTrialsPre,0,false,0);
          offset = numTrialsPre;
        }

        if(learn_cb)
        { 
          if(target_rotation1)
          { 
            initCBdir(angDegAdd(targetPre1,dirShift),false);
          } 
          else if(target_xreverse1)
          {
            initCBdir(180-targetPre1,false);
          }
          if(randomCBStateInit)
          {
            ml.setRandomCBState(randomCBStateInitAmpl);
          }
        } 

        if(stoi(params["trainWith_force_field1"]))
          ml.setFfield(stof(params["force_field1"]));
        if(fabs(rotateErr) > EPS || xreverseErr)
          ml.setModError(true);
        experimentPhase = ADAPT1;
        cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        ml.makeTrials(numTrialsAdapt,0,false,offset);
        offset += numTrialsAdapt;

        if(numTrialsPost > 0)
        { 
          if(stoi(params["trainWith_force_field1"]))
            ml.setFfield(0.);
          if(learn_cb)
          { 
              initCBdir(targetPre1,true);
          }
          ml.setModError(false);
          experimentPhase = ADAPT1;
          experimentPhase = POST1;
          cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
          ml.makeTrials(numTrialsPost,0,false,offset);
          offset += numTrialsPost;
        } 

//#ifdef TWO_PARTS
        if(numPhases > 3)
        { 
            if(learn_cb)
                initCBdir(targetPre2,true);   // update but do not save W, for testing purposes
            experimentPhase = PRE2;
            cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
            ml.makeTrials(numTrialsPre,0,false,offset);
            offset+= numTrialsPre;

            if(target_rotation2 && learn_cb)
                initCBdir(angDegAdd(targetPre2,dirShift),false);

            experimentPhase = ADAPT2;
            cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
            ml.makeTrials(numTrialsAdapt,0,false,offset);
            offset += numTrialsAdapt;
            
            if(target_rotation2 && learn_cb)
                initCBdir(targetPre2,false);
            experimentPhase = POST2;
            cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
            ml.makeTrials(numTrialsPost,0,false,offset);
        } 
//#endif

        exporter.exportClose();
}

void perturbationExperimentEnv::prelearn(int n, float * addInfo)
{
    ml.flushWeights(true);
    experimentPhase = PRELEARN;
    float wmmax;
    int wmmax_ind = -1;
    if(fake_prelearn)
    { 
        wmmax = wmmax_fake_prelearn;
        int dirIndPre1 = deg2action(targetPre1);
        int dirIndAdapt1 = deg2action(angDegAdd(targetPre1,dirShift));
        int dirIndPre2 = deg2action(targetPre2);
        int dirIndAdapt2 = deg2action(angDegAdd(targetPre2,dirShift));


        float tempWAmpl = fake_prelearn_tempWAmpl;

        ml.fakePrelearnReaching(0,dirIndPre1,wmmax,tempWAmpl);
        ml.fakePrelearnReaching(2,dirIndPre2,wmmax,tempWAmpl);

        if(action_change1)
            ml.fakePrelearnReaching(1,dirIndAdapt1,wmmax,tempWAmpl);
        else
            ml.fakePrelearnReaching(1,dirIndPre1,wmmax,tempWAmpl);

        if(action_change2)
            ml.fakePrelearnReaching(3,dirIndAdapt2,wmmax,tempWAmpl);
        else
            ml.fakePrelearnReaching(3,dirIndPre2,wmmax,tempWAmpl);

        xcur = 0.2;
        ycur = 0.4;
        cout<<"Fake prelearn max weight is "<<wmmax<<endl;
    
        wmmax_ind = 0;
    }
    else
    { 
        cout<<"experimentPhase is "<<phasesNames[PRELEARN]<<endl;
        cout<<"num prelearn trials "<<numTrialsPrelearn<<endl;

        // TODO: careful here!
        ml.setBGlearning(true);
        //ml.setCBlearning(true);

        exporter.exportInit("prelearn_",to_string(num_sess),"");
        ml.makeTrials(n,addInfo,true,0,false);  // last arg is whether we do export, or not
        exporter.exportClose();

        ml.initParams(params);  // restore bg and cb learning params from ini file
        //
        wmmax = 0;
        for(int i = 0; i<nc; i++)
        {
            for(int j = 0; j<na; j++)
            {        
                float wmcur = ml.getHabit(i,j);
                if(wmcur > wmmax)
                { 
                    wmmax = wmcur;
                    wmmax_ind = j;
                } 
            }
        }

        cout<<"True prelearn max weight is "<<wmmax<<" for action "<<wmmax_ind<<endl;
    }
    params["wmmax"] = to_string(wmmax);
    params["wmmax_action"] = to_string(wmmax_ind);

    ml.backupWeights();
}

void perturbationExperimentEnv::initCBdir(float dir, bool resetState)
{
    float xc,yc;
    ml.getReachCenterPos(xc,yc);

    float radAngle = 2*M_PI*dir/360;
    float x0=xc+armReachRadius*cos(radAngle);
    float y0=yc+armReachRadius*sin(radAngle);

    //cout<<"train CB for pt= "<<x0<<"  "<<y0<<endl;

    float yylast[na] = {}; 
    yylast[deg2action(dir)] = 1.;
    ml.trainCB(x0,y0,yylast,1.,resetState);
}

int perturbationExperimentEnv::deg2action(float degAngle)
{
    return int(float(degAngle) / 360. * 100.);
}

int perturbationExperimentEnv::turnOnCues(float * cues)
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
    else if (experimentPhase == PRELEARN)
    {
        cueInd = 0;
    }

    cues[cueInd] = 1.;
    return cueInd;
}

float perturbationExperimentEnv::getSuccess(float * x,float * y,unsigned int k,float *addInfo)
{
    float target = 0;
    float rot = 0;
    float sign = 1;
    float ffield = 0.;
    switch(experimentPhase)
    {
        case PRE1:
            rot = 0;
            target = targetPre1;
            break;
        case PRE2:
            // Do something to mimic this simuations, occurin at random
            rot = 0;
            target = targetPre2; 
            break;
        case ADAPT1:
            // Do something to mimic this simuations, occurin at random
            if(endpoint_rotation1)
                rot = dirShift;
            if(target_rotation1)
                target = angDegAdd(targetPre1,dirShift);
            else if(target_xreverse1)   // here we suppose tragetPre1 < 180
                target = 180-targetPre1;
            else
                target = targetPre1; 
            if(endpoint_xreverse1)
              sign = -1;
            ffield = force_field1;
            break;
        case POST1:
            rot = 0;
            target = targetPre1;
            break;
        case ADAPT2:
            if(endpoint_rotation1)
                rot = dirShift;
            if(target_rotation2)
                target = angDegAdd(targetPre2,dirShift);
            else
                target = targetPre2; 
            break;
        case POST2:
            rot = 0;
            target = targetPre2;
            break;
        case PRELEARN:
            rot =0;
            target = targetPre1;
            break;
    }

    float x0,y0;

    //float A=.2;
    float targetAngle = 2*M_PI*target/360;   // in radians

    float xc,yc;
    ml.getReachCenterPos(xc,yc);

    x0=xc+armReachRadius*cos(targetAngle),
    y0=yc+armReachRadius*sin(targetAngle);

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
    float sc = rewardDist+0.01;  // to be unrewarded by default
    if(sector_reward)
    { 
        float dist0=hypot(xcur-xc,ycur-yc); 
        float xd = (xcur-xc);  
        float yd = (ycur-yc);  
        float angleCur0 = atan( yd/xd) / (2*M_PI) * 360;
        if(xd<0)
            angleCur0 = atan(-xd/yd)/ (2*M_PI) * 360  + (yd>0?90:-90) ;
        //float angleCur = angleCur1 < 180 ? angleCur1 : angleCur0 - 180;
        float angleCur = angleCur0 > 0 ? angleCur0 : angleCur0+360.;
        float dif = angleCur-targetAngle/(2*M_PI)*360;
        float dif1 = (dif < 180. ? dif : dif-360);
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

    exporter.exportArm(k,xcur,ycur,x0,y0,xc,yc,addInfo);  // export percieved point
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

//    string paramsEnvFile, int num_sess_,float tgt,int learn_cb_,float cb_learn_rate_,unsigned int seed
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

    dirShift          = stof(params["dirShift"]);
    dirShiftInc       = stof(params["dirShiftInc"]);

    dirShift += num_sess * dirShiftInc;

    // Note that we than save all the params with modifications, to the output
    params["dirShift"] = to_string(dirShift);

    targetPre1        = stof(params["targetPre1"]);
    targetPre2        = stof(params["targetPre2"]);

    learn_cb = stoi(params["learn_cb"]); // it may look stupid to do so right after prev. line but note that the same parameters object are passed to MotorLearning
    learn_bg = stoi(params["learn_bg"]); 
    cb_learn_rate = stof(params["cb_learn_rate"]);

        
    ml.init(this,&exporter,params);  // we did it once in the Environemnt constructor but we might have changed learn_cb so do it again


    numTrials = numTrialsPre+numTrialsAdapt+numTrialsPost;
    if(numPhases == 6)
      numTrials = numTrialsPre*2+numTrialsAdapt*2+numTrialsPost*2;

    sector_thickness = stof(params["sector_thickness"]);
    sector_width = stof(params["sector_width"]);
    wmmax_fake_prelearn = stof(params["wmmax_fake_prelearn"]);
    fake_prelearn_tempWAmpl = stof(params["fake_prelearn_tempWAmpl"]);
    armReachRadius = stof(params["armReachRadius"]);

    randomCBStateInit = stoi(params["randomCBStateInit"]);
    randomCBStateInitAmpl = stof(params["randomCBStateInitAmpl"]);

    phasesNames.push_back("PRE1");
    phasesNames.push_back("PRE2");
    phasesNames.push_back("ADAPT1");
    phasesNames.push_back("POST1");
    phasesNames.push_back("ADAPT2");
    phasesNames.push_back("POST2");
    phasesNames.push_back("PRELEARN");
}

perturbationExperimentEnv::~perturbationExperimentEnv()
{
}
