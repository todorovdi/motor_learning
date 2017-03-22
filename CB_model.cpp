#include "BG_model.h"
#include "CB_model.h"

void CB_model::train(float x0, float y0, float * yy, bool flushW, bool useCurW, float ffield)
    // coef is the coef of the addition to the existing value
{
    float endpt[2];
//    if(yy == 0)
//        yy = last_y;

    x_cb_target = x0;     // TODO x_cb_target should debend on cues activated
    y_cb_target = y0;

    float wcb_backup[6][6];
    for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
            wcb_backup[k][l] = wcb[k][l];


    if(useCurW)
    {
      for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
        { 
          wcb[k][l]=wcb_backup[k][l];
        }
    }
    else
    {
      for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
        { 
          wcb[k][l]=0;
        }
    }


	  for(int i=0;i<6;i++) 
        for(int j=0;j<6;j++)
        {  
            // this is not needed because we add W*(activities ) to existing activities. So we actually do (Id + W)*(activities)
            //for(int l=0;l<6;l++) 
            //    wcb[l][l] = 1.;

            // set only current weight nonzero
            wcb[i][j]+=cb_init_shift_size;

            //x[0]=a; x[1]=b;
            
            // it influences update of DR which is basically some precalc * neuron activity
            arm->move(yy,endpt,wcb,ffield);

            // compute error vector
            dfwx[i][j]= (endpt[0]-x_cb_target)/cb_init_shift_size; 
            dfwy[i][j]= (endpt[1]-y_cb_target)/cb_init_shift_size;

            // restore
            wcb[i][j]-=cb_init_shift_size;
        }

    // flush cb weights so that they do not influence normal movements
    // we will set them to nonzero if we do cblearn
    if(flushW)
    { 
      cout<<"flushing CB state"<<endl;
      for(int k=0;k<6;k++) 
          for(int l=0;l<6;l++) 
              wcb[k][l]=0;
    }
    else  // maybe redundant
    { 
        for(int k=0;k<6;k++) 
            for(int l=0;l<6;l++) 
                wcb[k][l] = wcb_backup[k][l];
    } 

    //learn_cb = true;
}

void CB_model::trainCurPt(float * yy, float ffield, bool flushW, bool useCurW)
{
    train(x_cb_target,y_cb_target,yy,flushW, useCurW, ffield);
}

void CB_model::stateDegradeStep()
{
	for(int i=0;i<6;i++) 
  {
    for(int j=0;j<6;j++) 
    { 
        wcb[i][j]-=cbRateDepr * wcb[i][j];
    } 
  }
}

void CB_model::cblearn(float dx,float dy)
{
  //float norm = matrixNorm(wcb);
	for(int i=0;i<6;i++) 
  {
    for(int j=0;j<6;j++) 
    { 
        wcb[i][j]-=cbLRate*(dx*dfwx[i][j]+dy*dfwy[i][j]); 
    } 
  }
}
 
void CB_model::learn(float dx,float dy)
{      
  float errAbs = sqrt(dx*dx +dy*dy);
  float ratio = 0.;
  if(prevErrAbs < 10.)   // if not, it is set artificially
  {
    //float mult = ( 1./(1.+m)  );

    if(errAbs < prevErrAbs)       // prevErrAbs < errAbs
    { 
      ratio = fmin(cbLRateUpdSpdMax,prevErrAbs/errAbs);
      cbLRate += ratio* cbLRateUpdSpdUp;
    }
    else
    { 
      ratio = -fmin(cbLRateUpdSpdMax,errAbs/prevErrAbs); 
      cbLRate += ratio* cbLRateUpdSpdDown;
    } 

    cbLRate = fmax(cbLRate , 0.001);  // to avoid negativity

    float m = errAbs/updateCBStateDist;
    cbLRate = fmin( cbLRate, cbLRate_init/m);

    //cbLRate *= mult;     // like that we kill the rate as we do it each trial
  }
  //if( errAbs < updateCBStateDist )
  { 
    cblearn(dx, dy);
  }                  
  exporter->exportCBMisc(cbLRate,errAbs,ratio,prevErrAbs);

  prevErrAbs = errAbs;

  //cout<<"errAbs "<<errAbs<<", learn_rate  "<<cbLRate<<",  W norm "<<matrixNorm(wcb)<<endl;
}

void CB_model::resetPrevErr(float pe)
{
  prevErrAbs = pe;
}

void CB_model::resetLearnRate(float lr)
{
  if(lr < -EPS)
    cbLRate = cbLRate_init;
  else
  {
    cbLRate_init = lr;
    cbLRate = lr;
  }
}

void CB_model::flushTuning()
{
    for(int i = 0; i<6;i++)
        for(int j=0;j<6;j++)
        {
            dfwx[i][j] = 0;
            dfwy[i][j] = 0;
        }
}

void CB_model::setRandomState(float a)
{
    for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
            wcb[k][l] = a*2*rnd() - a;
}

void CB_model::setArm(Arm * arm_)
{
    arm = arm_;
}

CB_model::CB_model(Arm * arm_)
{
    setArm(arm_);
    x_cb_target=0., y_cb_target=0.;
    prevErrAbs = 100;
}

CB_model::CB_model()
{ 
    x_cb_target=0., y_cb_target=0.;
    prevErrAbs = 100;
}

void CB_model::setCBtarget(float x, float y)
{
    x_cb_target=x, y_cb_target=y;
}

void CB_model::moveArm(float * y, float * out, float ffield)
{
    arm->move(y,out,wcb,ffield);
}

void CB_model::init(parmap & params,Exporter *exporter_, Arm * arm_)
{
    //readIni(iniCBname,params);

    cbLRate = stof(params["cbLRate"]);
    cbLRate_init = cbLRate;
    cb_init_shift_size = stof(params["cb_init_shift_size"]);
    updateCBStateDist = stof(params["updateCBStateDist"]);
    cbLRateUpdSpdUp = stof(params["cbLRateUpdSpdUp"]);
    cbLRateUpdSpdDown = stof(params["cbLRateUpdSpdDown"]);
    cbLRateUpdSpdMax = stof(params["cbLRateUpdSpdMax"]);
    cbRateDepr = stof(params["cbRateDepr"]);

    arm = arm_;
    exporter = exporter_;
}

void CB_model::CBExport(int k)
{
  exporter->CBExport(k,wcb,dfwx,dfwy);
}
