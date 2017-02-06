#include "BG_model.h"
#include "motor_learning.h"
#include "suppl.h"

#define STN_IN_INDIRECT

void BG_model::activate_disease_PD()
{
    d1_ltp = 0.1;
    d2_ltp = 0.1;
    //dm_ltp = 0.;
}

void BG_model::activate_disease_HD()
{
    old_A_exp = A_exp;
    A_exp = 0;
    //old_gpi_drive = gpi_drive;
    //gpi_drive -= 0.2;
    d2activity = 0.1;
}

void BG_model::inactivate_disease_PD()
{
    d1_ltp = 1.;
    d2_ltp = 1.;
    //dm_ltp = 1.;
}

void BG_model::inactivate_disease_HD()
{
    A_exp = old_A_exp;
    //gpi_drive = old_gpi_drive;
    d2activity = 1.; 
}

float BG_model::bg_step(float **w1,float **w2,float **wm,float *x,float *y,float *expl)
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
		
		stn[i]=s(-gpe_stn*gpe[i] + stn_drive + expl[i]);
		//gpi_drive=.2;  , otherwise 1.7
        if(BGactive)
		    gpi[i]=s(-d1_gpi*d1[i]+stn_gpi*stn[i]+gpi_drive);
		    //gpi[i]=s(-d1_gpi*d1[i]+stn_gpi*stn[i]+gpi_drive+expl[i]);
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

float BG_model::do_step()
{
    bg_step(w1,w2,wm,x,y,expl);
}

void BG_model::bg_learn(float **w1,float **w2,float* x,float* y,float DA,float **wm)
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

void BG_model::learn(float DA)
{
	bg_learn(w1,w2,x,y,DA,wm);
}


void BG_model::allocMemory()
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
            wm[i][j]=0;
            w1Backup[i][j]=wm[i][j];
            w2Backup[i][j]=wm[i][j];
            wmBackup[i][j]=wm[i][j];
        }
    }
}

void BG_model::freeMemory()
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
}

void BG_model::copyWeights(float** wfrom, float ** wto)
{
    for(int i=0;i<nc;i++)
    {
        for(int j=0;j<na;j++)
        {
            wto[i][j]=wfrom[i][j];
        }
    }
}

void BG_model::backupWeights()
{
    copyWeights(wm,wmBackup);
    copyWeights(w1,w1Backup);
    copyWeights(w2,w2Backup);
}

void BG_model::restoreWeights(bool w12too)
{
    copyWeights(wmBackup,wm);
    if(w12too)
    { 
        copyWeights(w1Backup,w1);
        copyWeights(w2Backup,w2);
    }
}


void BG_model::flushWeights(bool wmToo)
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

void BG_model::trialBegin()
{
    //for(int i=0;i<na;i++) expl[i]=A_exp*( rnd()  ); // Uniform distribution in [0,1]
    for(int i=0;i<na;i++) expl[i]=A_exp*rnd(); // Uniform distribution in [0,1]

    // put random firing rates in MSN and M1 populations.
    // because trials are independent and we don't know what's there in the beginning
    for(int i=0;i<na;i++) { y[i]=Q*rnd(); d1[i]=Q*rnd(); d2[i]=Q*rnd();  }
}

BG_model::BG_model()
{
    na = MotorLearning::na;
    nc = MotorLearning::nc;

    allocMemory();

    BGactive=true; 
    d1_ltp=1., d2_ltp=1., dm_ltp=1.;

    normFactor = 1.;
    d2activity = 1.;

    old_A_exp = 0;
    old_gpi_drive = 0;

    memoryLen = 10;

    amplInitRand = 0.001;
    //Rpre_coef = 0.85;
}

BG_model::~BG_model()
{
    freeMemory();
}

void BG_model::exportBGstate(int k, float * addInfo)
{
    exporter->exportDynData(k,y,d1,d2,gpe,gpi,addInfo);
	exporter->exportWeights(k,w1,w2,wm);    
}

void BG_model::exportWeightsOnce()
{
    exporter->exportWeightsOnce(w1,w2,wm);
}

void BG_model::exportCuesState(int k)
{
    exporter->exportCuesState(k,x);
}

void BG_model::setCues(float * x_)
{
    for(int i = 0; i<nc; i++)
    {
        x[i] = x_[i];
    }
}

void BG_model::getPMC(float * y_)
{
    for(int j = 0; j<na; j++)
    {
        y_[j] = y[j];
    }
}

void BG_model::resetTrialBegin()
{
    for(int i = 0; i<nc; i++)
    {
        x[i] = 0;
    }
    for(int j = 0; j<na; j++)
    {
        y[j] = 0;
    }
}

void BG_model::inactivate()
{
    BGactive = false;
}

void BG_model::activate()
{
    BGactive = true;
}
