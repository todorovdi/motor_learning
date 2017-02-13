#include "galea.h"
#include <boost/program_options.hpp>

#define ACTION_CHANGE_SECOND

void runExperiment(int argc, char** argv)
{
    // read command line parameters. Most imortant -- number of sessions and name of the iniFile
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
      ("help,h", "Help screen")
      ("n", po::value<int>()->default_value(1), "Number of experiment runs (sessions)")
      ("iniEnv", po::value<string>()->default_value(""), "Ini params for the environment file name")
      ("prefix", po::value<string>(), "Prefix")
      ("targetPre1", po::value<float>(), "targetPre1")
      ("learn_cb", po::value<int>()->default_value(-1), "learn_cb")
      ("cb_learn_rate", po::value<float>()->default_value(-1.), "cb_learn_rate")
      ("seed", po::value<unsigned int>(), "seed")
      ("suffix", po::value<string>(), "Suffix");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    //notify(vm);

    string paramsEnvFile;

    string prefix = "";
    string suffix = "";

    float targetPre1 = -1;
    if (vm.count("help"))
      std::cout << desc << '\n';
    if (vm.count("iniEnv"))
    { 
        paramsEnvFile =vm["iniEnv"].as<string>();
        if(!paramsEnvFile.length())
            paramsEnvFile = "galea.ini";
    }
    if (vm.count("targetPre1"))
    {
        targetPre1 = vm["targetPre1"].as<float>();
    }

    int learn_cb = vm["learn_cb"].as<int>();
    int nsessions = vm["n"].as<int>(); 
    float cb_learn_rate = vm["cb_learn_rate"].as<float>(); 

    unsigned int seed =   time(NULL);  bool presetSeed = false; 
    if (vm.count("seed"))
    {
        seed = vm["seed"].as<unsigned int>(); 
        presetSeed = true;
    }
    srand(seed);

    cout<<"seed is "<<seed<<endl;
    if(presetSeed)
        cout<<"WARNING: PRESET SEED IS ACTIVE!!!!"<<endl;

    cout<<"nsessions is "<<nsessions<<endl;

    // parallel works only if compiled with a special flag. At least with gcc
#pragma omp parallel for
    for(int i = 0; i<nsessions; i++)
    { 
        testExperimentEnv te(paramsEnvFile,i,targetPre1,learn_cb,cb_learn_rate);
        te.runSession();
    }
}

// should output everything to files
void testExperimentEnv::runSession()
{
//    if(prelearnEachTime)
    //     Maybe we have to do longer prelarn
    { 
        float addInfoTemp[numTrialsPrelearn]; //to be used in prelearn

        cout<<"experimentPhase is "<<phasesNames[PRELEARN]<<endl;
#ifndef DO_FAKE_PRELEARN
        cout<<"num prelearn trials "<<numTrialsPrelearn<<endl;
#endif
        prelearn(numTrialsPrelearn, addInfoTemp);
    }

    // now it is set via ini file
//    ml.setBGlearning(false);
//    ml.setCBlearning(false);

//#ifdef   BG_ENABLED
//        ml.setBGlearning(true);
//#endif
//
//#ifdef   CEREBELLUM_ENABLED
//        ml.setCBlearning(true);
//#endif

        string prefix = string("BG")+to_string(learn_bg) +  string("_CB")+to_string(learn_cb) + string("_learnRate_") + to_string(cb_learn_rate) +  string("_rot_")+to_string(dirShift)+string(" target_")+to_string(targetPre1);
        string putInBeg = to_string(dirShift)+string("\n")+to_string(targetPre1) + string("\n");
        exporter.exportInit(prefix,std::to_string(num_sess),putInBeg);

        // if we did prelearn above
        ml.restoreWeights(false);  // false == not restoring w1,w2
        ml.flushWeights(false); 
        //flushRpre(); // flush all data except wm (we set them from prelearn)
        //float rpre = 3.;
        ml.setRpreMax();   // if we use only CB not need this

    //    initCBdir(targetPre1,true);
        experimentPhase = PRE1;
        cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        ml.makeTrials(numTrialsPre,0,false,0);
        int offset = numTrialsPre;

    //    if(target_rotation1)
    //        initCBdir(targetPre1+dirShift,false);

        experimentPhase = ADAPT1;
        cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        ml.makeTrials(numTrialsAdapt,0,false,offset);
        offset += numTrialsAdapt;

        initCBdir(targetPre1,false);
        experimentPhase = POST1;
        cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        ml.makeTrials(numTrialsPost,0,false,offset);
        offset += numTrialsPost;

//#ifdef TWO_PARTS
        if(numPhases > 3)
        { 
            initCBdir(targetPre2,true);   // update but do not save W, for testing purposes
            experimentPhase = PRE2;
            cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
            ml.makeTrials(numTrialsPre,0,false,offset);
            offset+= numTrialsPre;

            if(target_rotation2)
                initCBdir(targetPre2+dirShift,false);

            experimentPhase = ADAPT2;
            cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
            ml.makeTrials(numTrialsAdapt,0,false,offset);
            offset += numTrialsAdapt;
            
            initCBdir(targetPre2,false);
            experimentPhase = POST2;
            cout<<"session num = "<<num_sess<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
            ml.makeTrials(numTrialsPost,0,false,offset);
        } 
//#endif

        exporter.exportClose();
}

void testExperimentEnv::prelearn(int n, float * addInfo)
{
    experimentPhase = PRELEARN;
    float wmmax;
    if(fake_prelearn)
    { 
        wmmax = wmmax_fake_prelearn;
        int dirIndPre1 = deg2action(targetPre1);
        int dirIndAdapt1 = deg2action(targetPre1+dirShift);
        int dirIndPre2 = deg2action(targetPre2);
        int dirIndAdapt2 = deg2action(targetPre2+dirShift);

        ml.setHabit(0,dirIndPre1,wmmax);
        ml.setHabit(2,dirIndPre2,wmmax);

        if(action_change1)
            ml.setHabit(1,dirIndAdapt1,wmmax);
        else
            ml.setHabit(1,dirIndPre1,wmmax);

        if(action_change2)
            ml.setHabit(3,dirIndAdapt2,wmmax);
        else
            ml.setHabit(3,dirIndPre2,wmmax);

        xcur = 0.2;
        ycur = 0.4;
        cout<<"Fake prelearn max weight is "<<wmmax<<endl;
    }
    else
    { 
        // TODO: careful here!
        ml.setBGlearning(true);
        ml.setCBlearning(false);

        exporter.exportInit("prelearn",to_string(num_sess),"");
        ml.makeTrials(n,addInfo,false,0,true);  // last arg is whether we do export, or not
        exporter.exportClose();

        ml.initParams(params);  // restore bg and cb learning params from ini file
        //
    //    wmmax = 0;
    //    for(int i = 0; i<nc; i++)
    //    {
    //        for(int j = 0; j<na; j++)
    //        {
    //            if(wm[i][j] > wmmax)
    //                wmmax = wm[i][j];
    //        }
    //    }

        cout<<"True prelearn max weight is "<<wmmax<<endl;
    }

    ml.backupWeights();
}

void testExperimentEnv::initCBdir(float dir, bool resetState)
{
    float xc,yc;
    ml.getReachCenterPos(xc,yc);

    float radAngle = 2*M_PI*dir/360;
    float x0=xc+armReachRadius*cos(radAngle);
    float y0=yc+armReachRadius*sin(radAngle);

    float yylast[na] = {}; 
    yylast[deg2action(dir)] = 1.;
    ml.trainCB(x0,y0,yylast,1.,resetState);
}

int testExperimentEnv::deg2action(float degAngle)
{
    return int(float(degAngle) / 360. * 100.);
}

int testExperimentEnv::turnOnCues(float * cues)
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

    cues[cueInd] = 1.;
    return cueInd;
}

float testExperimentEnv::getSuccess(float * x,float * y,unsigned int k,float *addInfo)
{
    float target = 0;
    float rot = 0;
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
            rot = dirShift;
            if(target_rotation1)
                target = targetPre1+rot;
            else
                target = targetPre1; 
            break;
        case POST1:
            rot = 0;
            target = targetPre1;
            break;
        case ADAPT2:
            //rot = dirShift;
            rot = 0;
            if(target_rotation2)
                target = targetPre2+rot;
            else
                target = targetPre2; 
            break;
        case POST2:
            rot = 0;
            target = targetPre2;
            break;
        case PRELEARN:
            rot =0;
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
    ml.moveArm(y,out,0.);
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

    exporter.exportArm(k,xcur,ycur,x0,y0,xc,yc,addInfo);  // export percieved point
    return sc;
}

float testExperimentEnv::getReward(float sc, float * x,float * y, float & param)
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

testExperimentEnv::testExperimentEnv(string paramsEnvFile, int num_sess_,float tgt,int learn_cb_,float cb_learn_rate_):Environment(paramsEnvFile,num_sess_)
{
    numTrialsPre      = stoi(params["numTrialsPre"]);
    numTrialsAdapt    = stoi(params["numTrialsAdapt"]);
    numTrialsPost     = stoi(params["numTrialsPost"]);
    numTrialsPrelearn = stoi(params["numTrialsPrelearn"]);
    numPhases         = stoi(params["numPhases"]);

    fake_prelearn     = stoi(params["fake_prelearn"]);

    action_change1 = stoi(params["action_change1"]);
    action_change2 = stoi(params["action_change2"]);
    endpoint_rotation1 = stoi(params["endpoint_rotation1"]);
    target_rotation1 = stoi(params["target_rotation1"]);
    endpoint_rotation2 = stoi(params["endpoint_rotation2"]);
    target_rotation2 = stoi(params["target_rotation2"]);
    sector_reward = stoi(params["sector_reward"]);
    cue_change1 = stoi(params["cue_change1"]);
    cue_change2 = stoi(params["cue_change2"]);

    dirShift          = stof(params["dirShift"]);
    targetPre1        = stof(params["targetPre1"]);
    targetPre2        = stof(params["targetPre2"]);
    dirShiftInc       = stof(params["dirShiftInc"]);

    if(learn_cb_ != -1)
        params["learn_cb"] = to_string(learn_cb_);
    if(cb_learn_rate_ >= -EPS)
        params["cb_learn_rate"] = to_string(cb_learn_rate_);

    learn_cb = stoi(params["learn_cb"]); // it may look stupid to do so right after prev. line but note that the same parameters object are passed to MotorLearning
    learn_bg = stoi(params["learn_bg"]); 
    cb_learn_rate = stof(params["cb_learn_rate"]);
        

    ml.init(this,&exporter,params);  // we did it once in the Environemnt constructor but we might have changed learn_cb so do it again

    if(tgt>= (-EPS) )
    {
        targetPre1 = tgt;
        targetPre2 = tgt;
    }

    numTrials = numTrialsPre*2+numTrialsAdapt*2+numTrialsPost*2;

    sector_thickness = stof(params["sector_thickness"]);
    sector_width = stof(params["sector_width"]);
    wmmax_fake_prelearn = stof(params["wmmax_fake_prelearn"]);
    armReachRadius = stof(params["armReachRadius"]);

    phasesNames.push_back("PRE1");
    phasesNames.push_back("PRE2");
    phasesNames.push_back("ADAPT1");
    phasesNames.push_back("POST1");
    phasesNames.push_back("ADAPT2");
    phasesNames.push_back("POST2");
    phasesNames.push_back("PRELEARN");

    dirShift += num_sess * dirShiftInc;
}

testExperimentEnv::~testExperimentEnv()
{
}
