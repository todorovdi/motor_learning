#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "hand.h"
// class representing environement that BG works with
class Environment
{
////////////////////////////// 
////////   Constants to set in your experiment
/////////////////////////////
    unsigned int nsessions;
    unsigned int numTrials;
    float successDist;  // distance the hand has to reach the success to count
    float initAng[4];

    unsigned int memoryLen; // currently not used

    //Hand * hand;

    public:
///////////////////////////////////////
//  Functions to implement in your experiment
///////////////////////////////////////
        virtual int turnOnCues(float * cues) = 0;
        virtual float getSuccess(float * x,float * y,unsigned int k,float *addInfo){}  // precise meaning of success differs from experiment to experiment
        virtual float getReward(float success, float * x,float * y, float & param){}  
        
        Environment();
        //Environment(Hand * hand_);
        //float moveHand(float * y, float* out, float ffield);
};


class Experiment: public Environment
{
};

#endif // ENVIRONMENT_H
