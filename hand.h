#ifndef HAND_H
#define HAND_H

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include "suppl.h"
#include "par.h"

class Hand
{
    float phi0[2];
    float neuron2armMult;
    float finalNoiseAmpl; // amplitude of noise applied to the final hand position

    float xc,yc;

    int na;

    private:
        void initHand();
    public:
        Hand();
        Hand(int na);
        void move(float * y, float* out, float  wcb[][6], float ffield);  
};

#endif // HAND_H
