#include "environment.h"

//Environment::Environment(Hand * arm_)
Environment::Environment()
{
    //exporter(100,1),ml(this,&exporter,"paramsBG.ini","paramsCB.ini")
    nsessions = 1;
    numTrials = 100;
    successDist = 0.1;
    prelearnEachTime = false;

    //arm = arm_;
}

void Environment::prelearn(int nTrials, float * addInfo)
{
    ml.backupWeights();
} 

Environment::Environment(string paramsEnvFile, int num_sess_)
    //:ml(this,&exporter,paramsEnv["iniBG"], paramsEnv["iniCB"]),exporter(stoi(paramsEnv["na"]),stoi(paramsEnv["nc"]))                       
{
    readIni(paramsEnvFile,paramsEnv);
    ml.init(this,&exporter,paramsEnv);
    exporter.init(paramsEnv["suffix"],paramsEnv["output_dir"]);
    exporter.setCounts(stoi(paramsEnv["na"]),stoi(paramsEnv["nc"]));

    rewardDist = stof(paramsEnv["rewardDist"]);
    nc = stof(paramsEnv["nc"]);
    na = stof(paramsEnv["na"]);

    num_sess = num_sess_;
}

//void Environment::setModel(MotorLearning * ml_)
//{
//    ml = ml_;
//}

//float Environment::moveHand(float * y, float* out, float wcb[][6], float ffield)  
//{
//    arm->move(y,out,ffield);
//}
