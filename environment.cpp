#include "environment.h"

Environment::Environment()
{
    numTrials = 100;
    successDist = 0.1;
}

void Environment::prelearn(int nTrials, float * addInfo)
{
    ml.backupWeights();
} 

Environment::Environment(string paramsFile, int num_sess_)
{
    readIni(paramsFile,params);

    readIni(params["iniBG"],params);
    readIni(params["iniCB"],params);
    readIni(params["iniArm"],params);
    readIni(params["iniML"],params);


    exporter.init(params["prefix"],params["suffix"],params["output_dir"]);
    exporter.setCounts(stoi(params["na"]),stoi(params["nc"]));
    ml.init(this,&exporter,params);

    rewardDist = stof(params["rewardDist"]);
    nc = stof(params["nc"]);
    na = stof(params["na"]);

    num_sess = num_sess_;
}

