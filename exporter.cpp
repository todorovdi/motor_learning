#include "exporter.h"

#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#ifdef PARALLEL
#include <omp.h>
#endif

void Exporter::init(string prefix_,string suffix_,string dir_)   // prefix = "RC" for example
{
  prefix = prefix_;
  suffix = suffix_;

  dir = dir_;
  if(dir_[0] == '~')
  {
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL) 
    {
        homedir = getpwuid(getuid())->pw_dir;
    }
    dir = string(homedir) + dir_.substr(1,dir_.length()-1);
  }
  //cout<<dir<<endl;

}

void Exporter::exportInit(string prefix,string suffix,string begPut,bool minimumExport)   // prefix = "RC" for example
{
  prefix_for_cont = prefix;
  suffix_for_cont = suffix;

  this->suffix = suffix;
  this->prefix = prefix;

  if(!minimumExport)
  { 
    foutPerTrial.open( dir+prefix+string("_output")+suffix+string(".dat")) ;
    foutVarDyn.open  ( dir+prefix+string("_var_dyn")+suffix+string(".dat") );
    foutVarDyn2.open  ( dir+prefix+string("_var_dyn2")+suffix+string(".dat") );
    foutWeights.open  ( dir+prefix+string("_weights")+suffix+string(".dat") );
    foutWeights2.open  ( dir+prefix+string("_weights2")+suffix+string(".dat") );
    foutArm.open  ( dir+prefix+string("_arm")+suffix+string(".dat") );
    foutModParams.open  ( dir+prefix+string("_modParams")+suffix+string(".dat") );

    foutCBState.open  ( dir+prefix+string("_CBState")+suffix+string(".dat") );
    foutCBTuning.open  ( dir+prefix+string("_CBTuning")+suffix+string(".dat") );
    foutCBMisc.open ( dir+prefix+string("_CBMisc")+suffix+string(".dat") ); 
    if(begPut != "")
    { 
        foutArm<<begPut;
    }

  }
  foutWeightsOnce.open  ( dir+prefix+string("_weights_once")+suffix+string(".dat") );
  foutLasty.open  ( dir+prefix+string("_Lasty")+suffix+string(".dat") );
}

void Exporter::exportContOpen(int k)
{

  foutContState.open( dir+prefix_for_cont+string("_cont_state_")+std::to_string(k)+"_"+suffix_for_cont+string(".dat") );
}

void Exporter::exportCBMisc(float lrate,float errAbs,float ratio,float prevErrAbs)
{
  foutCBMisc<<lrate<<'\t'<<errAbs<<'\t'<<ratio<<'\t'<<prevErrAbs<<endl;
}

void Exporter::exportParams(parmap & params)
{
    parmap::iterator it= params.begin();
    for(;it!=params.end();it++)
    {
        foutModParams<<it->first<<"="<<it->second<<"\n";
    }
//#ifdef PARALLEL
//    cout<<"exporter thread Num "<<omp_get_thread_num()<<"  dirShift "<<params["dirShift"]<<" for prefix "<<prefix<<endl;
//#endif
    foutModParams.flush();
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
    foutModParams.close();
    foutLasty.close();
}

void Exporter::exportContClose()
{
    foutContState.close();

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

void Exporter::exportContState(float t,float *y,float *d1,float *d2,float * gpe,float *gpi)
{
  // first all M1s then all d1s and so on
  foutContState<<t<<'\t';
  for(int i=0;i<na;i++) foutContState<<y[i]<<'\t';
  for(int i=0;i<na;i++) foutContState<<d1[i]<<'\t';
  for(int i=0;i<na;i++) foutContState<<d2[i]<<'\t';
  for(int i=0;i<na;i++) foutContState<<gpe[i]<<'\t';
  for(int i=0;i<na;i++) foutContState<<gpi[i]<<'\t';
  foutContState<<endl;
}

void Exporter::exportWeights(unsigned int trialNum,float ** w1,float ** w2,float **wm)
{ 
    for(int cue=0;cue<nc;cue++)
    { 
      for(int i=0;i<na;i++) foutWeights2<<w1[cue][i]<<'\t';
      for(int i=0;i<na;i++) foutWeights2<<w2[cue][i]<<'\t';
      for(int i=0;i<na;i++) foutWeights2<<wm[cue][i]<<'\t';
    }
    foutWeights2<<endl;
}

void Exporter::exportLasty(float * y)
{
  for(int i=0;i<na;i++) foutLasty<<y[i]<<'\t';
  foutLasty<<endl;
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
    float x_actual = addInfo[3];
    float y_actual = addInfo[4];
    float xcbt = addInfo[5];
    float ycbt = addInfo[6];
    unsigned int k = trialNum;
    foutArm<<k<<'\t'       //1
           <<xcur<<'\t'    //2
           <<ycur<<'\t'    //3
           <<x0  <<'\t'    //4   // most rewarded target coords
           <<y0  <<'\t'    //5
           <<d   <<'\t'    //6
           <<xc  <<'\t'    //7
           <<yc  <<'\t'    //8
           <<x_actual  <<'\t'    //9
           <<y_actual  <<'\t'    //10
           <<xcbt  <<'\t'    //11
           <<ycbt  <<'\t'    //12
           <<endl;
}

void Exporter::CBExport(int k, float wcb[][6], float  dfwx[][6], float dfwy[][6])
{
  foutCBState<<k<<'\t';
  for(int i = 0;i<6;i++)
    for(int j =0;j<6;j++)
      foutCBState<<wcb[i][j]<<'\t';
  foutCBState<<endl;

  foutCBTuning<<k<<'\t';
  for(int i = 0;i<6;i++)
    for(int j =0;j<6;j++)
      foutCBTuning<<dfwx[i][j]<<'\t';
  for(int i = 0;i<6;i++)
    for(int j =0;j<6;j++)
      foutCBTuning<<dfwy[i][j]<<'\t';
  foutCBTuning<<endl;
}
