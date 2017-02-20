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

    float cb_learn_rate;   // learning rate for the CB 
    float cb_init_shift_size; // stepsize used for computation of DF in cerebellum
    float x_cb_target, y_cb_target;  // current CB target

    float wcb[6][6];                 // current CB state
    float dfwx[6][6],dfwy[6][6];     // CB state corrections, corresponding to the current CB target

    Arm * arm;

    public:
        void learn(float x, float y);

        // train correcting reaching to (x0,y0). Supposeding that with this motor program 
        // without perturbation we reach (x0,y0) precisely
        // sets DF tensor
        // last argument tells wheter we want to keep current state or reset it to the default
        void train(float x0, float y0, float * yy = 0, float coef = 1., bool flushW=true);
        void flush();   // resets DF tensor to zeros

        void moveArm(float * y, float * out, float ffield);   // pretty sefl-descriptive
    
        // sets the target directly. Rarely used, because usually you would like 
        // to retrain CB to reach this point -- so you would call train method instead 
        void setCBtarget(float x, float y);  

        void setRandomState(float amplitudeOfRand); // fill W with random values from [0,amplitudeOfRand]
        void init(parmap & params,Exporter *exporter,Arm * arm_);
        CB_model();
        CB_model(Arm * arm_);

    private:
        void cblearn(float xdif, float ydif);
        void setArm(Arm * arm_);
};

#endif
