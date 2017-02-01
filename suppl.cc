#include "suppl.h"
#include "learn.h"

float **wmBackup, **w2Backup, **w1Backup;
float* Rpre;

// using only prev data
void movingAverageFilter(float * input, unsigned int len, unsigned int windowSize, float * output)
{
    for(unsigned int i = 0; i<len; i++)
    {
        output[i] = 0;
        unsigned int n = std::min(windowSize,i+1);
        for(unsigned int j = 0; j<n; j++)
        {
            output[i] += input[i-j]/float(n);
        }
    }
}

void initWeightNormFactor(unsigned int memoryLen)  // should be called ONLY ONCE
{
    float nf = 0.;
    for(unsigned int i = 0; i<memoryLen; i++)
    {
        nf += weightRpre(i); // when we compute it, normFactor should be = 1.
    }
    normFactor = 1/nf;
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

void flushCB()
{
    for(int i = 0; i<6;i++)
        for(int j=0;j<6;j++)
        {
            dfwx[i][j] = 0;
            dfwy[i][j] = 0;
        }
}
