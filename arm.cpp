#include "arm.h"

Arm::Arm()
{
    na = 100;
    //wcb = {};
    finalNoiseAmpl = 0.01;
    neuron2armMult = 1;

    //init("");
    reach_init();
}

void Arm::init(parmap & p) //, bool oldverIni)
{
    //parmap p;
    //readIni(iniFile,p);
    
    na = stoi(p["na"]);

    string key;
    parmap::iterator it;

    key = "phi_0";
    it = p.find(key);
    if(it != p.end())
    {
      phi0[0] = stof(p["phi_0"]);
      phi0[1] = stof(p["phi_1"]);
    }
   
    key = "armInitAng_fname";
    it = p.find(key);
    if(it != p.end())
    { 
      string fname = it->second;
      ifstream(fname)>>phi0[0]>>phi0[1]; 
    }

	//float phi0[2]={ -0.832778,	1.16426};
    //if(oldverIni)
	//    ifstream(iniFile)>>phi0[0]>>phi0[1];
    //else
    
	  xc=(-L1*sin(phi0[0])+-L2*sin(phi0[1])),yc=(L1*cos(phi0[0])+L2*cos(phi0[1]));

    p["x_center"]=to_string(xc);
    p["y_center"]=to_string(yc);

    finalNoiseAmpl = stof(p["finalNoiseAmpl"]);
    neuron2armMult = stof(p["neuron2armMult"]);
}

void Arm::move(float * y, float* out, float wcb[][6], float ffield)   
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
    
    float xcur_tmp=(-L1*sin(phi[0])+-L2*sin(phi[1])); // V*(1.-2.*rnd())*5.;   
    float ycur_tmp=(L1*cos(phi[0])+L2*cos(phi[1]));

    float noisex = finalNoiseAmpl*gauss();
    float noisey = finalNoiseAmpl*gauss();

    out[0] = xcur_tmp + noisex;
    out[1] = ycur_tmp + noisey;
}

void Arm::getReachCenterPos(float &x, float&y)
{
    x = xc;
    y = yc;
}
