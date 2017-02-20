#include "motor_learning.h"
#include "environment.h"

//int MotorLearning::nc=1;

void MotorLearning::inactivateBG()
{
    bg.inactivate(); 
    //BGactive = false;
}

void MotorLearning::activateBG()
{
    bg.activate();
    //BGactive = true;
}

void MotorLearning::setRpre(float * rpre)
{
    for(int i=0;i<nc;i++)
    {
        Rpre[i] = rpre[i];
    }
}

void MotorLearning::setRpreMax()
{
    for(int i=0;i<nc;i++)
    {
        Rpre[i] = rewardSize;
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

void MotorLearning::setRandomCBState(float a)
{
    cb.setRandomState(a);
}

// calls getSuccess, which calls moveArm
float MotorLearning::makeTrials(unsigned int ntrials, float * addInfo, bool flushData, unsigned int indAdd, bool doExport)
{

    if(flushData)
    {
        bg.flushWeights(true);
        cb.flush();
        flushRpre();
    }

    float x[nc];
    // cycle over trials
    for(int k=indAdd;k<(ntrials+indAdd);k++)
      {

        bg.resetForTrialBegin();

        int cueActive = env->turnOnCues(x);
        bg.setCues(x);

        if(learn_bg)
	  {

            if(doExport)
	      { 
                bg.exportCuesState(k);
	      }

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
                if(dt >= 1.)
		  break;
	      };
	    if (CONT_OUT) {bg.exportContClose();}
	  } 
        else
	  {
            bg.habit2PMCdirectly(cueActive);
	  }

        float y[na];
        bg.getPMC(y);

        float addInfoItem[5];
        float sc = env->getSuccess(x,y,k,addInfoItem);   // here arm export happens
        float endpt_percieved_x = addInfoItem[1];
        float endpt_percieved_y = addInfoItem[2];
        float endpt_x = addInfoItem[3];
        float endpt_y = addInfoItem[4];

        float t; // is may be set in the following function (it passes as a reference argument)
        float R = env->getReward(sc,x,y,t);

        if(doExport )
	  { 
            bg.exportBGstate(k,0);
	  } 

        //rnd();  // just to follow same seed as Slava's code
        if(learn_bg)
	  bg.learn(R- Rpre[cueActive]);

        if(learn_cb)
	  { 
            //if( fzero(R) )
            { cb.learn(endpt_percieved_x, endpt_percieved_y); }
	    // else
	    // { x_cb_target = endpt_x; y_cb_target = endpt_y;  }
	  }

        updateRpre(cueActive,R,0);   

      }
    if(doExport)
      bg.exportWeightsOnce();

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

float MotorLearning::getHabit(int cue, int action)
{
    bg.getwm(cue,action);
}

void MotorLearning::restoreWeights(bool w12too)
{
    bg.restoreWeights(w12too);
}

MotorLearning::MotorLearning(Environment * env_, Exporter * exporter_, parmap & params ) 
{
    init(env_,exporter_,params);
}

MotorLearning::MotorLearning()
{
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
    rewardSize =  stof(params["rewardSize"]);

    textExport=stoi(params["textExport"]);

    nc=stoi(params["nc"]);
    na=stoi(params["na"]);
}

void MotorLearning::init(Environment* env_, Exporter* exporter_,parmap & params)
{
    initParams(params);
    env = env_;

    exporter = exporter_;

    arm.init(params);
    cb.init(params,exporter,&arm);
    bg.init(params,exporter);

    Rpre.resize(nc);
}

void MotorLearning::getReachCenterPos(float &x, float&y)
{
    arm.getReachCenterPos(x,y);
}

void MotorLearning::moveArm(float * PMC_activity, float * out, float ffield)
{
    cb.moveArm(PMC_activity,out,ffield);
}

void MotorLearning::trainCB(float x0, float y0, float * yy, float coef, bool flushW) 
{
    cb.train(x0,y0,yy,coef,flushW);
}

void MotorLearning::flushWeights(bool wmtoo)
{
    bg.flushWeights(wmtoo);
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
