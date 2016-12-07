#include "learn.h"
#include "galea_export.h"
#include "string.h"

//#include "params_galea.h"
#include "params_common.h"



///////////////

float rewardDist=0.09;  // distance the hand has to reach the success to count
int experimentPhase=0;   

/////////////////////////////
//
const int multNumTrials = 1; // our model does not work on such small 

//unsigned int nsessions = 15*8; // article - 15 
//unsigned int nsessions =90; 
//unsigned int nsessions =30; 
unsigned int nsessions = 1; 

const int numTrialsPre   = multNumTrials * 12;
const int numTrialsAdapt = multNumTrials * 25;
const int numTrialsAdapt2 = multNumTrials * 18;
const int numTrialsPost  = multNumTrials * 12;
//unsigned int numTrials = 2*numTrialsPre + numTrialsAdapt + numTrialsAdapt2 + 2*numTrialsPost; 
unsigned int numTrials = 2*numTrialsPre + numTrialsAdapt + numTrialsPost; 
//unsigned int numTrialsPrelearn = 1200;
unsigned int numTrialsPrelearn = 500;

int nc=1; // number of cues

const char * phasesNames[] = { "PRE1", "PRE2", "ADAPT1", "POST1", "ADAPT2", "POST2", "PRELEARN" };


const float armReachRadius = 0.2;    
float sector_thickness = 0.1;
float sector_width = 20; // 30   // in degrees

float wmmax_fake_prelearn = 0.4; // 0.65 -- no adaptation (and aftereffects), though w2 reach max values = 3
// 0.6 trying to adapt no success, no aftereffects (bacause of this)

float xcur=0.,ycur=0.;

#define CEREBELLUM_ENABLED

#define PRELEARN_EACH_TIME
//#define DO_FAKE_PRELEARN
 
#define SECTOR_REWARD
//#define HOR_REWARD_SLAVA

float dirShift = 30;  // in degrees

//float directions[8] = {0,45,90,135,180,225,270,315};       // in degrees

float getSuccess(float * x,float * y,unsigned int k, float * addInfo)
{

    float rot = 0;
    switch(experimentPhase)
    {
        case PRE1:
            rot = 0;
            break;
        case PRE2:
            // Do something to mimic this simuations, occurin at random
            rot = 0;
            break;
        case ADAPT1:
            // Do something to mimic this simuations, occurin at random
            rot = dirShift;
            break;
        case ADAPT2:
            rot = dirShift;
            break;
        case POST1:
            rot = 0;
            break;
        case POST2:
            rot = 0;
            break;
        case PRELEARN:
            rot =0;
            break;
    }

    // TODO: put out from trial cycle
    // read from AllD cortical data 
	float phi0[2]={ -0.832778,	1.16426};

    // just take two float numbers from file (-0.8 and 1.16)
	//ifstream("ini")>>phi0[0]>>phi0[1];
	float xc=(-L1*sin(phi0[0])+-L2*sin(phi0[1])),yc=(L1*cos(phi0[0])+L2*cos(phi0[1]));

    float target = rot;

    float A=.2;
    float targetAngle = 2*M_PI*target/360;   // in radians

    float x0,y0;
    x0=xc+A*cos(targetAngle),
    y0=yc+A*sin(targetAngle);

    
    float phi[4]={};
    phi[0]=phi0[0]; phi[1]=phi0[1]; phi[2]=0; phi[3]=0;

    float out[2];
    moveHand(phi,y,out);
    xcur = out[0], ycur = out[1];

    float sc = rewardDist+0.1;  // to be unrewarded by default
#if defined (SECTOR_REWARD)
    float dist0=hypot(xcur-xc,ycur-yc); 
    float xd = (xcur-xc);//  /dist0;    // normalized to lie on a circle
    float yd = (ycur-yc);//  /dist0;
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
        sc = fabs(dif1) > sector_width+0.1 ? dif : sector_width+0.1;
    *addInfo = -dif1;
#else 
    float dist0=hypot(xcur-x0,ycur-y0);
    sc = dist0;
    *addInfo = -(xcur-x0);
#endif

#ifdef CEREBELLUM_ENABLED
    cblearn(0,0);  // how do I know which point I should reach?
#endif

    exportArm(k,xcur,ycur,x0,y0,xc,yc,addInfo);
    return sc;
}

float getReward(float sc, float * x,float * y, float& t)  
{
    float R = 0;

#ifndef SECTOR_REWARD
    if( fabs(sc) < rewardDist) 
    {
        R = 3;
    }
#else
    if( fabs(sc) < sector_width/2) 
    {
        R = 3;
    }
#endif

    return R;
}

int turnOnCues(float * x)
{
    unsigned char cueInd = 0;     
    x[cueInd] = 1.;
    return cueInd;
}

void prelearn(int n, float * addInfo)
{
    experimentPhase = PRELEARN;
    float wmmax;
#ifdef DO_FAKE_PRELEARN
    wmmax = wmmax_fake_prelearn;
    wm[0][15] = wmmax;
    cout<<"Fake prelearn max weight is "<<wmmax<<endl;
#else
    makeTrials(n,memoryLen,addInfo,true,0,false);  // last arg is whether we do export, or not
    //
    wmmax = 0;
    for(int i = 0; i<nc; i++)
    {
        for(int j = 0; j<na; j++)
        {
            if(wm[i][j] > wmmax)
                wmmax = wm[i][j];
        }
    }

    cout<<"True prelearn max weight is "<<wmmax<<endl;
#endif
}


void extractErrors(float** addInfo, float* errorsOut, float* SMEout, int n)
{
    // subtract mean
    float meanPreErr[nsessions] = {};
    for(int i =0;i<nsessions;i++)
    {
        for(int j=0;j<numTrialsPre*2;j++)
        {
            meanPreErr[i] += addInfo[i][j];
        }    
        meanPreErr[i] = 0.;// /= numTrialsPre*2;
    }
//    float xaver=0,yaver=0;
    for(int j =0;j<n;j++)
    { 
        errorsOut[j] = 0.;
        SMEout[j] = 0.;
        for(int i =0;i<nsessions;i++)
        {
//            float x,y;
//            float angle = addInfo[i][j]
//            x = cos(angle);
//            y = sin(angle);
            
            errorsOut[j] += ( addInfo[i][j] - meanPreErr[i]) / float(nsessions);
        }
        for(int i =0;i<nsessions;i++)
        {
            float tt = (addInfo[i][j] - meanPreErr[i] - errorsOut[j]);
            SMEout[j] += tt*tt;
        }
        SMEout[j] = sqrt(SMEout[j])/nsessions; //sqrt(nsessions);
    }

    //movingAverageFilter(successRatesAver, n, 10, successRatesOut);
}

void runExperiment(int argc, char** argv)
{
    float * addInfo[nsessions];  

    for(int i=0;i<nsessions;i++)
    {
        addInfo[i] = new float[numTrials];
    }

#ifndef PRELEARN_EACH_TIME
    //     Maybe we have to do longer prelarn
    float successTemp[numTrialsPrelearn]; //to be used in prelearn
    cout<<"experimentPhase is "<<phasesNames[PRELEARN]<<endl;
    cout<<"num prelearn trials "<<numTrialsPrelearn<<endl;
    prelearn(numTrialsPrelearn, successTemp);
    backupWeights();
#endif


    for(int i=0;i<nsessions;i++)
    {
        /////////////////////////////////////////
        /////////////////////////////////////////
        /////////////////////////////////////////
        
#ifdef PRELEARN_EACH_TIME
        //     Maybe we have to do longer prelarn
        float successTemp[numTrialsPrelearn]; //to be used in prelearn
        cout<<"experimentPhase is "<<phasesNames[PRELEARN]<<endl;
        cout<<"num prelearn trials "<<numTrialsPrelearn<<endl;
        prelearn(numTrialsPrelearn, successTemp);
        backupWeights();
#endif

        // init CB after prelearning
        float x0 = xcur, y0 = xcur;  // set them to be last reached pts. Works only with real prelearning
        // y should be valid as well, but we flush y before doing a trial and not after, so should be ok
        initCB(x0,y0,1.);

        
        exportInit("galea");

        // if we did prelearn above
        restoreWeights(true);
        //flushWeights(false); 
        //flushRpre(); // flush all data except wm (we set them from prelearn)
        float rpre = 3.;
        setRpre(&rpre);

        experimentPhase = PRE1;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsPre,memoryLen,addInfo[i],false,0);
        int offset = numTrialsPre;

        experimentPhase = PRE2;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsPre,memoryLen,addInfo[i],false,offset);
        offset+= numTrialsPre;

        experimentPhase = ADAPT1;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsAdapt,memoryLen,addInfo[i],false,offset);
        offset += numTrialsAdapt;

        experimentPhase = POST1;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsPost,memoryLen,addInfo[i],false,offset);
        offset += numTrialsPost;

//        experimentPhase = ADAPT2;
//        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
//        makeTrials(numTrialsAdapt2,memoryLen,addInfo[i],false,offset);
//        offset += numTrialsAdapt2;
//        
//        experimentPhase = POST2;
//        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
//        makeTrials(numTrialsPost,memoryLen,addInfo[i],false,offset);

        exportClose();
    }

    float errors[numTrials] = {};
    float SME[numTrials] = {};
    extractErrors(addInfo, errors, SME, numTrials);

    char fname[256] = "./output_galea/";
    const char * prefix="galea";
    strcat(strcat(fname,prefix), "_errors.dat");
    foutErr.open(fname);
    for(int j =0;j<numTrials;j++)
    {
        foutErr<<j<<"\t"<<errors[j]<<"\t"<<SME[j]<<endl;
    }
    foutErr.close();

    for(int i=0;i<nsessions;i++)
    {
        delete addInfo[i];
    }
}
