#include "BG_model.h"
#include "CB_model.h"

void CB_model::train(float x0, float y0, float * yy, bool flushW, bool useCurW, float ffield)
    // coef is the coef of the addition to the existing value
{
    x_cb_target = x0;     // TODO x_cb_target should debend on cues activated
    y_cb_target = y0;

    float wcb_train[6][6];
    for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
            wcb_train[k][l] = wcb[k][l];

    if(!useCurW)
    {
      for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
        { 
          wcb[k][l]=0;
        }
    }

    float endpt[2];
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

    std::copy(yy,yy+train_patPMC.size(),train_patPMC.begin());
    //cout<<" CB training"<<endl;
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
      float t =  cbRateDepr * wcb[i][j];
      wcb[i][j]-= t;
    } 
  }
}

// update W
void CB_model::cblearn(float dx,float dy)
{
  //float norm = matrixNorm(wcb);
  last_errDFmod = 0;
	for(int i=0;i<6;i++) 
  {
    for(int j=0;j<6;j++) 
    { 
      float t = (dx*dfwx[i][j]+dy*dfwy[i][j]);
      wcb[i][j]-=cbLRate*t; 
      last_errDFmod += t*t;
    } 
  }
  //last_errDFmod = sqrt(last_errDFmod);
  //last_errDFmod *= cbLRate;
}

//float CB_model::errDFmod(float dx, float dy)
//{
//  float r = 0;
//	for(int i=0;i<6;i++) 
//  {
//    for(int j=0;j<6;j++) 
//    { 
//      float t = (dx*dfwx[i][j]+dy*dfwy[i][j]);
//        r+=t*t; 
//    } 
//  }
//  return sqrt(r);
//}

// should be called before cb learning is done
bool CB_model::trainNeeded(float * y_)
{
  bool ychanged = false;
  for(int i = 0; i<train_patPMC.size();i++)
  {
    if(fabs(y_[i]-train_patPMC[i]) > 0.1)
    { 
      ychanged = true;
      break;
    }
  }

  bool Wchanged = false;
  float tt = 0;
  float ttt = 0;

  if(!ychanged)
  {
    for(int k=0;k<6;k++) 
      for(int l=0;l<6;l++) 
      {
        ttt += fabs( wcb_train[k][l] - wcb[k][l] );
        //if( fabs( wcb_train[k][l] - wcb[k][l] ) > cbRetrainNeeded_thr )
        //{
        //  tt = fmax( tt, fabs( wcb_train[k][l] - wcb[k][l] ) );
        //  Wchanged = true;
        //  //break;
        //}
      }
  }
  Wchanged = ttt > cbRetrainNeeded_thr ? true : false;

  bool needed = ychanged || Wchanged;
  //cout<<" trainNeeded "<<needed<<" W change (L1 norm) "<<ttt<<endl;
  return needed;
} 

float CB_model::get_ACHappiness(float * pupd_coef_real, float * pupd_coef_cb)
{
  float dx,dy;
  float errAbs = percept->calcErr(&dx,&dy);
  float prevErrAbs = percept->getHistSz() > 1 ? percept->getErr(1) : -100;  
  float easq = errAbs*errAbs;

  *pupd_coef_real = prevErrAbs * prevErrAbs - easq;
  *pupd_coef_cb = last_errDFmod * cbLRate;
  // if true we assume that CB did last correction. And it did it using
  // correct visual feedback (more or less)
    
  return fabs(*pupd_coef_real - *pupd_coef_cb);
}
 
// adaptive critic code is here
void CB_model::learn()
{      
  float dx,dy;
  float errAbs = percept->calcErr(&dx,&dy);
  float errToCompare = percept->getErr(cbErrDepth);      // if size of hist is less then cbErrDepth, the oldest possible error is returned
  float cblr_upd = 1.;

  // here we take into account that CB_model::learn() is called AFTER
  // the arm movement, so error history contains current error already
  float prevErrAbs = percept->getHistSz() > 1 ? percept->getErr(1) : -100;  

  bool b = errAbs > cbLRateUpdAbsErr_threshold;
  if(cbLRateUpdTwoErrThreshold)
  { 
    b = b || errToCompare > cbLRateUpdAbsErr_threshold;
  }

  float upd_coef_real=0;
  float upd_coef_cb = 0;

  // if we don't have even last error before current, don't update the learning rate
  if(percept->getHistSz() > 1 && !cbLRateIsConst && b)   // if not, it is set artificially
  {
    //float mult = ( 1./(1.+m)  );

    // note that the cost functions that CB aims to decrease is quadratic, thus it is 
    // difference of SQUARES of moduli of vector errors, which is proportional to the prev learning rate
    float easq = errAbs*errAbs;
    bool b2,b2neg; 
    if(acByUpdCoefThr)
    { 
      upd_coef_real = prevErrAbs * prevErrAbs - easq;
      upd_coef_cb = last_errDFmod * cbLRate;
      // if true we assume that CB did last correction. And it did it using
      // correct visual feedback (more or less)
      
      float tt = acUpdCoefThr;
      b2 = fabs( upd_coef_real - upd_coef_cb) < tt;   
      b2neg = fabs( upd_coef_real - upd_coef_cb) > tt * 2.;   

      if(b2)       // means succesful correction. Then correct more!
      {
        cblr_upd = 0.6 * prevErrAbs * prevErrAbs / last_errDFmod;
        cbLRate = fmin( cbLRate * cbLRateUpdSpdUp , cblr_upd);
      }
      else if(b2neg)
      {
        cblr_upd = 1/(cbLRateUpdSpdDown);
        cbLRate *= cblr_upd;
        //cbLRate = 0;
      }
    }
    else
    {
      b2 = cbLRateUpdErrRatio_threshold * errAbs < errToCompare; 

      if(b2)       // means succesful correction. Then correct more!
      {
        cblr_upd = cbLRateUpdSpdUp * upd_coef_real;
        cbLRate *= cblr_upd;
      }
      else 
      {
        cblr_upd = 1/(upd_coef_real * cbLRateUpdSpdDown);
        cbLRate *= cblr_upd;
      }
    }

    cbLRate = fmax(cbLRate , 0.001);  // to avoid negativity

    float m = errAbs/updateCBStateDist;
    
    if(cbLRate > 0)
      cbLRate = fmin( fmin( cbLRate, cbLRateMax/m), 40.);
  }
  cblearn(dx, dy);


  float r = errToCompare /errAbs;
  exporter->exportCBMisc(cbLRate,errAbs,r,errToCompare,upd_coef_real,upd_coef_cb);
  //exporter->exportCBMisc(cbLRate,errAbs,log(cblr_upd),errToCompare);

  //cout<<"errAbs "<<errAbs<<", learn_rate  "<<cbLRate<<",  W norm "<<matrixNorm(wcb)<<endl;
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

  last_errDFmod = 0; 
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

void CB_model::init(parmap & params,Exporter *exporter_, Arm * arm_, Percept * percept_)
{

  cbLRate = stof(params["cbLRate"]);
  cbLRateMax = stof(params["cbLRateMax"]);
  cbLRate_init = cbLRate;
  cb_init_shift_size = stof(params["cb_init_shift_size"]);
  updateCBStateDist = stof(params["updateCBStateDist"]);
  cbLRateUpdSpdUp = stof(params["cbLRateUpdSpdUp"]);
  cbLRateUpdSpdDown = stof(params["cbLRateUpdSpdDown"]);
  //cbLRateUpdSpdMax = stof(params["cbLRateUpdSpdMax"]);
  cbRateDepr = stof(params["cbRateDepr"]);
  cbRateDepr = stof(params["cbRateDepr"]);

  string s;
  s = params["cbLRateUpd_errDiffBased"];
  cbLRateUpd_errDiffBased = s != "" ? stoi(s) : 0;

  s = params["cbErrDepth"];
  cbErrDepth = s != "" ? stoi(s) : 0;  ;

  s = params["cbLRateUpdAbsErr_threshold"];
  cbLRateUpdAbsErr_threshold = s != "" ? stof(s) : 0;  ;

  s = params["cbLRateUpdErrRatio_threshold"];
  cbLRateUpdErrRatio_threshold = s != "" ? stof(s) : 1.;  ;

  s = params["cbLRateUpdTwoErrThreshold"];
  cbLRateUpdTwoErrThreshold = s != "" ? stoi(s) : 0;  ;

  s = params["cbLRateUpdVariableSpd"];
  cbLRateUpdVariableSpd = s != "" ? stoi(s) : 1;  ;

  s = params["acByUpdCoefThr"];
  acByUpdCoefThr = s != "" ? stoi(s) : 0;  ;

  s = params["acUpdCoefThr"];
  acUpdCoefThr = s != "" ? stof(s) : 0.1;  ;

  s = params["cbLRateIsConst"];
  cbLRateIsConst = s != "" ? stoi(s) : 0; 

  def_updateCBStateDist = updateCBStateDist;
 
  s = params["cbRetrainNeeded_thr"];
  cbRetrainNeeded_thr = s != "" ? stof(s) : 0; 

  arm = arm_;
  exporter = exporter_;
  percept = percept_;
 
  train_patPMC.resize(stoi(params["na"]));

  last_errDFmod = 0; 
}

void CB_model::CBExport(int k)
{
  exporter->CBExport(k,wcb,dfwx,dfwy);
}

void CB_model::set_tCDS(float val)
{
  updateCBStateDist = def_updateCBStateDist + val;
}

float CB_model::getLearnRate()
{
  return cbLRate;
}
