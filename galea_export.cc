#include "galea_export.h"

#include "string.h"

ofstream foutVarDyn2;
ofstream foutWeights2;
ofstream foutWeightsOnce;
ofstream foutArm;
ofstream foutErr;

void exportInit(const char * prefix)   // prefix = "RC" for example
{
    char dir[]="./output_galea/";
    char strtmp[256]={};
    strcpy(strtmp,dir);
    foutPerTrial.open( strcat(strcat(strtmp,prefix),"_output.txt") );
    strcpy(strtmp,dir);
    foutVarDyn.open  ( strcat(strcat(strtmp,prefix),"_var_dyn.dat") );
    strcpy(strtmp,dir);
    foutWeights.open ( strcat(strcat(strtmp,prefix),"_weights.dat") );

    strcpy(strtmp,dir);
    foutVarDyn2.open    (strcat(strcat(strtmp,prefix),"_var_dyn2.dat" ) );
    strcpy(strtmp,dir);
    foutWeights2.open   (strcat(strcat(strtmp,prefix),"_weights2.dat" ) );
    strcpy(strtmp,dir);
    foutWeightsOnce.open(strcat(strcat(strtmp,prefix),"_weights_once.dat" )  );
    strcpy(strtmp,dir);
    foutArm.open(       strcat(strcat(strtmp,prefix),"_arm.dat"      ) );
}

void exportClose()
{
    foutPerTrial.close();
    foutVarDyn.close();
    foutVarDyn2.close();
    foutWeights.close();
    foutWeights2.close();
    foutWeightsOnce.close();
    foutArm.close();
}

void exportDynData(unsigned int trialNum,float *y,float *d1,float *d2,float * gpe,float *gpi,float t,float reward, float* addInfo)
{
    // first all M1s then all d1s and so on
    //foutVarDyn2<<trialNum<<'\t';  // would spoil existing graphing procs probably
    for(int i=0;i<na;i++) foutVarDyn2<<y[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn2<<d1[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn2<<d2[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn2<<gpe[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn2<<gpi[i]<<'\t';
    foutVarDyn2<<endl;
}



void exportWeights(unsigned int trialNum,float ** w1,float ** w2,float **wm)
{ 
    //foutWeights2<<trialNum<<'\t';
    for(int i=0;i<na;i++) foutWeights2<<w1[0][i]<<'\t';
    for(int i=0;i<na;i++) foutWeights2<<w2[0][i]<<'\t';
    for(int i=0;i<na;i++) foutWeights2<<wm[0][i]<<'\t';
    foutWeights2<<endl;

    //for(int i=0;i<na;i++) foutWeights2<<w1[0][i]<<'\t';
    //for(int i=0;i<na;i++) foutWeights2<<w2[0][i]<<'\t';
}

void exportWeightsOnce(float ** w1,float ** w2,float **wm)
{
    for(int i=0;i<na;i++) 
    {  
        for(int j=0;j<nc;j++) 
            foutWeightsOnce<<w1[j][i]<<'\t';
        foutWeightsOnce<<endl;
    }	
    foutWeightsOnce<<endl<<endl;	

    for(int i=0;i<na;i++)
    { 
        for(int j=0;j<nc;j++) 
            foutWeightsOnce<<w2[j][i]<<'\t';
        foutWeightsOnce<<endl;	
    }
    foutWeightsOnce<<endl<<endl;	
    
    for(int i=0;i<na;i++)
    { 
        for(int j=0;j<nc;j++) 
            foutWeightsOnce<<wm[j][i]<<'\t';
        foutWeightsOnce<<endl;	
    }
    foutWeightsOnce<<endl<<endl;	
	//for(int j=0;j<nc;j++)
	//{
	//	for(int i=0;i<na;i++)
	//	{
	//		foutWeightsOnce << w1[j][i] << '\t' << w2[j][i] << '\t' <<wm[j][i]<< endl;
	//	}
	//}
}

void exportArm(unsigned int trialNum,float xcur,float ycur, float x0, float y0, float xc, float yc, float * addInfo)
{
    float d = addInfo[0];
    unsigned int k = trialNum;
    foutArm<<k<<'\t'       //1
           <<xcur<<'\t'    //2
           <<ycur<<'\t'    //3
           <<x0  <<'\t'    //4   // most rewarded target coords
           <<y0  <<'\t'    //5
           <<d   <<'\t'    //6
           <<xc  <<'\t'    //7
           <<yc  <<'\t'    //8
           <<endl;

  
	//	out<<xcur<<'\t'<<ycur<<'\t'<<x0<<'\t'<<y0<<'\t'<<d<<'\t'<<xc<<'\t'<<yc;
        //<<'\t'<<sr<<endl;
}
