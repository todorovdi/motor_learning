#include "experiment_piron_learn.h"

///////////////////////////////////////////
//// experiment-indepent constants
//// model tuning
///////////////////////////////////////////

const int nc=6*2+6*2; // number of cues
const int na=2+2; // number of actions

float gpe[na], gpi[na];
float normFactor = 1.; // to be initalized in main. Used to weight learning
float expCoefRpre = 0.1;
float d1[na]={},d2[na]={};

const float W1max=3;
const float W2max=3; // No max for wm?

const float gam1=.005;
const float gam2=.005;

const float lam1=0.5; // learning rate
const float lam2=0.3; // learning rate

const float mgam=.002;
const float mlam=.002;

const float inh1=0.; // lateral inhibition in D1
const float inh2=0.; // lateral inhibition in D2
const float inh21=.0; // inhibition of D1 by D2
const float inh12=.0; // inhibition of D2 by D1
const float inhy=1.95; // lateral inhibition in M1  

const float A_exp = 1.7; // exploratory constant [0, 1]

const float y_drive=.88;
const float y_d1=2, y_d2=2;
const float d1_gpi=2, init_gpi_drive=2.35, gpi_y=1.5;
float gpi_drive = init_gpi_drive - A_exp; // modify gpi_drive
const float d2_gpe=2, gpe_drive=2, gpe_gpi=1.5;
const float tau=1; // time constant for Wilson-Cowan
                            
unsigned int memoryLen = 20;
unsigned int nsessions = 1; //20 in Piron
unsigned int numTrials = 240; // total, should be a multiple of 2*trialBlockLen

///////////////

unsigned int prelearnNumTrials = 500;  // ? in Piron article they only tell about number of months they used
unsigned int trialBlockLen = 10;
unsigned int numTrialsRC = numTrials/2; 
unsigned int numTrialsNC = numTrials/2; 



enum experimentPhases {PRELEARN, SHOW_RC, SHOW_NC, SHOW_RC_NOBG, SHOW_NC_NOBG};
unsigned char experimentPhase=0;   // 0 means prelearning, 1 means showing RC, 2 means showing NC

enum directions_enum {TOP=0, BOTTOM=1, LEFT=2, RIGHT=3};
// cues description, distr of a pair of cues on diamond-shape four buttons
// 0-5 vert, hor, north-east, south-east, south-west, north-west  (clocwise)
// 6-11 same with reverted orientation
// 12-17 like 1-6 but new shape/color pair (NC)
// 18-23 same with reverted orientation    (NC)
//
// action description -- touch top/bottom/left/right
// in 0-5 we always reward leftmost or topmost 
// bottom is better then right,  left is better then top (in non-inverted situation)
// !note that this order is for convinience of initialization of the array elements only
char succAction[nc][na] = {};
const unsigned char cueRCnum = 6*2;
const unsigned char cueNCnum = 6*2;

int getOppositeAction(int j)
{
    int r = 0;
    switch(j)
    {
        case TOP:
            r = BOTTOM;
        case BOTTOM:
            r = TOP;
        case LEFT:
            r = RIGHT;
        case RIGHT:
            r = LEFT;
    }
    return r;
}

void initSuccessTable()
{
    succAction[0 ][TOP] = 1;  // vertical 1 type RC, touch top
    succAction[6 ][TOP] = 0;  // vertical 2 type RC, touch top
    succAction[12][TOP] = 1;  // vertical 1 type NC, touch top
    succAction[18][TOP] = 0;  // vertical 2 type NC, touch top

    succAction[0 ][BOTTOM] = 0;  // vertical 1 type RC,  touch bottom
    succAction[6 ][BOTTOM] = 1;  // vertical 2 type RC,  touch bottom
    succAction[12][BOTTOM] = 0;  // vertical 1 type NC, touch bottom
    succAction[18][BOTTOM] = 1;  // vertical 2 type NC, touch bottom

    succAction[1 ][LEFT] = 1;  // horizontal 1 type RC,  touch left
    succAction[7 ][LEFT] = 0;  // horizontal 2 type RC,  touch left
    succAction[13][LEFT] = 1;  // horizontal 1 type NC,  touch left
    succAction[19][LEFT] = 0;  // horizontal 2 type NC,  touch left
    
    succAction[1 ][RIGHT] = 0;  // horizontal 1 type RC,  touch right
    succAction[7 ][RIGHT] = 1;  // horizontal 2 type RC,  touch right
    succAction[13][RIGHT] = 0;  // horizontal 1 type NC,  touch right
    succAction[19][RIGHT] = 1;  // horizontal 2 type NC,  touch right

    // -------------

    succAction[2 ][TOP] = 1;  // North-East 1 type RC,  touch top
    succAction[8 ][TOP] = 0;  // North-East 2 type RC,  touch top
    succAction[14][TOP] = 1;  // North-East 1 type NC,  touch top
    succAction[20][TOP] = 0;  // North-East 2 type NC,  touch top

    succAction[2 ][RIGHT] = 0;  // North-East 1 type RC,  touch right
    succAction[8 ][RIGHT] = 1;  // North-East 2 type RC,  touch right
    succAction[14][RIGHT] = 0;  // North-East 1 type NC,  touch right
    succAction[20][RIGHT] = 1;  // North-East 2 type NC,  touch right

    succAction[3 ][BOTTOM] = 1;  // South-East 1 type RC,  touch bottom
    succAction[9 ][BOTTOM] = 0;  // South-East 2 type RC,  touch bottom
    succAction[15][BOTTOM] = 1;  // South-East 1 type NC,  touch bottom
    succAction[21][BOTTOM] = 0;  // South-East 2 type NC,  touch bottom

    succAction[3 ][RIGHT] = 0;  // South-East 1 type RC,  touch right
    succAction[9 ][RIGHT] = 1;  // South-East 2 type RC,  touch right
    succAction[15][RIGHT] = 0;  // South-East 1 type NC,  touch right
    succAction[21][RIGHT] = 1;  // South-East 2 type NC,  touch right

    succAction[4 ][LEFT] = 1;  // South-West 1 type RC,  touch left
    succAction[10][LEFT] = 0;  // South-West 2 type RC,  touch left
    succAction[16][LEFT] = 1;  // South-West 1 type NC,  touch left
    succAction[22][LEFT] = 0;  // South-West 2 type NC,  touch left

    succAction[4 ][BOTTOM] = 0;  // South-West 1 type RC,  touch bottom
    succAction[10][BOTTOM] = 1;  // South-West 2 type RC,  touch bottom
    succAction[16][BOTTOM] = 0;  // South-West 1 type NC,  touch bottom
    succAction[22][BOTTOM] = 1;  // South-West 2 type NC,  touch bottom

    succAction[5 ][LEFT] = 1;  // South-West 1 type RC,  touch left
    succAction[11][LEFT] = 0;  // South-West 2 type RC,  touch left
    succAction[17][LEFT] = 1;  // South-West 1 type NC,  touch left
    succAction[23][LEFT] = 0;  // South-West 2 type NC,  touch left

    succAction[5 ][TOP] = 0;  // South-West 1 type RC,  touch top
    succAction[11][TOP] = 1;  // South-West 2 type RC,  touch top
    succAction[17][TOP] = 0;  // South-West 1 type NC,  touch top
    succAction[23][TOP] = 1;  // South-West 2 type NC,  touch top
}

float getSuccess(float * x,float * y)
{
    float sc = 0;
    if(experimentPhase == PRELEARN || experimentPhase == SHOW_NC || experimentPhase == SHOW_NC_NOBG)
    {
        for(int i = 0; i<cueRCnum; i++)
        {
            if (abs(float(x[i] - 1.)) < 0.00001)
            { 
                for(int j = 0; j<na; j++)
                {                      
                    char oj = getOppositeAction(j);
                    if( succAction[i][j] &&   y[j]  > firingRateTreshold  )
                        // if we select right action
                    {
                        sc = 1;
                        return sc;
                    }
                    if( succAction[i][oj] &&  y[oj]  > firingRateTreshold ) 
                        // if we select opposite action
                    {
                        sc = 2;
                        return sc;
                    }
                }
            }
        }
    }
    if(experimentPhase == SHOW_RC || experimentPhase == SHOW_RC_NOBG )
    {
        for(int i = cueRCnum+1; i<nc; i++)
        {
            if (abs(x[i] - 1.) < 0.00001)
            { 
                for(int j = 0; j<na; j++)
                {                      
                    int oj = getOppositeAction(j);
                    if( succAction[i][j] &&   y[j]  > firingRateTreshold  )
                        // if we select right action
                    {
                        sc = 1;
                        return sc;
                    }
                    if( succAction[i][oj] &&  y[oj]  > firingRateTreshold ) 
                        // if we select opposite action
                    {
                        sc = 2;
                        return sc;
                    }
                }
            }
        }
    }
    return sc;
}

float getReward(float sc, float * x,float * y, float& t)  
{
    float R = 0;
    t = rnd();

    //if(experimentPhase == PRELEARN || experimentPhase == SHOW_RC || experimentPhase == SHOW_RC_NOBG)
    { 
        if( abs(sc-1.) < EPS && t <= probReward)
        {
            R = 1;
        }
        else if ( abs(sc-2.) < EPS && t > probReward)  // but NOT if sc = 0
        {
            R = 1;
        }
    }
    return R;
}

void turnOnCues(float * x)
{
    //static char cueInd;
    //static unsigned int trialNumber=0;
    //static bool needUpdateCueInd = true;
    
    // zero all the cues
    for(int i = 0; i<nc; i++)
    {
        x[i] = 0.;
    }
    if(experimentPhase == PRELEARN || experimentPhase == SHOW_RC || experimentPhase == SHOW_RC_NOBG)
    {
        char cueInd = rand() % cueRCnum;     // here we use that suppose that first curRCnum cues correspond to RC (and others to NC)
        x[cueInd] = 1;
    }
    if(experimentPhase == SHOW_NC || experimentPhase == SHOW_NC_NOBG)
    {
        char cueInd = cueRCnum + rand() % cueNCnum;     
        x[cueInd] = 1;
    }

    //trialNumber++;
}

float getRpre(unsigned int k, float * Rs, float * successHist)  
{
    float Rpre = 0.;
    // set SUMMED reward prediction 
    //float normFactor = 1/min(memoryLen, k+1);
    for(int i=0;i<memoryLen && i<=k;i++)
        Rpre += weightRpre(i) * Rs[k-i];
    //Rpre = Rpre*normFactor;
    return Rpre;
}

void prelearn(int n, float * successes)
{
    experimentPhase = PRELEARN;
    makeTrials(n,memoryLen,successes,true,0,false);
}

void extractRates(float** successes, float* successRatesOut, int indAdd)
{
    float successRates[nsessions][numTrialsRC] = {};
    float successRatesAver[numTrialsRC] = {};

    for(int i =0;i<nsessions;i++)
    {
        successRates[i][0] = successes[i][0+indAdd];
        for(int j=0; j<numTrials; j+=trialBlockLen*2)
        {
            for(int k = 1; k<trialBlockLen; k++)
            { 
                successRates[i][j/2+k] = successRates[i][j/2+k-1] + successes[i][j+k+indAdd];
            }
        }
        for(int j =0;j<numTrialsRC;j++)
        {
            successRates[i][j] = successRates[i][j]/(j+1);
            successRatesAver[j] += successRates[i][j];
        }
    }

    // normalize by number of sessions
    for(int j =0;j<numTrialsRC;j++)
    {
        successRatesAver[j] = successRatesAver[j] / nsessions;
    }
    movingAverageFilter(successRatesAver, numTrialsRC, 10, successRatesOut);
}

void runExperiment()
{
    initSuccessTable();

    // array containing whether j trial got success at i session 
    // I would be happy to do just a 2D array but C++ can not
    // convert multidim array to float**, which prevents passing it to
    // a function. And I can't make it global as I do 
    // constant extern in // a different .cc file and it is 
    // not recognized outside functions in this file
    float * successes[nsessions];  
    float * successes_NOBG[nsessions];  

    for(int i=0;i<nsessions;i++)
    {
        successes[i] = new float[numTrials];
        successes_NOBG[i] = new float[numTrials];
    }

    // here they have meaning of final (averged and filtered) success rates

    float averRate_RC = 0.;
    float averRate_NC = 0.;
    float lastSR_RC = 0.;
    float lastSR_NC = 0.;

    float successTemp[prelearnNumTrials]; //to be used in prelearn

    for(int i=0;i<nsessions;i++)
    {
        foutPerTrial.open("./output_piron/output.txt", std::ofstream::out | std::ofstream::trunc );
        foutVarDyn.open("./output_piron/var_dyn.dat", std::ofstream::out | std::ofstream::trunc);
        foutWeights.open("./output_piron/weights.dat", std::ofstream::out | std::ofstream::trunc);

//        ofstream fout("probSucess.txt");
//        fout<<"memoryLen is "<<memoryLen<<endl;
//
        //cout<<i<<" calc started"<<endl;
        prelearn(prelearnNumTrials, successTemp);
        for(int j=0; j<numTrials; j+=trialBlockLen*2)
        { 
            unsigned int offset;
            unsigned int indAdd;

            offset = j;//*sizeof(successRatesRC[0][0])*j;
            indAdd = j;
            experimentPhase = SHOW_RC;
            lastSR_RC = makeTrials(trialBlockLen,memoryLen,successes[i]+offset,false,indAdd);

            //cout<<numTrials/(2*trialBlockLen)<<" "<<*(successes[i]+offset)<<endl;

            offset = j+trialBlockLen;//*sizeof(successRatesNC[0][0])*j;
            indAdd = j+trialBlockLen;
            experimentPhase = SHOW_NC;
            lastSR_NC = makeTrials(trialBlockLen,memoryLen,successes[i]+offset,false,indAdd);
        }

        lastSR_RC /= numTrialsRC;
        lastSR_NC /= numTrialsRC;
        foutPerTrial<<"success rate RC is "<<lastSR_RC<<endl;
        foutPerTrial<<"success rate NC is "<<lastSR_NC<<endl;

        foutPerTrial.close();
        foutVarDyn.close();
        foutWeights.close();

        foutPerTrial.open("./output_piron/output_NOBG.txt", std::ofstream::out | std::ofstream::trunc );
        foutVarDyn.open("./output_piron/var_dyn_NOBG.dat", std::ofstream::out | std::ofstream::trunc);
        foutWeights.open("./output_piron/weights_NOBG.dat", std::ofstream::out | std::ofstream::trunc);

        prelearn(prelearnNumTrials, successTemp);
        inactivateBG();
        for(int j=0; j<numTrials; j+=trialBlockLen*2)
        { 
            unsigned int offset;
            unsigned int indAdd;

            offset = j;//*sizeof(successRatesRC[0][0])*j;
            indAdd = j;
            experimentPhase = SHOW_RC_NOBG;
            lastSR_RC = makeTrials(trialBlockLen,memoryLen,successes_NOBG[i]+offset,false,indAdd);

            offset = j+trialBlockLen;//*sizeof(successRatesNC[0][0])*j;
            indAdd = j+trialBlockLen;
            experimentPhase = SHOW_NC_NOBG;
            lastSR_NC = makeTrials(trialBlockLen,memoryLen,successes_NOBG[i]+offset,false,indAdd);
        }

        foutPerTrial.close();
        foutVarDyn.close();
        foutWeights.close();
        //cout<<i<<" calc finished"<<endl;

        //cout<<endl;

    }
//    fout<<"aver suceesRate is "<<averRate/nsessions<<endl;


    // make successRates array contain indeed success rates (rather than success sums)
    // and compute averaged (over sessions) successRate 
//    for(int i =0;i<nsessions;i++)
//    {
//        // make successRates continan success sums
//        successRatesRC[i][0] = successes[i][0];
//        successRatesNC[i][0] = successes[i][trialBlockLen];
//        successRatesRC_NOBG[i][0] = successes_NOBG[i][0];
//        successRatesNC_NOBG[i][0] = successes_NOBG[i][trialBlockLen];
//        for(int j=0; j<numTrials/(2*trialBlockLen); j+=trialBlockLen*2)
//        { 
//            for(int k=1;k<trialBlockLen;k++)
//            {
//                successRatesRC[i][j/2+k] = successRatesRC[i][j/2+k-1] + successes[i][j+k];
//                successRatesNC[i][j/2+k] = successRatesNC[i][j/2+k-1] + successes[i][j+trialBlockLen+k];
//                successRatesRC_NOBG[i][j/2+k] = successRatesAverRC_NOBG[i][j/2+k-1] + successes_NOBG[i][j+k];
//                successRatesNC_NOBG[i][j/2+k] = successRatesNC_NOBG[i][j/2+k-1] + successes_NOBG[i][j+trialBlockLen+k];
//                //cout<<successRates[i][j]<<" ";
//            }
//        }
//
//        for(int j =0;j<numTrialsRC;j++)
//        {
//            successRatesRC[i][j] = successRatesRC[i][j]/(j+1);
//            successRatesAverRC[j] += successRatesRC[i][j];
//
//            successRatesRC_NOBG[i][j] = successRatesRC_NOBG[i][j]/(j+1);
//            successRatesAverRC_NOBG[j] += successRatesRC_NOBG[i][j];
//            //cout<<j<<" "<<successRatesAver[j]<<endl;
//        }
//        for(int j =0;j<numTrialsRC;j++)
//        {
//            successRatesNC[i][j] = successRatesNC[i][j]/(j+1);
//            successRatesAverNC[j] += successRatesNC[i][j];
//            
//            successRatesNC_NOBG[i][j] = successRatesNC_NOBG[i][j]/(j+1);
//            successRatesAverNC_NOBG[j] += successRatesNC_NOBG[i][j];
//            //cout<<j<<" "<<successRatesAver[j]<<endl;
//        }
//        //cout<<endl;
//    }
//
//    // normalize by number of sessions
//    for(int j =0;j<numTrialsRC;j++)
//    {
//        successRatesAverRC[j] = successRatesAverRC[j] / nsessions;
//    }

    float successRatesRC     [numTrialsRC] = {};
    float successRatesNC     [numTrialsNC] = {};
    float successRatesRC_NOBG[numTrialsRC] = {};
    float successRatesNC_NOBG[numTrialsNC] = {};

    extractRates(successes, successRatesRC, 0);
    extractRates(successes, successRatesNC, trialBlockLen);
    extractRates(successes_NOBG, successRatesRC_NOBG, 0);
    extractRates(successes_NOBG, successRatesNC_NOBG, trialBlockLen);

    foutPlotSucc.open("./output_piron/probSuccess.dat", std::ofstream::out | std::ofstream::trunc);
    for(int j =0;j<numTrialsRC;j++)
    {
        foutPlotSucc<<j<<" "<<successRatesRC[j]<<" "<<
            successRatesNC[j]<<" "<<successRatesRC_NOBG[j]<<" "<<successRatesNC_NOBG[j]<<endl;
    }

}

/* Piron: success rate -- proportion of trials monkey set the optimal target
 *
 A session consisted of at least 250 trials broken up into alternate blocks of 10 trials in routine (top) or novelty (bottom) conditions

In the novelty condition (bottom), the cues (NC1 and NC2) have the same reward probabilities (P 0.75 and P 0.25, respectively), 
but the pairs are changed (new shape and colors) for each session.

A total of 20 sessions (10 for each monkey) with
saline injections (saline) and 20 (10 for each monkey)
with muscimol injections (muscimol) were performed.

In each trial, 2 cues were displayed simultaneously in 2 of 4 
randomly chosen possible positions on the screen.
  so position changes EVERY TRIAL
 
Two experimental conditions were alternated 
in blocks of 10 trials: the
 Routine Condition (RC) and the Novelty Condition (NC). 
 In the RC, the animals had been trained (during 8 months for monkey Z and 12 months for monkey
F) on the 2 cues used (RC1 and RC2). 
Each cue had a fixed reward probability (P RC1 5 .75 and P RC2 5 .25).
The cue symbol and probability remained unchanged during a session and between sessions. 

In the NC, 2 new cues were presented (NC1 and NC2). 
Each cue had a fixed probability of reward (P NC1 5 .75 and P NC2 5 .25). 
The cue symbol and probability remained the same during a session, but they changed between
sessions. 
Once the cues were shown, the monkeys had a random duration time window (0.5-1.5 s) to press the button associated with 1 cue. 
It moves the cursor over the chosen cue and they have to maintain the position for 0.5 s to 1.5 s. 

After this delay, the monkeys were rewarded (0.3 ml of water) or not 
according to the reward probability of the chosen target. 

An end-of-trial signal corresponding to the disappearance
of the cursor was given, indicating to the monkeys
that the trial was finished and they could start a new
trial after an intertrial interval between 0.5 s and 1.5 s.

graphs from Piron:
Mean success rate for both monkeys across successive trials. 
The curve is smoothed using a moving average filter of 10 consecutive trials.
*/
