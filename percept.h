#ifndef PERCEPT_H
#define PERCEPT_H

#include "suppl.h"


class Percept
{
  vector<float> errHist;
  float tgt_x, tgt_y;
  float reach_x, reach_y;
  bool error_clamp_mode;

    public:
  float getErr(int ind);   // ind>=0, returns errHist[-1-ind]
  int getHistSz();
  void resetErrHist();
  void setErrorClamp(bool b);

  void setTgt(float x, float y);
  void setEndpt(float x, float y);     
  void getEndpt(float * x, float * y);

  float calcErr(float * pdx, float * pdy);
  void saveCurErr();

  Percept();
};

#endif
