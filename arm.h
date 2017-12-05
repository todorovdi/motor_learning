#ifndef HAND_H
#define HAND_H

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include "suppl.h"
#include "par.h"
#include <map>
#include <string>

using std::string;
using std::map;
typedef map<string,string> parmap;

class Arm
{
    float phi0[2];
    float neuron2armMult;
    float finalNoiseAmpl; // amplitude of noise applied to the final arm position

    float xc,yc;                     // center of the reaching movement

    bool linearArm;
    int na;

    float minAngDeg_,maxAngDeg_;
    float armReachRadius;

    bool debug_printAC;

    public:
        Arm();
        void move(float * y, float* out, float  wcb[][6], float ffield, bool noiseOn=true);   // note that there is no perception pertubations happening here, only endpoint noise is added
        // perception is perturbed when the error is computed, in percept.cpp
        
        void init(parmap & params); 
        void getReachCenterPos(float &x, float&y);
};

#endif // HAND_H
