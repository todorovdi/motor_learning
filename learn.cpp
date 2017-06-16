// learn.cpp

#define _USE_MATH_DEFINES

//#include <boost/program_options.hpp>
#include <time.h>
#include "suppl.h"

#ifndef RECALIB
#include "CB_model.h"
#include "motor_learning.h"
#include "suppl.h"
#include "exporter.h"
#include "environment.h"
#include "pert.h"
#endif

//using namespace std;

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

    cout<<"cmd line args are "<<endl;
    for(int i=0;i<argc;i++)
    {
      cout<<argv[i]<<endl;
    }
    
    parmap params_pre;
    parseCMDargs(argc,argv,params_pre);

    parmap::iterator i;
    
    bool notfound = false;
    i = params_pre.find("ini");
    string paramsEnvFile = "";
    if (i != params_pre.end())
    { 
        paramsEnvFile =i->second;
        if(!paramsEnvFile.length())
        {
            // I want to get exception
            //paramsEnvFile = defParamFile;
            notfound = true;
        }
    }
    else
    {
      // I want to get exception
      //paramsEnvFile = defParamFile;
      notfound = true;
    }

    if(notfound)
    {
      cout<<" INI FILE ARGUMENT NOT PRESENT! "<<endl;
    }

    parmap params;
    readIni(paramsEnvFile,params);

    /////////   before I used several files to store parameters
    // so just to maintain backward compatibility
    i = params.find("iniBG");
    if(i!= params.end())
        readIni(i->second,params);

    i = params.find("iniCB");
    if(i!= params.end())
        readIni(i->second,params);

    i = params.find("iniArm");
    if(i!= params.end())
        readIni(i->second,params);

    i = params.find("iniML");
    if(i!= params.end())
        readIni(i->second,params);

    i = params.find("iniAdd");
    if(i!= params.end())
        readIni(i->second,params);

    readIni(paramsEnvFile,params);   // again, to replace what was in ini* files. To modify common params in general

    parseCMDargs(argc,argv,params);  // to overwrite ini file options

    cout<<"pdfSuffix="<<params["pdfSuffix"]<<endl;
    params["datPrefix"] = params["pdfSuffix"]; 

    //int nsessions = vm["n"].as<int>(); 
    //params["nsessions"] = to_string(nsessions); 

    string key;
    parmap::iterator iter;

    key = string("recalibrateArmCortControl");
    iter = params.find(key);
    if(iter!=params.end() && atoi(iter->second.c_str()))
    {
      genCortcalData(params);
    }
#ifndef RECALIB
    else
    { 
      runExperiment(params);
    }
#endif // RECALIB

    clock_t end = clock();
    cout<<"Calc finished, clock time (in sec) passed is "<<(end-start)/CLOCKS_PER_SEC<<endl;
    return 0;
}

