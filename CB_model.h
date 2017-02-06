#ifndef CB_MODEL_H
#define CB_MODEL_H

#include "hand.h"
// Cerebellum
class CB_model
{
    ////////// CB related

    float cb_learn_rate;  
    float cb_init_shift; // stepsize used for computation of DF in cerebellum
    float x_cb_target, y_cb_target;

    float wcb[6][6];
    float dfwx[6][6],dfwy[6][6];
    float xc,yc;
    float phi0[2];

    Hand * hand;

    private:
        void cblearn(float xdif, float ydif);
    public:
        void learn(float x, float y);
        void initCB(float x0, float y0, float * yy = 0, float coef = 1., bool flushW=true);
        void flush();

        void setHand(Hand * hand_);
    
        void setCBtarget(float x, float y);

        CB_model();
        CB_model(Hand * hand_);
};

#endif
