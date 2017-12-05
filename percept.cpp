#include "percept.h"

Percept::Percept()
{
    error_clamp_mode = false;
    reach_x = reach_y = tgt_x = tgt_y = -1000;

    errHist_x.reserve(20);
    errHist_y.reserve(20);
    errHist_x.resize(0);
    errHist_y.resize(0);
}

// currently when toBorder=true we the coordinates are not ok, only the modulus is right
float Percept::getErr(int ind, bool toBorder, float * px, float * py)   // ind>=0, returns errHist[-1-ind]
{
//if(ind < err
  int ind_res = std::min<int>(ind,errHist_x.size()-1);
  float err_x = ind_res >= 0 ? *(errHist_x.end() - ind_res -1) : -1000;
  float err_y = ind_res >= 0 ? *(errHist_y.end() - ind_res -1) : -1000;

  if(px && py)
  {
    *px = err_x;
    *py = err_y;
  }

  float err = sqrt(err_x*err_x + err_y*err_y);
  if(vectorErrTgtBorder && toBorder && err > ( -1000. + EPS ) )
  {
    float t = rewardDist * vectorErrTgtBorderMult;
    err = err < t ? 0 : err - t; 
  }
  return err;
}

int Percept::getHistSz()
{
  return errHist_x.size();
}

void Percept::resetErrHist()
{
  if(debug_printAC)
  {
    cout<<"Percept: error history was reset "<<endl;
  }

  errHist_x.resize(0);
  errHist_y.resize(0);
  //cout<<"reset hist"<<endl;
}

void Percept::setErrorClamp(bool b)
{
   error_clamp_mode = b; 
}

// set reaching endpoint, inclluding all perception perturbations
// what the model actually sees
void Percept::setEndpt(float x, float y)
{
    reach_x = x;
    reach_y = y;
}

// returns perturbed endpoint 
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

void Percept::getTgt(float * px, float * py)
{
    *px = tgt_x;
    *py = tgt_y;
}

// reach - tgt
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
    calcErr(&dx,&dy,false);
    errHist_x.push_back(dx);
    errHist_y.push_back(dy);
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

    s = params["debug_printAC"];
    debug_printAC = s != "" ? stoi(s) : 0; 
}
