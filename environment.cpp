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

Environment::Environment(string paramsFile, int num_sess_)
    //:ml(this,&exporter,params["iniBG"], params["iniCB"]),exporter(stoi(params["na"]),stoi(params["nc"]))                       
{
    readIni(paramsFile,params);

    readIni(params["iniBG"],params);
    readIni(params["iniCB"],params);
    readIni(params["iniArm"],params);
    readIni(params["iniML"],params);


    exporter.init(params["suffix"],params["output_dir"]);
    exporter.setCounts(stoi(params["na"]),stoi(params["nc"]));
    ml.init(this,&exporter,params);

    rewardDist = stof(params["rewardDist"]);
    nc = stof(params["nc"]);
    na = stof(params["na"]);

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
