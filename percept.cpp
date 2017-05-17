#include "percept.h"

Percept::Percept()
{
    error_clamp_mode = false;
    reach_x = reach_y = tgt_x = tgt_y = -1000;

    errHist.reserve(20);
    errHist.resize(0);
}

float Percept::getErr(int ind, bool toBorder)   // ind>=0, returns errHist[-1-ind]
{
//if(ind < err
  int ind_res = std::min<int>(ind,errHist.size()-1);
  float err = ind_res >= 0 ? *(errHist.end() - ind_res -1) : -1000;
  if(vectorErrTgtBorder && toBorder && err > ( -1000. + EPS ) )
  {
    float t = rewardDist * vectorErrTgtBorderMult;
    err = err < t ? 0 : err - t; 
  }
  return err;
}

int Percept::getHistSz()
{
    return errHist.size();
}

void Percept::resetErrHist(){
    errHist.resize(0);
}

void Percept::setErrorClamp(bool b)
{
   error_clamp_mode = b; 
}

void Percept::setEndpt(float x, float y)
{
    reach_x = x;
    reach_y = y;
}

void Percept::getEndpt(float * x, float * y)
{
    *x = reach_x;
    *y = reach_y;
}

void Percept::setTgt(float x, float y)
{
    tgt_x = x;
    tgt_y = y;
}

float Percept::calcErr(float * pdx, float * pdy, bool toBorder)
{ 
 // vectorErrTgtBorder

    float dx = reach_x - tgt_x;
    float dy = reach_y - tgt_y;

    float mod_dx = dx*cos(rotateErr) - dy*sin(rotateErr);
    float mod_dy = dx*sin(rotateErr) + dy*cos(rotateErr);
    if(xreverseErr)
      mod_dx = -mod_dx;

   *pdx = mod_dx;
   *pdy = mod_dy;

    float errAbs = sqrt( (*pdx)*(*pdx) +(*pdy)*(*pdy));
    float err = errAbs;
    if(error_clamp_mode)
    {
        *pdx = 0.;
        *pdy = 0.;
        err = 0.;
    }
    else 
    { 
      if(vectorErrTgtBorder && toBorder)
      {

        float t = rewardDist * vectorErrTgtBorderMult;
        float errBorder = errAbs < t ? 0 : errAbs - t; 

        float coef = errBorder / errAbs;
        *pdx = coef * (reach_x - tgt_x);
        *pdy = coef * (reach_y - tgt_y);
        err = errBorder;
      }
    }
    return err;
}

void Percept::saveCurErr()
{
    float dx,dy;
    errHist.push_back(calcErr(&dx,&dy,false));
}

void Percept::init(parmap & params)
{
    string s = params["rewardDist"];
    rewardDist = s!="" ? stof(s) : 0.05;

    s = params["vectorErrTgtBorder"];
    vectorErrTgtBorder = s!="" ? stoi(s) : 0;

    s = params["vectorErrTgtBorderMult"];
    vectorErrTgtBorderMult = s!="" ? stof(s) : 0.8;

    s = params["xreverseErr"];
    xreverseErr = s!="" ? stoi(s) : 0;

    s = params["rotateErr"];
    rotateErr = s!="" ? stoi(s) : 0;
}
