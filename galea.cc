// Galea experiment, cerebellum version

#include "learn.h"
#include "galea_export.h"
#include "string.h"

//#include "params_galea.h"
#include "params_common.h"



///////////////

float rewardDist=0.02;  // distance the hand has to reach the success to count
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
//const int numTrialsAdapt2 = multNumTrials * 18;
const int numTrialsPost  = multNumTrials * 12;
//unsigned int numTrials = 2*numTrialsPre + numTrialsAdapt + numTrialsAdapt2 + 2*numTrialsPost; 
unsigned int numTrials = 3*numTrialsPre + 2*numTrialsAdapt + 2*numTrialsPost; 
//unsigned int numTrialsPrelearn = 1200;
unsigned int numTrialsPrelearn = 500;

int nc=4; // number of cues

const char * phasesNames[] = { "PRE1", "PRE2", "ADAPT1", "POST1", "ADAPT2", "POST2", "PRELEARN", "PRE3" };

// RIGHT, TOP, LEFT, BOTTOM
const float xcompass[] = {0.2, 0., -0.2, 0.};
const float ycompass[] = {0.4, 0.6, 0.4, 0.2};
const int compActionIndices[] = {0,24,49,74};

const float armReachRadius = 0.2;    
float sector_thickness = 0.1;
float sector_width = 20; // 30   // in degrees

float wmmax_fake_prelearn = 0.4; // 0.65 -- no adaptation (and aftereffects), though w2 reach max values = 3
// 0.6 trying to adapt no success, no aftereffects (bacause of this)

float xcur=0.,ycur=0.;

float targetAdapt2 = 90;  // in degrees
float dirShift = 30;  // in degrees

#define CEREBELLUM_ENABLED
#define CB_MULTI_POINT_INIT


////#define NO_ACTION_ROTATION
//
#define ACTION_ROTATION
//#define TARGET_ROTATION

//#define PRELEARN_EACH_TIME
#define DO_FAKE_PRELEARN
 
//#define SECTOR_REWARD
//#define HOR_REWARD_SLAVA


//float directions[8] = {0,45,90,135,180,225,270,315};       // in degrees

float getSuccess(float * x,float * y,unsigned int k, float * addInfo)
{

    float target = 0;
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
#ifdef TARGET_ROTATION
            target = rot;
#else
            target = 0; 
#endif
            break;
        case POST1:
            rot = 0;
            break;
        case PRE3:
            rot = 0;
            target = targetAdapt2;
            break;
        case ADAPT2:
            rot = dirShift;
#ifdef TARGET_ROTATION
            target = targetAdapt2 + rot;
#else
            target = targetAdapt2; 
#endif
            break;
        case POST2:
            rot = 0;
            target = targetAdapt2;
            break;
        case PRELEARN:
            rot =0;
            break;
    }

    float x0,y0;

    //float A=.2;
    float targetAngle = 2*M_PI*target/360;   // in radians

    x0=xc+armReachRadius*cos(targetAngle),
    y0=yc+armReachRadius*sin(targetAngle);

    //setCBtarget(x0,y0);

    float out[2];
    moveHand(y,out,0.);
    xcur = out[0], ycur = out[1];


#if defined(ACTION_ROTATION)
    float xtmp = xcur - xc, ytmp = ycur -yc;
    float angle = 2.*M_PI/360.*rot;
    xcur = xtmp*cos(angle) - ytmp*sin(angle) + xc;
    ycur = xtmp*sin(angle) + ytmp*cos(angle) + yc;
#endif
    float sc = rewardDist+0.1;  // to be unrewarded by default
#if defined (SECTOR_REWARD)
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
#else 
    float dist0=hypot(xcur-x0,ycur-y0);
    sc = dist0;
    //addInfo[0] = -(xcur-x0);
    addInfo[0] = dist0;
#endif
    addInfo[1] = xcur;
    addInfo[2] = ycur;

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
    // flush
    unsigned char cueInd = 0;     
    for(int i=0; i<nc; i++)
        x[i] = 0.;

    if(experimentPhase == ADAPT1) 
        cueInd = 1;
    else if (experimentPhase == PRE3 || experimentPhase == POST2) 
        cueInd = 2;
    else if (experimentPhase == ADAPT2) 
        cueInd = 3;

    x[cueInd] = 1.;
    return cueInd;
}

int deg2action(float degAngle)
{
    return int(float(degAngle) / 360. * 100.);
}

void initCBdir(float degAngle)       // sets matrix update rule relative to the direction
{
    float x0=xc+armReachRadius*cos(degAngle);
    float y0=yc+armReachRadius*sin(degAngle);

    float yylast[na] = {}; 
    yylast[deg2action(degAngle)] = 1.;
    initCB(x0,y0,1.,yylast);
}

void prelearn(int n, float * addInfo)
{
    experimentPhase = PRELEARN;
    float wmmax;
#ifdef DO_FAKE_PRELEARN
    wmmax = wmmax_fake_prelearn;
    wm[0][0] = wmmax;
        int dirInd = deg2action(dirShift);
    wm[1][dirInd] = wmmax;
        dirInd = deg2action(targetAdapt2);
    wm[2][dirInd] = wmmax;
        dirInd = deg2action(targetAdapt2+dirShift);
    wm[3][dirInd] = wmmax;

    xcur = 0.2;
    ycur = 0.4;
    cout<<"Fake prelearn max weight is "<<wmmax<<endl;
#else
    setBGlearning(true);
    setCBlearning(false);

    exportInit("prelearn");
    makeTrials(n,memoryLen,addInfo,true,0,true);  // last arg is whether we do export, or not
    exportClose();
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
#else
        xcur = 0.2;
        ycur = 0.4;
#endif

        setBGlearning(false);
        setCBlearning(true);

        setCBlearning(false);

        
        exportInit("galea");

        // if we did prelearn above
        restoreWeights(true);
        //flushWeights(false); 
        //flushRpre(); // flush all data except wm (we set them from prelearn)
        float rpre = 3.;
        setRpre(&rpre);

        initCBdir(0);
        experimentPhase = PRE1;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsPre,memoryLen,addInfo[i],false,0);
        int offset = numTrialsPre;

        experimentPhase = PRE2;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsPre,memoryLen,addInfo[i],false,offset);
        offset+= numTrialsPre;

        initCBdir(dirShift);
        experimentPhase = ADAPT1;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsAdapt,memoryLen,addInfo[i],false,offset);
        offset += numTrialsAdapt;

        initCBdir(0);
        experimentPhase = POST1;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsPost,memoryLen,addInfo[i],false,offset);
        offset += numTrialsPost;



        // learn how to correct differnt action. Each next is less important than the prev
//#ifdef CB_MULTI_POINT_INIT
//#ifdef DO_FAKE_PRELEARN
//        cout<<"   multi point cb init"<<endl;
//        float coef = 1;
//        int compind = 0;
//
//        yylast[compActionIndices[compind]] = 0.9;
//        initCB(xcompass[compind],ycompass[compind],1.,yylast,coef);
//        yylast[compActionIndices[compind]] = 0.0;
//        compind++;
//
//        coef = 0.3;
//        for(; compind<4; compind++)
//        { 
//            yylast[compActionIndices[compind]] = 0.9;
//            initCB(xcompass[compind],ycompass[compind],1.,yylast,coef);
//            yylast[compActionIndices[compind]] = 0.0;
//        } 
//#else
//        initCB(x0,y0,1.);
//#endif
//#endif //CB_MULTI_POINT_INIT
        
        initCBdir(targetAdapt2);
        experimentPhase = PRE3;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsPre,memoryLen,addInfo[i],false,offset);
        offset+= numTrialsPre;

        initCBdir(targetAdapt2+dirShift);
        experimentPhase = ADAPT2;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsAdapt,memoryLen,addInfo[i],false,offset);
        offset += numTrialsAdapt;
        
        initCBdir(targetAdapt2);
        experimentPhase = POST2;
        cout<<"session num = "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(numTrialsPost,memoryLen,addInfo[i],false,offset);

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
