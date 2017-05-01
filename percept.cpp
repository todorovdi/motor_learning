#include "percept.h"

Percept::Percept()
{
    error_clamp_mode = false;
    reach_x = reach_y = tgt_x = tgt_y = -1000;

    errHist.reserve(20);
    errHist.resize(0);
}

float Percept::getErr(int ind)   // ind>=0, returns errHist[-1-ind]
{
//if(ind < err
  int ind_res = std::min<int>(ind,errHist.size()-1);
  return ind_res >= 0 ? *(errHist.end() - ind_res -1) : -1000;
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

float Percept::calcErr(float * pdx, float * pdy)
{
    *pdx = reach_x - tgt_x;
    *pdy = reach_y - tgt_y;
    float errAbs = sqrt( (*pdx)*(*pdx) +(*pdy)*(*pdy));
    if(error_clamp_mode)
    {
        *pdx = 0.;
        *pdy = 0.;
        errAbs = 0.;
    }
    return errAbs;
}

void Percept::saveCurErr()
{
    float dx,dy;
    errHist.push_back(calcErr(&dx,&dy));
}
