#ifndef FILE_EXPORT_H
#define FILE_EXPORT_H

#include "BG_model.h"

// separate exporter objects for separate sessions                                     
class Exporter
{ 
    // typically prefix is the stage of experiment number
    // suffix is the number of session
    // dir is the output_<experiment name>
    string prefix,suffix,dir;

    ofstream foutPerTrial;
    ofstream foutVarDyn;
    ofstream foutWeights;

    ofstream foutVarDyn2;
    ofstream foutWeights2;
    ofstream foutWeightsOnce;
    ofstream foutArm;
    ofstream foutErr;

    int na,nc;
 
    parmap *paramsEnv;
    parmap *paramsBG;
    parmap *paramsCB;

public: 
    void exportDynDataStd(unsigned int trialNum,float *y,float *d1,float *d2,float * gpe,float *gpi,float t,float reward, float sr);
    void exportWeightsStd(unsigned int trialNum,float ** w1,float ** w2,float **wm);
    void exportCuesState(unsigned int trialNum, float * cues);
    void trialEndExport(float * sumM1freq, unsigned int seed);

    void init(string dir,string suffix);   // prefix = "RC" for example
    void exportInit(string prefix,string suffix,string putInBeg);   // prefix = "RC" for example
    void exportClose();

    void exportDynData(unsigned int trialNum,float *y,float *d1,float *d2,float * gpe,float *gpi, float* addInfo);
    void exportWeights(unsigned int trialNum,float ** w1,float ** w2,float **wm);
    void exportWeightsOnce(float ** w1,float ** w2,float **wm);
    void exportArm(unsigned int trialNum,float xcur,float ycur, float x0, float y0, float xc, float yc, float * addInfo);

    void setCounts(int na_,int nc_);
    void setParamsMaps(parmap *paramsEnv, parmap *paramsBG, parmap *paramsCB);

    //Exporter(parmap paramsEnv, parmap paramsBG, parmap paramsCB);
    Exporter(int na_, int nc_);
    Exporter();
};

#endif // FILE_EXPORT_H
