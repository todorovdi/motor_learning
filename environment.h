#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "arm.h"
#include "BG_model.h"
#include "motor_learning.h"
#include "exporter.h"
#include <vector>
#include <string>
#include <map>


using std::string;
using std::map;
typedef parmap parmap;

// class representing environement that BG works with
// one object per session (experiment run)
// single session is not parallelizable, works in a single thread
class Environment
{
////////////////////////////// 
////////   Constants to set in your experiment
/////////////////////////////
    protected:
    unsigned int nsessions;
    unsigned int numTrials;
    float successDist;  // distance the arm has to reach the success to count
    //float initAng[4];
    float rewardDist;

    unsigned int memoryLen; // currently not used

    bool prelearnEachTime;

    int experimentPhase;
    vector<string> phasesNames;
    int nc,na;

    MotorLearning ml;
    Exporter exporter;

    parmap params;   // contain ALL the parameters from all files
    int num_sess;

    public:
///////////////////////////////////////
//  Functions to implement in your experiment
///////////////////////////////////////
        virtual int turnOnCues(float * cues) = 0;
        virtual float getSuccess(float * x,float * y,unsigned int k,float *addInfo) = 0;  // precise meaning of success differs from experiment to experiment
        virtual float getReward(float success, float * x,float * y, float & param) = 0;  
        virtual void runSession() = 0;
        virtual void setParams(int argc, char** argv) = 0;

        //void setModel(MotorLearning * ml_);
        
        Environment();
        Environment(string paramsFile, int num_sess);

        virtual void prelearn(int nTrials, float * addInfo) = 0;
        //Environment(Hand * arm_);
        //float moveHand(float * y, float* out, float ffield);
};


class Experiment: public Environment
{
};

#endif // ENVIRONMENT_H
