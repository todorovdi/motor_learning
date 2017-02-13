// learn.cpp

#include "CB_model.h"
#include "motor_learning.h"
#include "suppl.h"
#include "exporter.h"
#include "environment.h"


#ifdef  BUILD_PIRON_ARM
#include "piron_arm.h"
#endif

#ifdef  BUILD_FORCE_FIELD
#include "force_field.h"
#endif

#ifdef  BUILD_GUTIERREZ
#define TWO_CUES
#include "gutierrez.h"
#endif

#ifdef  BUILD_GALEA
#include "galea.h"
#endif

#include <time.h>


using namespace std;


//thread_local std::random_device rd;    // used once only per thread to initialise RNG
std::random_device rd;    
thread_local rng_type rng(rd());   // thread-specific RNG
boost::uniform_real<> uni_dist(0,1);
boost::variate_generator<rng_type&, boost::uniform_real<> > uni(rng,uni_dist);


//float forceField = 0.;
//float *gpe, *gpi, *stn;
//float *d1,*d2;



//const int na=100; // number of actions

// init_gpi_drive=1.7,

////////////////


const float EPS = 0.0000001;

//float last_y[na] = {};


// we want to make monkey touch one of the points that are good
// thus we need to output it somehow
// it interacts badly with existing experiment code
// we have to make a framework that would allow to use the different one



int main(int argc, char** argv)
{
    // write class GaleaExperiment:public Environment
    // create MotorLearning object
    // run trials from here
        // --ini env.ini

    clock_t start = clock();
    cout<<"Calc started"<<endl;
    //cout<<"Calc started, nc = "<<nc<<" na = "<<na<<" nsessions "<<nsessions<<" numTrials = "<<numTrials<<endl;
    //bool presetSeed = false;
    //unsigned int seed =   time(NULL);  presetSeed = false; 
    //seed = 1486963713;   presetSeed = true;  
    ////seed = 1486963714;   presetSeed = true;  
    //srand(seed);
    //cout<<"seed is "<<seed<<endl;
    //if(presetSeed)
    //    cout<<"WARNING: PRESET SEED IS ACTIVE!!!!"<<endl;

    runExperiment(argc,argv);

    clock_t end = clock();
    cout<<"Calc finished, clock time (in sec) passed is "<<(end-start)/CLOCKS_PER_SEC<<endl;
    return 0;
}

