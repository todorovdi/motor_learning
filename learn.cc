// learn.cc
// this file is for the learning model only
// INDEPENDENT of the experiment we are modelling

//#include "learn.h"

#ifdef  BUILD_PIRON_ARM
#include "piron_arm.h"
#endif

#ifdef  BUILD_FORCE_FIELD
#include "force_field.h"
#endif

#ifdef  BUILD_GUTIERREZ
#define TWO_CUES
#include "gutierrez.h"
#endif

#ifdef  BUILD_GALEA
#include "galea.h"
#endif

#ifdef  BUILD_PIRON_TAEGYO
#include "piron_taegyo.h"
#endif

#include "suppl.h"
#include "file_export.h"
#include <time.h>

#define STN_IN_INDIRECT

float normFactor = 1.; // to be initalized in main. Used to weight learning
//float forceField = 0.;
float *gpe, *gpi, *stn;
float *d1,*d2;

float * Rpre;

float d2activity = 1.;
    
float amplInitRand = 0.001;

const int na=100; // number of actions

// init_gpi_drive=1.7,

////////////////

unsigned int memoryLen = 10;

const float EPS = 0.0000001;
bool BGactive=true; 
float d1_ltp=1., d2_ltp=1., dm_ltp=1.;


float **w1, **w2, **wm;
float **wmBackup, **w2Backup, **w1Backup;

float wcb[6][6]={},dfwx[6][6],dfwy[6][6];
float cb_learn_rate=10;

float last_y[na] = {};

bool learn_cb = false;
float x_cb_target=0., y_cb_target=0.;

// we want to make monkey touch one of the points that are good
// thus we need to output it somehow
// it interacts badly with existing experiment code
// we have to make a framework that would allow to use the different one

void initWeightNormFactor(unsigned int memoryLen)  // should be called ONLY ONCE
{
    float nf = 0.;
    for(unsigned int i = 0; i<memoryLen; i++)
    {
        nf += weightRpre(i); // when we compute it, normFactor should be = 1.
    }
    normFactor = 1/nf;
}

float bg_step(float **w1,float **w2,float **wm,float *x,float *y,float *expl)
{
	float DD1=0,DD2=0,YY=0;
	for(int j=0;j<na;j++) { DD1+=d1[j]; DD2+=d2[j]; YY+=y[j]; }
	float YYpre=YY;

	float D1[na]={},D2[na]={},Y[na]={};
	for(int i=0;i<na;i++)
	{
		D1[i]=-inh1*(DD1-d1[i]);
		D2[i]=-inh2*(DD2-d2[i]);
		Y[i]=-inhy*(YY-y[i]);
		
		for(int j=0;j<nc;j++)
		{
			D1[i]+=w1[j][i]*x[j];
			D2[i]+=w2[j][i]*x[j];
			Y[i]+=wm[j][i]*x[j];
		}
        //D2[i] = D2[i] * d2activity;

#ifndef STN_IN_INDIRECT
		D1[i]-=inh21*d2[i];
		D2[i]-=inh12*d1[i];

		Y[i]+=y_drive;

		D1[i]=s(D1[i]+y_d1*y[i]);
		D2[i]=s(D2[i]+y_d2*y[i]);

		gpe[i]=s(-d2_gpe*d2[i]+gpe_drive);
		
		//stn[i]=s(y_stn*y[i]-gpe_stn*gpe[i]);
		
        if(BGactive)
		    gpi[i]=s(-d1_gpi*d1[i]-gpe_gpi*gpe[i]+gpi_drive+expl[i]); //  stn_gpi*stn[i]);
        else
            gpi[i]=0.;
		//gpi[i]=s(-d1_gpi*d1[i]-gpe_gpi*gpe[i]+gpi_drive+expl[i]);
		
		Y[i]=s(Y[i]-gpi_y*gpi[i]);
#else
		//D1[i]-=inh21*d2[i];
		//D2[i]-=inh12*d1[i];
		D1[i]+=d1_drive;
		D2[i]+=d2_drive;

		Y[i]+=y_drive;

		D1[i]=s(D1[i]+y_d1*y[i]);
		D2[i]=s(D2[i]+y_d2*y[i]);

		gpe[i]=s(-d2_gpe*d2[i]+gpe_drive);
		
		stn[i]=s(-gpe_stn*gpe[i]+stn_drive);
		gpi_drive=.2;
		float stn_gpi=1;
        if(BGactive)
		    gpi[i]=s(-d1_gpi*d1[i]+stn_gpi*stn[i]+gpi_drive+expl[i]);
        else
            gpi[i]=0;
		
		Y[i]=s(Y[i]-gpi_y*gpi[i]);
#endif

        // if Y[i] becomes 0, the cycle hangs
	}
	
	YY=0;
	for(int j=0;j<na;j++) YY+=Y[j];
	float dt=fmax(.01,.001*YYpre/fabs(YY-YYpre));
	float mu=(1-exp(-dt/tau));

	for(int i=0;i<na;i++)
	{
		y[i]+=(Y[i]-y[i])*mu;
		d1[i]+=(D1[i]-d1[i])*mu;
		d2[i]+=(D2[i]-d2[i])*mu * d2activity;

        d2[i] = d2[i];
        //d2[i] = d2[i] * d2activity;
	}
	return dt;
}


void bg_learn(float **w1,float **w2,float* x,float* y,float DA,float **wm)
{
	for(int i=0;i<na;i++)
	{
		for(int j=0;j<nc;j++)
		{
            if(x[j]>EPS)   // modify only if we have right cue, otherwise don't loose knowledge
            {
                w1[j][i]+= d1_ltp*( lam1*DA*x[j]*d1[i]-gam1*w1[j][i]);
                w2[j][i]+= d2_ltp*( -lam2*DA*x[j]*d2[i]-gam2*w2[j][i]);
                if(w1[j][i]<0) w1[j][i]=0;
                if(w2[j][i]<0) w2[j][i]=0;
                if(w1[j][i]>W1max) w1[j][i]=W1max;
                if(w2[j][i]>W2max) w2[j][i]=W2max;
            }
		}
	}
	for(int i=0;i<na;i++) 
        for(int j=0;j<nc;j++) 
        {
            if(x[j]>EPS && BGactive)   // modify only if we have right cue, otherwise don't loose knowledge
                wm[j][i]+=dm_ltp*(mlam*x[j]*y[i] - mgam*wm[j][i]);

            if(wm[j][i]>WMmax) wm[j][i]=WMmax;
        }
}

float moveHand(float * phi, float * y, float* out)  
{
    float Y[na];
    float th=0.;
    // Y -- here comes result of the BG
    for(int i=0;i<na;i++) Y[i]=(y[i]-th)*neuron2armMult;

    //////  ACTION  /////////
    // fill phi with some values, based on wht we had in Y
    reach(phi,Y,0,wcb);
    
    float xcur=(-L1*sin(phi[0])+-L2*sin(phi[1]))+ // V*(1.-2.*rnd())*5.;   
        finalNoiseAmpl*gauss();
    float ycur=(L1*cos(phi[0])+L2*cos(phi[1]))+  // V*(1.-2.*rnd())*5.;     
        finalNoiseAmpl*gauss();

    out[0] = xcur;
    out[1] = ycur;
}

float gauss()
{
	static int flag=0;
 	static float x1, x2, w, y1, y2;

 	if(flag)
 	{
 		flag=0;
 		return y2;
 	}
 	flag=1;
 
    do 
    {
    	x1 = 2.0 * rnd() - 1.0;
        x2 = 2.0 * rnd() - 1.0;
        w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );

    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    return y1;
}

float getRpre(unsigned int k, float R, float * addInfo)  // to be called AFTER bg_learn
{
    { 
        Rpre[k]=Rpre_coef*Rpre[k]+(1-Rpre_coef)*R;
    }
    
    return Rpre[k];
}

void allocMemory()
{ 
    w1 = new float*[nc];
    w2 = new float*[nc];
    wm = new float*[nc];
    w1Backup = new float*[nc];
    w2Backup = new float*[nc];
    wmBackup = new float*[nc];
    for(int j=0;j<nc;j++)
    {
        w1[j]=new float [na];
        w2[j]=new float [na];
        wm[j]=new float [na];
        w1Backup[j]=new float [na];
        w2Backup[j]=new float [na];
        wmBackup[j]=new float [na];
    }

    gpe = new float[na], gpi = new float[na];
    stn= new float[na];
    d1= new float[na], d2= new float[na];

    for(int i=0;i<nc;i++)
    {
        for(int j=0;j<na;j++)
        {
            wm[i][j]=i==j?0:0;
            w1Backup[i][j]=wm[i][j];
            w2Backup[i][j]=wm[i][j];
            wmBackup[i][j]=wm[i][j];
        }
    }

    Rpre = new float[nc];
}

void freeMemory()
{
    for(int j=0;j<nc;j++)
    {
        delete w1[j];
        delete w2[j];
        delete wm[j];
        delete w1Backup[j];
        delete w2Backup[j];
        delete wmBackup[j];
    }
    delete w1;
    delete w2;
    delete wm;
    delete w1Backup;
    delete w2Backup;
    delete wmBackup;

    delete gpi;
    delete gpe;
    delete d1;
    delete d2;

    delete Rpre;
}

void copyWeights(float** wfrom, float ** wto)
{
    for(int i=0;i<nc;i++)
    {
        for(int j=0;j<na;j++)
        {
            wto[i][j]=wfrom[i][j];
        }
    }
}

void backupWeights()
{
    copyWeights(wm,wmBackup);
    copyWeights(w1,w1Backup);
    copyWeights(w2,w2Backup);
}

void restoreWeights(bool w12too)
{
    copyWeights(wmBackup,wm);
    if(w12too)
    { 
        copyWeights(w1Backup,w1);
        copyWeights(w2Backup,w2);
    }
}

void cblearn(float dx,float dy)
{
	for(int i=0;i<6;i++) 
        for(int j=0;j<6;j++) 
            wcb[i][j]-=cb_learn_rate*(dx*dfwx[i][j]+dy*dfwy[i][j]);
}

void initCB(float x0, float y0, float dw, float * yy)
{
    float endpt[2];
    if(yy == 0)
        yy = last_y;

    x_cb_target = x0;
    y_cb_target = y0;

	for(int i=0;i<6;i++) 
        for(int j=0;j<6;j++)
        {  
            // flush wcb
            for(int k=0;k<6;k++) 
                for(int l=0;l<6;l++) 
                    wcb[k][l]=0;

            // set only current weight nonzero
            wcb[i][j]=dw;

            float phi[4]={};
            for(int ii=0; ii<4; ii++)
                phi[ii] = initAng[ii];
            //x[0]=a; x[1]=b;
            
            // it influences update of DR which is basically some precalc * neuron activity
            moveHand(initAng,yy,endpt);

            // compute error vector
            dfwx[i][j]=(endpt[0]-x0)/dw; 
            dfwy[i][j]=(endpt[1]-y0)/dw;
        }

    // flush cb weights so that they do not influence normal movements
    // we will set them to nonzero if we do cblearn
	for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
            wcb[k][l]=0;

    learn_cb = true;
}

inline bool fzero(float t)
{
    return abs(t) < EPS;
}

void flushWeights(bool wmToo)
{
    for(int i=0;i<nc;i++)
    {
        for(int j=0;j<na;j++)
        {
//#ifdef MATCH_SLAVA
//            w1[i][j]=0.01*rnd(); 
//            w2[i][j]=0.01*rnd();
//#else
            w1[i][j]=amplInitRand*rnd(); 
            w2[i][j]=amplInitRand*rnd();
            if(wmToo)
                wm[i][j]=i==j?0:0;
//#endif
        }
    }
}

void flushRpre()
{
    for(int i=0;i<nc;i++)
    {
        Rpre[i] = 0.;
    }
}


float makeTrials(unsigned int ntrials, unsigned int memoryLen, float * addInfo, bool flushData, unsigned int indAdd, bool doExport)
{
    if(flushData)
    {
        flushWeights(true);
        flushRpre();
    }

	float x[nc]={},y[na]={};
	//float dt=.3,T=30;
    float T=20;
	

    //float sumM1freq[na]={0.,0.};
    float Rs[ntrials] = {};
    //float addInfo[ntrials]={};

//    ofstream outmp;;
//    if(indAdd == 0)
//    { 
//        outmp.open("dat_tmp");
//        ofstream wwwtmp("www_tmp");
//    }

    static float sumSuccPerCue[24] = {};
    // cycle over trials
	for(int k=indAdd;k<(ntrials+indAdd);k++)
	{
        int cueActive = turnOnCues(x);


        // should work but looks confusing
		
		float expl[na]={};
		//for(int i=0;i<na;i++) expl[i]=A_exp*( rnd()  ); // Uniform distribution in [0,1]
		for(int i=0;i<na;i++) expl[i]=A_exp*rnd(); // Uniform distribution in [0,1]

        // put random firing rates in MSN and M1 populations.
        // because trials are independent and we don't know what's there in the beginning
		for(int i=0;i<na;i++) { y[i]=Q*rnd(); d1[i]=Q*rnd(); d2[i]=Q*rnd();  }

        if(doExport)
        { 
            exportCuesState(k,x);
        }

        int nsteps = 0;
        float dt = 0;
        for(float t=0; t<T; )
        {   
            dt = bg_step(w1,w2,wm,x,y,expl); 
            nsteps++;
            t+= dt;
            if(dt >= 1.)
                break;
        };

        float addInfoItem[3];
        float sc = getSuccess(x,y,k,addInfoItem);   // here arm export happens
        float endpt_x = addInfoItem[1];
        float endpt_y = addInfoItem[2];

        float t; // is set in the following function (via a link)
        float R = getReward(sc,x,y,t);

        addInfo[k] = addInfoItem[0];

        // for Piron debug
        //sumSuccPerCue[activeCue] += ( fabs(sc- 2.)<EPS ? -1. : sc);
        //cout<<k<<" sumSucc 13 = "<<sumSuccPerCue[13]<<" and 19 = "<<sumSuccPerCue[19]<<endl;

        if(doExport)
        { 
            exportDynData(k,y,d1,d2,gpe,gpi,t,R,addInfo);
        } 

        if(doExport)
        { 
	        exportWeights(k,w1,w2,wm);    
        } 

        //rnd();  // just to follow same seed as Slava's code
		bg_learn(w1,w2,x,y,(R- Rpre[cueActive]),wm);

        if(learn_cb)
            cblearn(endpt_x-x_cb_target, endpt_y-y_cb_target);

        getRpre(cueActive,R,addInfo);   

	}
    if(doExport)
        exportWeightsOnce(w1,w2,wm);

//    if(doExport)
//        trialEndExport(sumM1freq, 0);
    for(int j=0; j<na; j++)
        last_y[j] = y[j];

    return 0;
}

int main(int argc, char** argv)
{
    clock_t start = clock();
    cout<<"Calc started, nc = "<<nc<<" na = "<<na<<" nsessions "<<nsessions<<" numTrials = "<<numTrials<<endl;
    bool presetSeed = false;
    //unsigned int seed =   1477324525;
    unsigned int seed =   time(NULL);  presetSeed = false; 
    srand(seed);
    cout<<"seed is "<<seed<<endl;
    if(presetSeed)
        cout<<"WARNING: PRESET SEED IS ACTIVE!!!!"<<endl;

    initWeightNormFactor(memoryLen);
    
    allocMemory();
    reach_init();

    runExperiment(argc,argv);

    freeMemory();

    clock_t end = clock();
    cout<<"Calc finished, clock time (in sec) passed is "<<(end-start)/CLOCKS_PER_SEC<<endl;
    return 0;
}

static float old_A_exp = 0;
static float old_gpi_drive = 0;

void activate_disease_PD()
{
    d1_ltp = 0.1;
    d2_ltp = 0.1;
    //dm_ltp = 0.;
}

void activate_disease_HD()
{
    old_A_exp = A_exp;
    A_exp = 0;
    //old_gpi_drive = gpi_drive;
    //gpi_drive -= 0.2;
    d2activity = 0.1;
}

void inactivate_disease_PD()
{
    d1_ltp = 1.;
    d2_ltp = 1.;
    //dm_ltp = 1.;
}

void inactivate_disease_HD()
{
    A_exp = old_A_exp;
    //gpi_drive = old_gpi_drive;
    d2activity = 1.; 
}

void inactivateBG()
{
    BGactive = false;
}

void activateBG()
{
    BGactive = true;
}

void setRpre(float * rpre)
{
    for(int i=0;i<nc;i++)
    {
        Rpre[i] = rpre[i];
    }
}

void setRpreMax()
{
    for(int i=0;i<nc;i++)
    {
        Rpre[i] = 3.;
    }
}
