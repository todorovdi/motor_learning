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

//#define DEPR_DEP_ON_ERR
 
void CB_model::learn(float dx,float dy)
{      
  float errAbs = sqrt(dx*dx +dy*dy);
  float cblr_upd = 0.;
  if(prevErrAbs < 10. && !cbLRateIsConst)   // if not, it is set artificially
  {
    //float mult = ( 1./(1.+m)  );

    if(cbLRateUpd_errDiffBased)
    { 
      cblr_upd = prevErrAbs - errAbs;
      cbLRate += cblr_upd > 0 ? cblr_upd * cbLRateUpdSpdUp : cblr_upd * cbLRateUpdSpdDown;
    }
    else
    { 
      if(errAbs < prevErrAbs)       // means succesful correction. Then correct more!
      { 
        cblr_upd = fmin(cbLRateUpdSpdMax,prevErrAbs/errAbs);
        cbLRate += cblr_upd* cbLRateUpdSpdUp;
   
#ifdef  DEPR_DEP_ON_ERR
        cbRateDepr = cbRateDepr_def;
        //cbRateDepr = cbRateDepr_def;
#endif
      }
      else
      { 
        cblr_upd = -fmin(cbLRateUpdSpdMax,errAbs/prevErrAbs); 
        cbLRate += cblr_upd* cbLRateUpdSpdDown;

#ifdef  DEPR_DEP_ON_ERR
        cbRateDepr = -cblr_upd* cbLDeprUpdSpd;
        //cout<<" current depr rate is "<<cbRateDepr<<endl;
#endif
      } 
    }

    cbLRate = fmax(cbLRate , 0.001);  // to avoid negativity

    float m = errAbs/updateCBStateDist;
    
    if(cbLRate > 0)
      cbLRate = fmin( cbLRate, cbLRateMax/m);
    else                                    // can happen if we comment one of the lines abouve
      cbLRate = max( cbLRate, -cbLRateMax/m);

  }
  cblearn(dx, dy);

  exporter->exportCBMisc(cbLRate,errAbs,cblr_upd,prevErrAbs);

  //lastErrRatio = fmin(cbLRateUpdSpdMax,prevErrAbs/errAbs);
  //lastErrRatio = errAbs;
  prevErrAbs = errAbs;

  //cout<<"errAbs "<<errAbs<<", learn_rate  "<<cbLRate<<",  W norm "<<matrixNorm(wcb)<<endl;
}

float CB_model::getLastErr()
{
  return prevErrAbs;
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
  cbLRate = stof(params["cbLRate"]);
  cbLRateMax = stof(params["cbLRateMax"]);
  cbLRate_init = cbLRate;
  cb_init_shift_size = stof(params["cb_init_shift_size"]);
  updateCBStateDist = stof(params["updateCBStateDist"]);
  cbLRateUpdSpdUp = stof(params["cbLRateUpdSpdUp"]);
  cbLRateUpdSpdDown = stof(params["cbLRateUpdSpdDown"]);
  cbLRateUpdSpdMax = stof(params["cbLRateUpdSpdMax"]);
  cbRateDepr = stof(params["cbRateDepr"]);
  cbRateDepr = stof(params["cbRateDepr"]);

  string s = params["cbLRateUpd_errDiffBased"];
  cbLRateUpd_errDiffBased = s != "" ? stoi(s) : 0;

 //////////////// WARNING!!!!
  cbLDeprUpdSpd = stof(params["cbLDeprUpdSpd"]);
  
  s = params["cbLRateIsConst"];
  cbLRateIsConst = s != "" ? stoi(s) : 0; 
  
  cbRateDepr_def = cbRateDepr;

  def_updateCBStateDist = updateCBStateDist;

  arm = arm_;
  exporter = exporter_;
}

void CB_model::CBExport(int k)
{
  exporter->CBExport(k,wcb,dfwx,dfwy);
}

void CB_model::set_tCDS(float val)
{
  updateCBStateDist = def_updateCBStateDist + val;
}
