#ifndef PERCEPT_H
#define PERCEPT_H

#include "suppl.h"
#include "BG_model.h"


class Percept
{
  vector<float> errHist_x;
  vector<float> errHist_y;

  float tgt_x, tgt_y;
  float reach_x, reach_y;
  bool error_clamp_mode;
  float rewardDist;
  bool vectorErrTgtBorder;
  float vectorErrTgtBorderMult;

  bool rotateErr;
  bool xreverseErr;

  bool debug_printAC;


    public:
  float getErr(int ind, bool toBorder=false, float * px=NULL, float * py=NULL);   // ind>=0, returns errHist[-1-ind]
  int getHistSz();
  void resetErrHist();
  void setErrorClamp(bool b);

  void setTgt(float x, float y);
  void getTgt(float * px, float * py);
  void setEndpt(float x, float y);     
  void getEndpt(float * x, float * y);

  float calcErr(float * pdx, float * pdy, bool toBorder);
  void saveCurErr();

  Percept();
  void init(parmap & params);
};

#endif
