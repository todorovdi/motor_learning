#include "force_field.h"
#include "force_field_export.h"
#include "string.h"

//#include "params_force_field.h"
#include "params_common.h"

unsigned int nsessions = 1; // article -- 15 
unsigned int prelearnNumTrials = 1; // 200 

//const float ffield = -2;
const float ffield = -2.0;

///////////////

float successDist=0.1;  // distance the hand has to reach the success to count
//float finalNoiseAmpl = 0.005; // amplitude of noise applied to the final hand position
float rewardDist=0.03;  // distance the hand has to reach the success to count
int experimentPhase=0;   

//const int baselineTrialNum = 300;
//const int forceTrialNum =    350;   // 400
//const int washoutTrialNum =  1000 - 300 - 350;  // 300

const int baselineTrialNum = 30;
const int forceTrialNum =    35;   // 400
const int washoutTrialNum =  100 - 30 - 35;  // 300

unsigned int numTrials = baselineTrialNum + forceTrialNum + washoutTrialNum; 

float xcur=0.,ycur=0.;

int nc=1; // number of cues

float wmmax_fake_prelearn = 0.7;

const char * phasesNames[] = {"PRELEARN", "BASELINE", "WITH_FORCE", "WITH_FORCE_CCW", "WASHOUT"};

#define DO_FAKE_PRELEARN
//#define DO_FULL_PRELEARN

#define CW_DO
#define CCW_DO



void getTargetPoints(float *x, float xc, float yc,float *out)
{
    // target 0 is most rewarded
    float target = 0;    // need exactly this otherwise error is more difficult to compute

    float A=.2;
    out[0]=xc+A*cos(2*M_PI*target/na),
    out[1]=yc+A*sin(2*M_PI*target/na);

    //cout<<out[0]-xc<<" "<<out[2]-yc<<"   "<<out[1]-xc<<" "<<out[3]-yc<<endl;
}

float getSuccess(float * x,float * y,unsigned int k, float * addInfo)
{

    //////////////////////////////////////
    ////////// Arm related stuff ////////
    //
    //
    float forceField = 0;
    switch(experimentPhase)
    {
        case BASELINE:
            forceField = 0;
            break;
        case WITH_FORCE:
            forceField = ffield;
            break;
        case WITH_FORCE_CCW:
            forceField = -ffield;
            break;
        case WASHOUT:
            forceField = 0;
            break;
    }

    float x0,y0;
    float tmp[4];
    getTargetPoints(x,xc,yc,tmp);
    x0 = tmp[0];
    y0 = tmp[1];
    
    //////  ACTION  /////////
    float out[2];
    moveHand(y,out,forceField);
    xcur = out[0], ycur = out[1];

    //cout<<"x0 and y0 are "<<x0<<" "<<x1<<" not centered are "<<x0-xc<<" "<<y0-yc<<endl;
    //cout<<"cur "<<xcur<<" "<<ycur<<" not centered are "<<xcur-xc<<" "<<ycur-yc<<endl<<endl;

    float dist0=hypot(xcur-x0,ycur-y0);
    float d=dist0;

   // // first 700 trials it is 0.25, then if <900 is 0.75 else 0.25 
   // float P=k<700?.25:(k<900?.75:.25); // switch of target, then return back

   // // if random number < P, the reward=3 is given if d0 > 0.1
   // float R=rnd()<P?(d0>.1?0:3):(d1>.1?0:3);
   // if(doExport)
   

   float sc = dist0;
   addInfo[0] = (xcur-x0);
   addInfo[1] = xcur;
   addInfo[2] = ycur;

   exportArm(k,xcur,ycur,x0,y0,xc,yc,*addInfo);

//    if( dist0 < successDist  )
//        sc = 1.;
    return sc;                  // return just dist
}

float getReward(float sc, float * x,float * y, float& t)  
{
    float R = 0;

    if( sc < rewardDist)
    {
        R = 3;
    }

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

    xcur = 0.2;
    ycur = 0.4;

    float wmmax;
#if defined(DO_HALF_PRELEARN) || defined(DO_FULL_PRELEARN)
    setBGlearning(true);
    setCBlearning(false);

    makeTrials(n,memoryLen,addInfo,true,0,true);  // last arg is whether we do export, or not
    //makeTrials(n,memoryLen,addInfo,true,0,false);
    //
    // max we get with prelearning = 0.07
    // compute the maximum wm weight we get via prelearning, should be around 0.07
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

#if defined DO_FAKE_PRELEARN
    wmmax = wmmax_fake_prelearn;
    wm[0][0] = wmmax;
    cout<<"Fake prelearn max weight is "<<wmmax<<endl;
#endif
#if defined(DO_HALF_PRELEARN)
    cout<<"half prelearn is not coded yet"<<endl;
    // artificial prelearn
#endif

}

void extractErrors(float** addInfo, float* errorsOut, int indAdd, int n)
{
    for(int j =0;j<n;j++)
    { 
        errorsOut[j] = 0.;
    }

    int bins[] = {5,2,5,2,5};
    int placesBinChangeInc[] = {baselineTrialNum,10,forceTrialNum-10,10,washoutTrialNum-10};

//    for(int i =0;i<nsessions;i++)
//    {
//        int ulim = 0;
//        int ubeg = 0;
//        for(int bind = 0; bind<5;bind++)
//        { 
//            ulim += placesBinChangeInc[bind];
//            for(int j =ubeg+bins[bind]-1;j<ulim;j+=bins[bind]) 
//            { 
//                for(int k=0;k<bins[bind] && (j-k>=0);k++)
//                { 
//                    errorsOut[j] += addInfo[i][j-k] / float(bins[bind]);
//                //    cout<<j<<endl;
//                }
//                for(int k=1;k<bins[bind] && (j-k>=0);k++)
//                {
//                    errorsOut[j-k] += errorsOut[j];
//                //    cout<<"  "<<j-k<<endl;
//                }
//            }
//            ubeg += placesBinChangeInc[bind];
//        }
//    }
//
//    for(int j=0;j<numTrials;j++)
//    {
//        errorsOut[j] /= float(nsessions);
//    }


    for(int i = 0;i<nsessions; i++)
    {
        for(int j=0;j<numTrials;j++)
        {
            errorsOut[j] += addInfo[i][j] / float(nsessions);
        }
    }

    // TODO: add binsize 5 (and 2 in special places)

    // normalize by number of sessions
    //movingAverageFilter(successRatesAver, n, 10, successRatesOut);
}

// TODO: make global prelarn weights out of sessions

void runOneCurlDir(int midPhase, const char * prefix)
{
//    float * addInfo_baseline[nsessions];  
//    float * addInfo_force[nsessions];  
//    float * addInfo_washout[nsessions];  
    float * addInfo[nsessions];  

    for(int i=0;i<nsessions;i++)
    {
        addInfo[i] = new float[numTrials];
//        addInfo_baseline[i] = new float[numTrials];
//        addInfo_force[i] = new float[numTrials];
//        addInfo_washout[i] = new float[numTrials];
    }
    //float * Rs = new float[numTrials];  

    // here they have meaning of final (averged and filtered) success rates

    float successTemp[prelearnNumTrials]; //to be used in prelearn
    cout<<"num prelearn trials "<<prelearnNumTrials<<endl;

    prelearn(prelearnNumTrials, successTemp);
    backupWeights();


    float x0 = xcur, y0 = ycur;  // set them to be last reached pts. Works only with real prelearning

    float yylast[na] = {}; 
#ifdef DO_FAKE_PRELEARN
    yylast[0] = 1;
    initCB(x0,y0,1.,yylast);
#else
    initCB(x0,y0,1.);
#endif


    setBGlearning(false);
    setCBlearning(true);

    for(int i=0;i<nsessions;i++)
    {
        restoreWeights();
        flushWeights(false); flushRpre(); // flush all data except wm (we set them from prelearn)
        /////////////////////////////////////////
        /////////////////////////////////////////
        /////////////////////////////////////////
        
        exportInit(prefix);

        experimentPhase = BASELINE;
        cout<<"nsession is "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(baselineTrialNum,memoryLen,addInfo[i],false,0);

        //foutPerTrial<<"success rate NC is "<<lastSR_NC<<endl;
        //exportClose();

        //////////////////////////////////////////
        //////////////////////////////////////////
        //////////////////////////////////////////
        
  //      x0 = 0.;
  //      y0 = 0.6;
  //      yylast[24] = 0.9;
  //      initCB(x0,y0,1.,yylast);

        //exportInit("force");
        experimentPhase = midPhase;
        cout<<"nsession is "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        int offset = baselineTrialNum;
        makeTrials(forceTrialNum,memoryLen,addInfo[i],false,offset);

        //foutPerTrial<<"success rate NC is "<<lastSR_NC<<endl;
        //exportClose();


        //////////////////////////////////////////
        //////////////////////////////////////////
        //////////////////////////////////////////
        
  //      x0 = 0.2;
  //      y0 = 0.4;
  //      yylast[0] = 0.9;
  //      initCB(x0,y0,1.,yylast);

        //exportInit("washout");
        experimentPhase = WASHOUT;
        offset += forceTrialNum;
        cout<<"nsession is "<<i<<"  experimentPhase is "<<phasesNames[experimentPhase]<<endl;
        makeTrials(washoutTrialNum,memoryLen,addInfo[i],false,offset);

        //foutPerTrial<<"success rate NC is "<<lastSR_NC<<endl;
        exportClose();
    }

    float errors[numTrials] = {};
    //float errors_baseline = {};
    //float errors_force = {};
    //float errors_washout = {};
//
    extractErrors(addInfo, errors, 0, numTrials);
    //extractErrors(addInfo, errors_baseline, 0);
    //extractRates(addInfo, errors_force   , baselineTrialNum);
    //extractRates(addInfo, errors_washout , baselineTrialNum+forceTrialNum);
//
    char fname[256] = "./output_force_field/";
    strcat(strcat(fname,prefix), "_errors.dat");
    foutErr.open(fname);
    for(int j =0;j<numTrials;j++)
    {
        foutErr<<j<<"\t"<<errors[j]<<endl;
    }
    foutErr.close();

    for(int i=0;i<nsessions;i++)
    {
        delete addInfo[i];
    }
    //delete Rs;
}

void runExperiment(int argc, char** argv)
{
#ifdef CW_DO
    cout<<"Start CW phase"<<endl;
    runOneCurlDir(WITH_FORCE,"CW");
#endif

#ifdef CCW_DO
    cout<<"Start CCW phase"<<endl;
    runOneCurlDir(WITH_FORCE_CCW,"CCW");
#endif
}
 
