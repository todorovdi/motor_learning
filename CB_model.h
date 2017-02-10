#ifndef CB_MODEL_H
#define CB_MODEL_H

#include "arm.h"
#include <map>
#include <string>

using std::string;
using std::map;
typedef map<string,string> parmap;
// Cerebellum
class CB_model
{
    ////////// CB related

    float cb_learn_rate;  
    float cb_init_shift_size; // stepsize used for computation of DF in cerebellum
    float x_cb_target, y_cb_target;

    float wcb[6][6];
    float dfwx[6][6],dfwy[6][6];
    float xc,yc;
    float phi0[2];

    Arm * arm;
    //parmap params;

    private:
        void cblearn(float xdif, float ydif);
    public:
        void learn(float x, float y);

        // train correcting reaching to (x0,y0). Supposeding that with this motor program 
        // without perturbation we reach (x0,y0) precisely
        // sets DF tensor
        void train(float x0, float y0, float * yy = 0, float coef = 1., bool flushW=true);
        void flush();

        void setArm(Arm * arm_);
        void moveArm(float * y, float * out, float ffield);
    
        void setCBtarget(float x, float y);

        void init(parmap & params,Exporter *exporter,Arm * arm_);
        CB_model();
        CB_model(Arm * arm_);
};

#endif
