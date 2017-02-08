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

    float xc,yc;

    int na;

    public:
        Arm();
        void move(float * y, float* out, float  wcb[][6], float ffield);  
        void init(string iniFile, int na, bool oldVerIni=false);
        void getReachCenterPos(float &x, float&y);
};

#endif // HAND_H
