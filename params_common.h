///////////////////////////////////////////
//// experiment-indepent constants
//// model tuning
///////////////////////////////////////////

float y_drive=1.3;  // before 0.9
float y_d1=2, y_d2=2;
float d1_gpi=2, gpi_drive=.2, gpi_y=1.8;
//float gpi_drive = 1.6; //init_gpi_drive - A_exp; // modify gpi_drive
float d2_gpe=2, gpe_drive=2, gpe_gpi=1.5;
float tau=1; // time constant for Wilson-Cowan

float inh1=0.; // lateral inhibition in D1
float inh2=0.; // lateral inhibition in D2
float inh21=.0; // inhibition of D1 by D2
float inh12=.0; // inhibition of D2 by D1
float inhy=1.7; // lateral inhibition in M1  

////////////////// learning consts /////////////////

float gam1=.02; // degradaion for w1
float gam2=.02; // degradation for w2

float lam1=0.2; // learning rate for w1
float lam2=0.2; // learning rate for w2, before 0.1
                    // 0.1001 -- still works
                    // 0.1002 -- completely turns off

float mgam=.001;
float mlam=.001;

float W1max=3;
float W2max=3;
float WMmax=3;

////////////// CB

float cb_learn_rate=2; // was 10

////////////  for the version where STN is a part of the indirect pathway

float stn_drive=1; // control
float gpe_stn=1;
float stn_gpi=1;

float d1_drive=0.; 
float d2_drive=0.; 
////////////// exploration-related consts

float Q = 0.02; //0.001 in old ver with dif. exploration.  1 in new version
float A_exp = 0.2; // exploratory constant [0, 1]


float expCoefRpre = 0.1;

float Rpre_coef = 0.85; 

/// arm movement constants

float finalNoiseAmpl = 0.005; //0.005; // amplitude of noise applied to the final hand position
float neuron2armMult = 1.2;  // =S in prev Slava models it was = 1.3
float initAng[4] ={ -0.832778,	1.16426, 0, 0};

////////////////////////////////////////
////////////////////////////////////////
////////////////////////////////////////
