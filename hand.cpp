#include "hand.h"

Hand::Hand()
{
    //wcb = {};
    finalNoiseAmpl = 0.01;
    neuron2armMult = 1;

    initHand();
    reach_init();
}

Hand::Hand(int na_)
{
    na = na_;
}

void Hand::initHand()
{
	//float phi0[2]={ -0.832778,	1.16426};
	ifstream("ini")>>phi0[0]>>phi0[1];
	xc=(-L1*sin(phi0[0])+-L2*sin(phi0[1])),yc=(L1*cos(phi0[0])+L2*cos(phi0[1]));
}

void Hand::move(float * y, float* out, float wcb[][6], float ffield)   
{ 
    float Y[na];
    float th=0.;
    // Y -- here comes result of the BG
    for(int i=0;i<na;i++) Y[i]=(y[i]-th)*neuron2armMult;

    //////  ACTION  /////////
    // fill phi with some values, based on wht we had in Y
    
    float phi[4]={};
    phi[0]=phi0[0]; phi[1]=phi0[1]; phi[2]=0; phi[3]=0;
    reach(phi,Y,ffield,wcb);
    
    float xcur_tmp=(-L1*sin(phi[0])+-L2*sin(phi[1]))+ // V*(1.-2.*rnd())*5.;   
        finalNoiseAmpl*gauss();
    float ycur_tmp=(L1*cos(phi[0])+L2*cos(phi[1]))+  // V*(1.-2.*rnd())*5.;     
        finalNoiseAmpl*gauss();

    out[0] = xcur_tmp;
    out[1] = ycur_tmp;
}
