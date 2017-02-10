#include "CB_model.h"
#include "motor_learning.h"
#include "suppl.h"
#include "exporter.h"
#include "environment.h"


class testExperimentEnv: public Environment
{
    float targetPre1; // in degrees
    float targetPre2;  // in degrees
    float dirShift;  // = in degrees

    int numTrialsPre;
    int numTrialsAdapt;
    int numTrialsAdapt2;
    int numTrialsPost;
    //unsigned int numTrialsPrelearn = 1200;
    int numTrialsPrelearn;

    float xcur,ycur;   // percieved endpoint
    float sector_thickness;
    float sector_width;
    float wmmax_fake_prelearn;
    float armReachRadius;
    int  numPhases;

    bool fake_prelearn;
    bool sector_reward;
    bool action_change1;
    bool endpoint_rotation1;
    bool target_rotation1;
    bool action_change2;
    bool endpoint_rotation2;
    bool target_rotation2;

    int dirShiftInc;

    public:
    int turnOnCues(float * cues);
    float getSuccess(float * x,float * y,unsigned int k,float *addInfo);
    float getReward(float success, float * x,float * y, float & param);  
    void setParams(int argc, char** argv){}
    void prelearn(int nTrials, float * addInfo);
    void runSession();

    //void setParams();
    int deg2action(float degAngle);

    testExperimentEnv(string paramsEnvFile, int num_sess, float tgt);
    ~testExperimentEnv();

    // experiment-specific
    void initCBdir(float dir, bool resetState);
};

void runExperiment(int argc, char** argv);

enum experimentPhases {PRE1=0, PRE2=1, ADAPT1=2, POST1=3,  ADAPT2=4, POST2=5, PRELEARN=6};
