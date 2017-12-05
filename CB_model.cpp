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
    arm->move(yy,endpt,wcb,ffield,false);
    float bx = endpt[0]-x_cb_target;
    float by = endpt[1]-y_cb_target;

	  for(int i=0;i<6;i++) 
        for(int j=0;j<6;j++)
        {  
            // set only current weight nonzero
            wcb[i][j]+=cbInitShiftSz;

            // it influences update of DR which is basically some precalc * neuron activity
            // no noise in internal model
            arm->move(yy,endpt,wcb,ffield,false);

            // compute error vector
            dfwx[i][j]= (endpt[0]-x_cb_target - bx)/cbInitShiftSz; 
            dfwy[i][j]= (endpt[1]-y_cb_target - by)/cbInitShiftSz;

            //if(debug_printAC && (dfwx[i][j] > EPS || dfwy[i][j] > EPS) )
            //{
            //  cout<<" train ij"<<i<<j<<" dfwx,wy "<<dfwx[i][j]<<dfwy[i][j]<<endl;
            //}

            // restore
            wcb[i][j]-=cbInitShiftSz;
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

    if(debug_printAC)
    {
      cout<<" train CB to get new DF "<<endl;
    }
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
      float t =  cbStateDepr * wcb[i][j];
      wcb[i][j]-= t;
    } 
  }
}

float CB_model::getCurErrDFmod(float dx,float dy)
{
  float r = 0;
	for(int i=0;i<6;i++) 
  {
    for(int j=0;j<6;j++) 
    { 
      float t = (dx*dfwx[i][j]+dy*dfwy[i][j]);
      wcb[i][j]-=cbLRate*t; 
      r += t*t;

      //if(debug_printAC && t > EPS)
      //{
      //  cout<<" cblearn: ij"<<i<<j<<" dfwx[i][j], dfwy "<<dfwx[i][j]<<" "<<dfwy[i][j]<<" Delta wcb[i][j] "<<t;
      //  cout<<" wcb[i][j]= "<<wcb[i][j]<<endl;
      //}
    } 
  }
  return r;
}

// uses current CB state, so predicts for next reach
float CB_model::getExpectedErrChange(float dx,float dy,float lambda, float * pdifx, float * pdify)
{
  //DF * err
  // err is 1x2 string
  // DF is 2x32
  // eDF is 32x1
  float eDF[6][6];

  //cout<<"  matrix calc dx,dy"<<dx<<" "<<dy<<endl;
	for(int i=0;i<6;i++) 
  {
    for(int j=0;j<6;j++) 
    { 
      float t = (dx*dfwx[i][j]+dy*dfwy[i][j]);
      eDF[i][j] = lambda * t;
       
      if( criticPredictDepr )
      {
        eDF[i][j] += cbStateDepr * wcb[i][j]; 
      }
      //cout<<"  eDF["<<i<<","<<j<<"] is "<<eDF[i][j]<<endl;
      //cout<<"   dfwx "<<dfwx[i][j]<<" dfwy "<<dfwy[i][j]<<endl;
    } 
  }
  
  float rx = 0., ry=0.;
  // compute DF * eDF
	for(int i=0;i<6;i++) 
  {
    for(int j=0;j<6;j++) 
    { 
      float tx = dfwx[i][j] * eDF[i][j];
      float ty = dfwy[i][j] * eDF[i][j];

      //cout<<"   ij "<<i<<" "<<j<<" tx,ty "<<tx<<" "<<ty<<endl;

      rx += tx; 
      ry += ty; 
    } 
  }

  //cout<<"   rx,ry "<<rx<<" "<<ry<<" when analytic "<<rx = 

  if(pdifx && pdify)
  {
    *pdifx= rx;
    *pdify= ry;
  }

  return sqrt(rx*rx + ry*ry);
  //return rx*dx + ry*dy;
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
      wcb[i][j]-= cbStateDepr * wcb[i][j]; 
      wcb[i][j]-= cbLRate*t; 

      last_errDFmod += t*t;  
      

      //if(debug_printAC && t > EPS)
      //{
      //  cout<<" cblearn: ij"<<i<<j<<" dfwx[i][j], dfwy "<<dfwx[i][j]<<" "<<dfwy[i][j]<<" Delta wcb[i][j] "<<t;
      //  cout<<" wcb[i][j]= "<<wcb[i][j]<<endl;
      //}
    } 
  }

  //last_errDFmod = getCurErrDFmod(dx,dy);
  //if(debug_printAC)
  //{
  //  cout<<" cblearn: last_errDFmod = "<<last_errDFmod<<endl;
  //}
}

// should be called before cb learning is done
bool CB_model::trainNeeded(float * y_,float newx, float newy)
{
  bool ptchanged = false;
  if(newx > -100 && newy > -100)
  {
    ptchanged = ( fabs(newx - x_cb_target) > 0.05 ) || ( fabs(newy-y_cb_target) > 0.05 );
  }

  bool ychanged = false;
  bool Wchanged = false;

  if(!ptchanged)
  {
    for(int i = 0; i<train_patPMC.size();i++)
    {
      if(fabs(y_[i]-train_patPMC[i]) > 0.05)
      { 
        ychanged = true;
        break;
      }
    }
  }

  float ttt = 0;

  if(!ychanged && !ptchanged)
  {
    if(cbRetrainNeeded_LinfNorm)
    { 
      for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
        {
          // L^infty norm
          float tt_ = fabs( wcb_train[k][l] - wcb[k][l] );
          if( tt_  > cbRetrainNeeded_thr_Linf )
          {
            ttt = fmax( tt_, fabs( wcb_train[k][l] - wcb[k][l] ) );
            Wchanged = true;
            if(!debug_retrainCB)
            {
              break;
            }
          }
        }
    }
    else
    {
      for(int k=0;k<6;k++) 
      {
        for(int l=0;l<6;l++) 
        {
          ttt += fabs( wcb_train[k][l] - wcb[k][l] );
          // L^infty norm
          //if( fabs( wcb_train[k][l] - wcb[k][l] ) > cbRetrainNeeded_thr )
          //{
          //  tt = fmax( tt, fabs( wcb_train[k][l] - wcb[k][l] ) );
          //  Wchanged = true;
          //  //break;
          //}
        }
      }

      Wchanged = ttt > cbRetrainNeeded_thr ? true : false;
    }
  }


  bool needed = ychanged || Wchanged || ptchanged;

  if(debug_retrainCB)
  {
    cout<<" DEBUG: retrain W norm change "<<ttt<<" Linf norm? "<<cbRetrainNeeded_LinfNorm;
    cout<<" trainNeeded "<<needed<<" y|W|pt = "<<ychanged<<Wchanged<<ptchanged<<endl;
  }

  return needed;
} 

int CB_model::get_ACHappiness(float * pu_real, float * pu_expect)
{
  float dx,dy;

  // get current trial reaching error 
  float errAbs = percept->calcErr(&dx,&dy,true);

  // if the current error is really small, we say that we don't want to do with the learning rate
  if( errAbs < EPS )
    return 1;

  // get prev trial error, enough to have absolute value
  float dx_prev, dy_prev;
  float prevErrAbs = percept->getHistSz() > 1 ? percept->getErr(1,true,&dx_prev,&dy_prev) : -100;  

  float mismatch = 0;
  float mismatchThr = 0;

  float difx,dify;
  float difx_real,dify_real;

  // new shiny critic
  if(criticCoordVer)
  {
    // change of error
    difx_real = (dx_prev - dx);
    dify_real = (dy_prev - dy);

    if(criticExact)
    {
      // here we use just calculate what error we'd get if we used the current correction
      // but did not have any perceptual perturbation. I.e. if internal model F of the CB
      // was entierly consistent with reality
      difx = dx_prev - expectExactErr_x;
      dify = dy_prev - expectExactErr_y;

      float errMis_x = expectExactErr_x - dx;
      float errMis_y = expectExactErr_y - dy;

      //compare errors directly
      mismatch = sqrt( errMis_x*errMis_x + errMis_y*errMis_y  );
      if(pu_expect)
        *pu_expect = sqrt(expectExactErrSq);
    }
    else
    {
      // same as vector from prev reaching point (assuming it was unperturbed) to expected current reaching point
      //difx = dx_prev - expectApproxErr_x;
      //dify = dy_prev - expectApproxErr_y;

      float errMis_x = expectApproxErr_x - dx;
      float errMis_y = expectApproxErr_y - dy;

      //compare error magnitudes
      //mismatch = fabs( sqrt(expectApproxErrSq) - errAbs);

      //compare errors directly
      mismatch = sqrt( errMis_x*errMis_x + errMis_y*errMis_y  );
      if(pu_expect)
        *pu_expect = sqrt(expectApproxErrSq);
    }

    // the threshold is usually about the size of the noise level
    // if the difference between expectation and truth is within the noise size, then we are happy
    mismatchThr = cbMotVarEst;  

    if(pu_real)
    {
      *pu_real = mismatch;
    }
  }
  // old rusty critic
  else
  {
    float easq = errAbs*errAbs;

    // note that the cost functions that CB aims to decrease is quadratic, thus it is 
    // difference of SQUARES of moduli of vector errors, which is proportional to the prev learning rate
    float u_real = prevErrAbs * prevErrAbs - easq;

    float u_expect = 0;
    if(criticExact)
    {
      // here we use just calculate what error we'd get if we used the current correction
      // but did not have any perceptual perturbation. I.e. if internal model F of the CB
      // was entierly consistent with reality
      u_expect = prevErrAbs * prevErrAbs - expectExactErrSq;
    }
    else
    {
      u_expect = 2. * last_errDFmod * cbLRate;  // can also mult by (1-degradation)
    }

    if(pu_real && pu_expect)
    {
      *pu_real = u_real;
      *pu_expect = u_expect;
    }

    // if true we assume that CB did last correction. And it did it using
    // correct visual feedback (more or less)
  
    //float tt = cbMotVarEst * cbMotVarEst;
    // (err+ rnd)^2 - err^2 =  rnd^2 + 2*err*rnd
    mismatchThr = (2*prevErrAbs + cbMotVarEst)*cbMotVarEst; 
  }

  bool b,bneg; 
  //float  mismatch = fabs( u_real - u_expect);

  b =    mismatch <  mismatchThr * acLowThrMult;   
  bneg = mismatch >= mismatchThr * acThrMult;   

  if(debug_printAC)
  {
    cout<< " predicted difx,dify "<<difx<<", "<<dify<<" real is "<<difx_real<<", "<<dify_real
      <<" mismatch is "<<mismatch;
    cout<< " _and_ mismatch thresholds are "<<mismatchThr * acLowThrMult<<", "<<
      mismatchThr * acThrMult<<endl;
  }
    
  if(b) return 0;          // CB works ok
  else if(bneg) return 2;  // CB works bad
  else return 1;            // we are not sure
}
 
// adaptive critic code is here
void CB_model::learn()
{      
  float dx,dy;
  float errAbs = percept->calcErr(&dx,&dy,true);
  float errToCompare = percept->getErr(cbErrDepth,true,NULL,NULL);      // if size of hist is less then cbErrDepth, the oldest possible error is returned
  float cblr_upd = 1.;


  // here we take into account that CB_model::learn() is called AFTER
  // the arm movement, so error history contains current error already
  float prevErrAbs = percept->getHistSz() > 1 ? percept->getErr(1,true,NULL,NULL) : -100;  

  // if the error is too small, we will not learn as we are good already
  //bool b = errAbs > 0.3*cbMotVarEst;
  bool b = errAbs > criticMinErrToNotice;
  //bool b = true;
  if(debug_printAC)
  {
    cout<< "Critic start: current errAbs is "<<errAbs<<" dx,dy "<<dx<<", "<<dy<<" willUpd:"<<b<<endl;
  }

  // we can set a special flag to take previous error into account too
  if(cbLRateUpdTwoErrThreshold)
  { 
    b = b || errToCompare > cbMotVarEst;
  }

  float u_real=0;
  float u_expect = 0;
  float optimalLambda = 0;

  // if we don't have even last error before current, don't update the learning rate
  // it saves us from the problem of huge fullCorrSz
  if(percept->getHistSz() > 1 && !cbLRateIsConst && b)   
  {
    // compute square of the current erro
    float easq = errAbs*errAbs;
     
    // ask critic what it thinks we should do: increase learning rate, decrease it, or don't change
    int acAns = get_ACHappiness(&u_real,&u_expect);
    // prediction of how much correction should we make to eliminate error completely. 
    // It uses current correction matrix (which was used to make movement that has driven current
    // error) and current error

    float fullCorrSz;  

    // calculate learning rate that one would have to set to eliminate the error completely on the next step
    if(criticCoordVer)
    {      
      fullCorrSz = getExpectedErrChange(dx,dy,1.,NULL,NULL);   
      fullCorrSz = fmax(fullCorrSz,1e-9);
      optimalLambda = errAbs / fullCorrSz;
    }
    else
    {
      fullCorrSz = 2. * getCurErrDFmod(dx,dy);  
      fullCorrSz = fmax(fullCorrSz,1e-9);
      optimalLambda =  errAbs * errAbs / fullCorrSz;
    }

    if(debug_printAC)
    {
      cout<< " fullCorrSz is "<<fullCorrSz<<endl;
    }

    // resolve numerical uncertainty if numerator and denominator are too small. Otherwise we can get huge values, which spoil error clamp in Shmuelof
    // basically we don't the rate in this case
    //if( fullCorrSz < 1e-5 && errAbs < 1e-5 )
    //{
    //  optimalLambda = cbLRate / acOptimalRateMult;
    //}

    float maxRateCurTrial = acOptimalRateMult * optimalLambda;
    if(acAns==0)    // means CB expectation meets reality. Then correct stronger!
    {
      cbLRate = fmin( cbLRate * cbLRateUpdSpdUp , maxRateCurTrial);
      //cbLRate = fmin( cbLRate + cbLRateUpdSpdUp , maxRateCurTrial);
      if(debug_printAC)
      {
        cout<<" +AC increases Lrate to "<<cbLRate<<", maxRateCurTrial (max for the cbLRate) is"<<maxRateCurTrial;
        cout<<" optimalLbd is"<<optimalLambda<<endl;
        cout<<" wcb[0][0] is "<<wcb[0][0]<<endl;
      }
    }
    else if(acAns==2)  // means CB expectation is inconsistent with reality. Then reduce the l. rate 
    {
      cbLRate = fmin( cbLRate / cbLRateUpdSpdDown , maxRateCurTrial);
      //cbLRate -= cbLRateUpdSpdDown;
      //cbLRate = 0;
      if(debug_printAC)
      {
        cout<<" -AC reduces Lrate to "<<cbLRate<<endl;
      }
    }
    else  // we are not sure what happens in terms of meeting expectations -- let's wait
    {
      cbLRate = fmin( cbLRate , maxRateCurTrial);
      if(debug_printAC)
      {
        cout<<" 0AC does nothing "<<endl;
      }
    }

    cbLRate = fmax(cbLRate , 0.001);  // to avoid complete zero and negativity which was possible in some of the prev. realizations

    // some natural limit on the learning rate size
    cbLRate = fmin( cbLRate,  40.);
  }
 
  predictNextErrApprox(NULL);

  cblearn(dx, dy);
  

  float r = errToCompare /errAbs;
  exporter->exportCBMisc(cbLRate,errAbs,r,errToCompare,u_real,u_expect,optimalLambda);

  if(debug_printAC)
  {
    cout<<" CB learn end: learn rate is: "<<cbLRate<<endl;
  }
}

void CB_model::predictNextErrExact(float * y)
{
  // supposing that the action does not change and that CB knows how the arm will move for the new CB state
  // predict the next error
  float nextpt[2],tgt_x,tgt_y;
  // no noise
  arm->move(y,nextpt,wcb,0.,false);
  percept->getTgt(&tgt_x,&tgt_y);
  float tx,ty;
  tx = (nextpt[0] - tgt_x);
  ty = (nextpt[1] - tgt_y);

  expectExactErr_x = tx;
  expectExactErr_y = ty;
        
  expectExactErrSq = tx*tx + ty*ty;
  //if(debug_printAC)
  //{
  //  //cout<<" tgt now is "<<tgt_x<<" "<<tgt_y;
  //  //cout<<" pred endpt is "<<nextpt[0]<<" "<<nextpt[1];
  //  cout<<" predicted errSq is "<<expectExactErrSq<<endl;

  //  float xx,yy;
  //  percept->getEndpt(&xx,&yy);   // get PERCIEVED point
  //  cout<<" real reach was "<<xx<<", "<<yy<<"  predicted "<<nextpt[0]<<", "<<nextpt[1]<<endl;
  //}
}

// in fact we don't need motor program here, but note that CB was retrained with new 
// motor program alread (check the place where this function is called at motor_learning.cpp
void CB_model::predictNextErrApprox(float * y)
{
  float dx, dy;
  float errAbs = percept->calcErr(&dx,&dy,true);

  //float dx_prev, dy_prev;
  //float prevErrAbs = percept->getHistSz() > 1 ? percept->getErr(1,true,&dx_prev,&dy_prev) : -100;  

  // the function is called before we update the cbLRate, it is the old one
  // we will have difference between errors as CB expects it
  float difx,dify;
  getExpectedErrChange(dx, dy, cbLRate, &difx, &dify);

  expectApproxErr_x = dx - difx;
  expectApproxErr_y = dy - dify;   

  expectApproxErrSq = expectApproxErr_x * expectApproxErr_x + expectApproxErr_y * expectApproxErr_y;
  if(debug_printAC)
  {
    cout<<" approx error predicted: "<<sqrt(expectApproxErrSq)<<" coords "<<expectApproxErr_x<<", "<<expectApproxErr_y<<endl;
  }
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
  if(debug_printAC)
  {
    cout<<" learning rate resetted"<<endl;
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
  if(debug_printAC)
  {
    cout<<" tuning flushed"<<endl;
  }
}

void CB_model::setRandomState(float a)
{
    for(int k=0;k<6;k++) 
        for(int l=0;l<6;l++) 
            wcb[k][l] = a*2*rnd() - a;

  if(debug_printAC)
  {
    cout<<" random state set"<<endl;
  }
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
  if(debug_printAC)
  {
    cout<<" CB target sat "<<endl;
  }
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
  cbLRateUpdSpdUp = stof(params["cbLRateUpdSpdUp"]);
  cbLRateUpdSpdDown = stof(params["cbLRateUpdSpdDown"]);
  //cbLRateUpdSpdMax = stof(params["cbLRateUpdSpdMax"]);
  cbStateDepr = stof(params["cbStateDepr"]);

  string s;
  s = params["cbLRateUpd_errDiffBased"];
  cbLRateUpd_errDiffBased = s != "" ? stoi(s) : 0;

  s = params["cbInitShiftSz"];
  cbInitShiftSz = s != "" ? stof(s) : 0.5;

  s = params["cbErrDepth"];
  cbErrDepth = s != "" ? stoi(s) : 0;  ;

  s = params["cbMotVarEst"];
  cbMotVarEst = s != "" ? stof(s) : 0;  ;

  s = params["criticMinErrToNotice"];
  criticMinErrToNotice = s != "" ? stof(s) : 0;  ;

  s = params["cbLRateUpdErrRatio_threshold"];
  cbLRateUpdErrRatio_threshold = s != "" ? stof(s) : 1.;  ;

  s = params["cbLRateUpdTwoErrThreshold"];
  cbLRateUpdTwoErrThreshold = s != "" ? stoi(s) : 0;  ;

  s = params["cbLRateUpdVariableSpd"];
  cbLRateUpdVariableSpd = s != "" ? stoi(s) : 1;  ;

  s = params["cbLRateIsConst"];
  cbLRateIsConst = s != "" ? stoi(s) : 0; 


  s = params["cbRetrainNeeded_thr"];
  cbRetrainNeeded_thr = s != "" ? stof(s) : 0.4; 

  s = params["cbRetrainNeeded_thr_Linf"];
  cbRetrainNeeded_thr_Linf = s != "" ? stof(s) : 0.025; 

  s = params["cbRetrainNeeded_LinfNorm"];
  cbRetrainNeeded_LinfNorm = s != "" ? stoi(s) : 0; 

  s = params["acOptimalRateMult"];
  acOptimalRateMult = s != "" ? stof(s) : 0.6; 

  s = params["acThrMult"];
  acThrMult = s != "" ? stof(s) : 2; 

  s = params["acLowThrMult"];
  acLowThrMult = s != "" ? stof(s) : 2; 

  s = params["debug_printAC"];
  debug_printAC = s != "" ? stoi(s) : 0; 

  s = params["criticExact"];
  criticExact = s != "" ? stoi(s) : 0; 

  s = params["criticCoordVer"];
  criticCoordVer = s != "" ? stoi(s) : 0; 

  s = params["debug_retrainCB"];
  debug_retrainCB = s != "" ? stoi(s) : 0; 

  s = params["criticPredictDepr"];
  criticPredictDepr = s != "" ? stoi(s) : 0; 
 
  arm = arm_;
  exporter = exporter_;
  percept = percept_;
 
  train_patPMC.resize(stoi(params["na"]));

  last_errDFmod = 0; 
  expectExactErrSq = 0;

  // set inital correction to 0.
  setRandomState(0.);
}

void CB_model::CBExport(int k)
{
  exporter->CBExport(k,wcb,dfwx,dfwy);
}

void CB_model::set_tCDS(float val)
{
}

float CB_model::getLearnRate()
{
  return cbLRate;
}
