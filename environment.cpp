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

Environment::Environment(parmap & params_, int num_sess_)
{
    params = params_;
    exporter.init("","",params["output_dir"]);
    exporter.setCounts(stoi(params["na"]),stoi(params["nc"]));
    ml.init(this,&exporter,&percept,params);

    rewardDist = stof(params["rewardDist"]);
    nc = stof(params["nc"]);
    na = stof(params["na"]);

    num_sess = num_sess_;
}

