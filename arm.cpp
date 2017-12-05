#include "arm.h"

Arm::Arm()
{
    na = 100;
    //wcb = {};
    finalNoiseAmpl = 0.01;
    neuron2armMult = 1;
    linearArm = false;

    //init("");
}

void Arm::init(parmap & p) //, bool oldverIni)
{
    //parmap p;
    //readIni(iniFile,p);
    
    na = stoi(p["na"]);

    string key;
    parmap::iterator it;


    finalNoiseAmpl = stof(p["finalNoiseAmpl"]);
    neuron2armMult = stof(p["neuron2armMult"]);

    string fname = "AllD_CorticalData";

    key = string("armCortDataFile");
    it = p.find(key);
    if(it!=p.end())
    {
      fname = it->second;   
    }

    ifstream cortData(fname);

    if( ! cortData.good() )
    {
      cout<<"--------- Warning:  It looks like file with arm cortical controller data ("<<fname<<") does not exist"<<endl;
      cout<<"--------- Warning:  before doing computation, please calibrate cortical controller by running "<<endl;
      cout<<"make pert && pert --ini=<your ini> --recalibrateArmCortControl=1"<<endl;
      throw string("no cortical data file!");
      
      //genCortcalData(p); 
      //cortData.open(fname);
    }

    int na_;
    cortData>>minAngDeg_;
    cortData>>maxAngDeg_;
    cortData>>na_;

    string ma = p["minActionAngDeg"];
    float minAngDeg_p = ma != "" ? stof(ma) : 0; 

    string mma =  p["maxActionAngDeg"];
    float maxAngDeg_p = mma != "" ? stof(mma) : 360.; 


    bool b = fabs(minAngDeg_ - minAngDeg_p)  < EPS && 
      fabs(maxAngDeg_ - maxAngDeg_p ) < EPS &&
      na_ == stoi(p["na"]);
    if(!b)
    { 
      cout<<"--------- Warning: Arm cortical data file was generated for different parameters"<<endl;
      cout<<"--------- Warning: "<<minAngDeg_<<" "<<maxAngDeg_<<" "<<na<<endl;
      cout<<"--------- Warning:  before doing computation, please calibrate cortical controller by running "<<endl;
      cout<<"make pert && pert --ini=<your ini> --recalibrateArmCortControl=1"<<endl;

      //cortData.close();
      //genCortcalData(p); 
      //cortData.open(fname);
      //cortData>>minAngDeg_;
      //cortData>>maxAngDeg_;
      //cortData>>na_;
      throw string("wrong cortical data params");
    }
    
    reach_init(cortData);


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

    string s =  p["linearArm"];
    linearArm = s != "" ? stoi(s) : 0; 

    s =  p["armReachRadius"];
    armReachRadius = s != "" ? stof(s) : 0; 

    s =  p["debug_printAC"];
    debug_printAC = s != "" ? stoi(s) : 0; 

    
	//float phi0[2]={ -0.832778,	1.16426};
	  xc=(-L1*sin(phi0[0])+-L2*sin(phi0[1])),yc=(L1*cos(phi0[0])+L2*cos(phi0[1]));

    p["x_center"]=to_string(xc);
    p["y_center"]=to_string(yc);
}

void Arm::move(float * y, float* out, float wcb[][6], float ffield, bool noiseOn)   
{ 
    float Y[na];
    float th=0.;
    // Y -- here comes result of the BG
    for(int i=0;i<na;i++) Y[i]=(y[i]-th)*neuron2armMult;

    //////  ACTION  /////////
    // fill phi with some values, based on wht we had in Y
    
    float xcur_tmp;
    float ycur_tmp;

    if(linearArm)
    {
      float r=0,ri=0;
      float w=0,wi=0;

      for(int i=0;i<na;i++)
      {
        // summ all unit vectors
        float mult = (M_PI/180.) * (maxAngDeg_ - minAngDeg_);
        float ang = minAngDeg_*M_PI/180. + mult*float(i)/float(na);
        r+= y[i]*cos(ang);
        ri+= y[i]*sin(ang );

        //if( y[i] > 0.01 )
        //{
        //  cout<<"for action "<<i<<" y[i]="<<y[i]<<" ang is "<<ang<<" current r,ri "<<r<<", "<<ri<<endl;
        //}
      }

      //// rotate resulting vector by angle given by wcb[0][0]
      //w= cos(wcb[0][0]);  
      //wi= sin(wcb[0][0]);  
      //// multply complex numbers
      //r = r*w - ri*wi;
      //ri = r*wi + w*ri;

      //cout<<"before norm r,ri "<<r<<", "<<ri<<endl;

      // normalize
      float rmod = sqrt(r*r + ri*ri);
      r *= armReachRadius/rmod;
      ri *= armReachRadius/rmod;

      r += 0.2 * wcb[0][0];
      ri += 0.2 * wcb[0][1];

      //cout<<"r,ri "<<r<<", "<<ri<<endl;

      xcur_tmp=xc+r;
      ycur_tmp=yc+ri;
    }
    else
    {
      float phi[4]={};
      phi[0]=phi0[0]; phi[1]=phi0[1]; phi[2]=0; phi[3]=0;
      reach(phi,Y,ffield,wcb);
      
      xcur_tmp=(-L1*sin(phi[0])+-L2*sin(phi[1])); // V*(1.-2.*rnd())*5.;   
      ycur_tmp=(L1*cos(phi[0])+L2*cos(phi[1]));
    }


    float noisex=0., noisey=0.;
    if(noiseOn)
    { 
      noisex = finalNoiseAmpl*gauss();
      noisey = finalNoiseAmpl*gauss();

      if( debug_printAC )
      {
        cout<<"Arm: Noise applied size was: "<< sqrt(noisex*noisex + noisey*noisey)<<endl;
      }
    }

    out[0] = xcur_tmp + noisex;
    out[1] = ycur_tmp + noisey;
}

void Arm::getReachCenterPos(float &x, float&y)
{
    x = xc;
    y = yc;
}
