#ifndef FILE_EXPORT_H
#define FILE_EXPORT_H

#include "BG_model.h"

// separate exporter objects for separate sessions                                     
class Exporter
{ 
    // typically prefix is the stage of experiment number and maybe some parameter info
    // suffix is the number of session
    // dir is the output_<experiment name>
    string prefix,suffix,dir;
    string prefix_for_cont;
    string suffix_for_cont;
    
    ofstream foutPerTrial;
    //ofstream foutVarDyn;
    //ofstream foutWeights;

    ofstream foutVarDyn2;
    ofstream foutWeights2;
    ofstream foutWeightsOnce;
    ofstream foutLasty;
    ofstream foutArm;
    ofstream foutErr;
    
    ofstream foutContState;
    ofstream foutModParams;
    ofstream foutCBState;
    ofstream foutCBTuning;
    ofstream foutCBMisc;

    int na,nc;
public: 

    Exporter();
    Exporter(int na_, int nc_);
    void init(string prefix, string dir,string suffix);   // prefix = "RC" for example, dir = output dir, usually is read from ini file
    void setCounts(int na_,int nc_); 

    // this method should be called before any actual data write method
    // you may put anything you want to putInBeg string
    void exportInit(string prefix,string suffix,string putInBeg, bool minimumExport=false);   // prefix = "RC" for example, minimumExport -- used for prelearn to export only the weights at the end
    void exportClose();
    void exportContOpen(int k);
    void exportContClose();

    void exportParams(parmap & params);
    void exportCBMisc(float lrate,float errAbs,float ratio,float prevErrAbs,float ucr, float ucc);
    
    // data write methods
    void exportDynData(unsigned int trialNum,float *y,float *d1,float *d2,float * gpe,float *gpi, float* addInfo);
    void exportWeights(unsigned int trialNum,float ** w1,float ** w2,float **wm);
    void exportWeightsOnce(float ** w1,float ** w2,float **wm);
    void exportLasty(float * y);

    void exportArm(unsigned int trialNum,float xcur,float ycur, float x0, float y0, float xc, float yc, float * addInfo);
    void exportContState(float t,float *y,float *d1,float *d2,float * gpe,float *gpi);
    // methods mostly for compatibility, old stuff
    //void exportDynDataStd(unsigned int trialNum,float *y,float *d1,float *d2,float * gpe,float *gpi,float t,float reward, float sr);
    //void exportWeightsStd(unsigned int trialNum,float ** w1,float ** w2,float **wm);
    void exportTrial(unsigned int trialNum, float * cues, float R, float Rpre);
    void trialEndExport(float * sumM1freq, unsigned int seed);
    void CBExport(int k, float wcb[][6], float dfwx[][6], float dfwy[][6]);
};

#endif // FILE_EXPORT_H
