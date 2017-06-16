#ifndef CB_MODEL_H
#define CB_MODEL_H

#include "arm.h"
#include "exporter.h"
#include "percept.h"
#include <map>
#include <string>

using std::string;
using std::map;
typedef map<string,string> parmap;
// Cerebellum
class CB_model
{
  //int na;
    ////////// CB related
  float cbLRate;   // learning rate for the CB 
  float cbLRateMax;  
  float cbLRate_init;  

  float cbInitShiftSz; // stepsize used for computation of DF in cerebellum
  float x_cb_target, y_cb_target;  // current CB target
  float updateCBStateDist;

  float def_updateCBStateDist;

  float cbLRateUpdSpdUp;
  float cbLRateUpdSpdDown;
  float cbStateDepr;

  float cbRetrainNeeded_thr;

  float prevErrAbs;

  //float lastErrRatio;
  bool cbLRateUpd_errDiffBased;
  bool cbLRateIsConst;
  bool acByUpdCoefThr;

  float acUpdCoefThr;

  int cbErrDepth;
  float cbLRateUpdAbsErr_threshold;
  float cbLRateUpdErrRatio_threshold;

  int cbLRateUpdTwoErrThreshold;
  bool cbLRateUpdVariableSpd;

  float acOptimalRateMult;
  float acThrMult;
  bool acInstantUpd;

  vector<float> errHist;

    float wcb[6][6];                 // current CB state
    float dfwx[6][6],dfwy[6][6];     // CB state corrections, corresponding to the current CB target

    vector<float> train_patPMC;
    float wcb_train[6][6];                 // CB state used for last training

    float last_errDFmod;  // last value of update of the W, not multiplied by learning rate. Equals 
    // to  DF * err 

    Arm * arm;
    Exporter * exporter;
    Percept * percept;

    public:
        void learn();

        // train correcting reaching to (x0,y0). Supposeding that with this motor program 
        // without perturbation we reach (x0,y0) precisely
        // sets DF tensor           
        // last argument tells wheter we want to keep current state or reset it to the default
        void train(float x0, float y0, float * yy = 0, bool flushW=true, bool useCurW=false, float ffield=0.);
        void trainCurPt(float * yy=0, float ffield=0., bool flushW=false, bool useCurW=true);
        void flushTuning();   // resets DF tensor to zeros

        void moveArm(float * y, float * out, float ffield);   // pretty sefl-descriptive
        void resetLearnRate(float lr = -100);

        void set_tCDS(float val);
    
        float getLearnRate();
        // sets the target directly. Rarely used, because usually you would like 
        // to retrain CB to reach this point -- so you would call train method instead 
        void setCBtarget(float x, float y);  

        void setRandomState(float amplitudeOfRand); // fill W with random values from [0,amplitudeOfRand]
        void stateDegradeStep();
        void CBExport(int k);
        void init(parmap & params,Exporter *exporter,Arm * arm_, Percept * percept_);
        
        //float errDFmod(float dx, float dy);
        float get_ACHappiness(float * pupd_coef_real, float * pupd_coef_cb);

        bool trainNeeded(float * y_, float newx=-1000, float newy=-1000); 

        CB_model();
        CB_model(Arm * arm_);

    private:
        void cblearn(float xdif, float ydif);
        void setArm(Arm * arm_);
};

inline float matrixNorm(float matrix[][6])
{
  float norm = 0;
  for(int i = 0; i<6; i++)
    for(int j = 0; j<6; j++)
    {
      //sum += matrix[i][j] * matrix[i][j];
      norm = fmax(norm, fabs(matrix[i][j]) );
    }
  return norm;
}

#endif
