#include "motor_learning.h"

int MotorLearning::nc=1;

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
        Rpre[i] = 3.;
    }
}

void MotorLearning::flushRpre()
{
    for(int i=0;i<nc;i++)
    {
        Rpre[i] = 0.;
    }
}

float MotorLearning::getRpre(unsigned int k, float R, float * addInfo)  // to be called AFTER bg_learn
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

float MotorLearning::makeTrials(unsigned int ntrials, float * addInfo, bool flushData, unsigned int indAdd, bool doExport)
{
    if(flushData)
    {
        bg.flushWeights(true);
        cb.flush();
        flushRpre();
    }

    bg.resetTrialBegin();
    float T=20;

    float x[nc];
    // cycle over trials
	for(int k=indAdd;k<(ntrials+indAdd);k++)
	{
        int cueActive = env->turnOnCues(x);
        bg.setCues(x);

//        if(learn_bg)
        { 
            if(doExport)
            { 
                bg.exportCuesState(k);
            }

            int nsteps = 0;
            float dt = 0;
            for(float t=0; t<T; )
            {   
                dt = bg.do_step(); 
                nsteps++;
                t+= dt;
                if(dt >= 1.)
                    break;
            };
        } 
//        else
//        {
//            for(int j =0;j<na; j++)
//            {
//                if(wm[cueActive][j] > EPS)
//                    y[j] = 1.;
//                else
//                    y[j] = 0.;
//            }
//        }

        float y[na];
        bg.getPMC(y);

        float addInfoItem[5];
        float sc = env->getSuccess(x,y,k,addInfoItem);   // here arm export happens
        float endpt_percieved_x = addInfoItem[1];
        float endpt_percieved_y = addInfoItem[2];
        float endpt_x = addInfoItem[3];
        float endpt_y = addInfoItem[4];

        float t; // is set in the following function (via a link)
        float R = env->getReward(sc,x,y,t);

        addInfo[k] = addInfoItem[0];

        if(doExport )
        { 
            bg.exportBGstate(k,addInfo);
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

        getRpre(cueActive,R,addInfo);   

	}
    if(doExport)
        bg.exportWeightsOnce();

//    if(doExport)
//        trialEndExport(sumM1freq, 0);

    return 0;
}


MotorLearning::MotorLearning(Hand * hand, Environment * env_, int nc)
    {
        nc=1;
        learn_cb = false;
        learn_bg = true;

        env = env_;
        cb.setHand(hand);
    }

MotorLearning::~MotorLearning()
    {
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
