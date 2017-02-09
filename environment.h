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
    unsigned int numTrials;

    float successDist;  // distance the arm has to reach the success to count
    float rewardDist;   // distance, reaching within weach implies a reward. But actually depends on the way you define getReward, so it is more a suggestion

    unsigned int memoryLen; // currently not used

    int experimentPhase; // experiment phase number
    vector<string> phasesNames;

    int nc,na;  // number of cues and actions

    MotorLearning ml;
    Exporter exporter;

    parmap params;   // contain ALL the parameters from all files
    int num_sess;

    public:
        Environment();
        Environment(string paramsFile, int num_sess);

///////////////////////////////////////
//  Functions to implement in your experiment
///////////////////////////////////////
        virtual int turnOnCues(float * cues) = 0;
        virtual float getSuccess(float * x,float * y,unsigned int k,float *addInfo) = 0;  // precise meaning of success differs from experiment to experiment
        virtual float getReward(float success, float * x,float * y, float & param) = 0;  
        virtual void runSession() = 0;
        virtual void setParams(int argc, char** argv) = 0;
        virtual void prelearn(int nTrials, float * addInfo) = 0;   // for prelearning
};

#endif // ENVIRONMENT_H
