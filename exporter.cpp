#include "exporter.h"

void Exporter::init(string suffix_,string dir_)   // prefix = "RC" for example
{
    suffix = suffix_;
    dir = dir_;
}

void Exporter::exportInit(string prefix,string suffix,string begPut)   // prefix = "RC" for example
{
    foutPerTrial.open( dir+prefix+string("_output")+suffix+string(".dat")) ;
    foutVarDyn.open  ( dir+prefix+string("_var_dyn")+suffix+string(".dat") );
    foutVarDyn2.open  ( dir+prefix+string("_var_dyn2")+suffix+string(".dat") );
    foutWeights.open  ( dir+prefix+string("_weights")+suffix+string(".dat") );
    foutWeights2.open  ( dir+prefix+string("_weights2")+suffix+string(".dat") );
    foutWeightsOnce.open  ( dir+prefix+string("_weights_once")+suffix+string(".dat") );
    foutArm.open  ( dir+prefix+string("_arm")+suffix+string(".dat") );

    foutArm<<begPut;
}

void Exporter::exportClose()

{
    foutPerTrial.close();
    foutVarDyn.close();
    foutVarDyn2.close();
    foutWeights.close();
    foutWeights2.close();
    foutWeightsOnce.close();
    foutArm.close();
}
void Exporter::exportDynDataStd(unsigned int k, float *y,float *d1,float *d2,float * gpe,float *gpi,float t,float R, float sr)
{
    foutVarDyn<<k<<'\t';
    for(int i=0;i<na;i++) foutVarDyn<<y[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn<<d1[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn<<d2[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn<<gpe[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn<<gpi[i]<<'\t';
    foutVarDyn<<endl;
}

void Exporter::exportWeightsStd(unsigned int k, float ** w1,float ** w2,float **wm)
{
    for(int i=0;i<na;i++) 
    {  
        for(int j=0;j<nc;j++) 
            foutWeights<<w1[j][i]<<'\t';
        foutWeights<<endl;
    }	
    foutWeights<<endl<<endl;	

    for(int i=0;i<na;i++)
    { 
        for(int j=0;j<nc;j++) 
            foutWeights<<w2[j][i]<<'\t';
        foutWeights<<endl;	
    }
    foutWeights<<endl<<endl;	
    
    for(int i=0;i<na;i++)
    { 
        for(int j=0;j<nc;j++) 
            foutWeights<<wm[j][i]<<'\t';
        foutWeights<<endl;	
    }
    foutWeights<<endl<<endl;	
}

void Exporter::exportCuesState(unsigned int k, float * x)
{
    foutPerTrial<<"trial number k="<<k<<" cues are ";
    for(int i=0;i<nc;i++)
        foutPerTrial<<x[i]<<"  ";
    foutPerTrial<<endl;
}

void Exporter::trialEndExport(float * sumM1freq, unsigned int seed)
{
    foutPerTrial<<"sums are  ";
    for(int i=0;i<na;i++)
        foutPerTrial<<sumM1freq[i]<<"  ";
    foutPerTrial<<endl;
    foutPerTrial<<"seed was "<<seed<<endl;
}

Exporter::Exporter()
{
    na = 100;
    nc = 1;
}

Exporter::Exporter(int na_,int nc_)
{
    setCounts(na_,nc_);
}

void Exporter::setCounts(int na_,int nc_)
{
    na = na_;
    nc = nc_;
}

void Exporter::exportDynData(unsigned int trialNum,float *y,float *d1,float *d2,float * gpe,float *gpi, float* addInfo)
{
    // first all M1s then all d1s and so on
    for(int i=0;i<na;i++) foutVarDyn2<<y[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn2<<d1[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn2<<d2[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn2<<gpe[i]<<'\t';
    for(int i=0;i<na;i++) foutVarDyn2<<gpi[i]<<'\t';
    foutVarDyn2<<endl;
}

void Exporter::exportWeights(unsigned int trialNum,float ** w1,float ** w2,float **wm)
{ 
    for(int i=0;i<na;i++) foutWeights2<<w1[0][i]<<'\t';
    for(int i=0;i<na;i++) foutWeights2<<w2[0][i]<<'\t';
    for(int i=0;i<na;i++) foutWeights2<<wm[0][i]<<'\t';
    foutWeights2<<endl;
}

void Exporter::exportWeightsOnce(float ** w1,float ** w2,float **wm)
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
}

void Exporter::exportArm(unsigned int trialNum,float xcur,float ycur, float x0, float y0, float xc, float yc, float * addInfo)
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
}
