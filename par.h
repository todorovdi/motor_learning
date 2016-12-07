
// % Specify parameters of 2-joints arm (masses and lengths)

const float g = 9.81;                  // %m/sec^2, gravity constant
const float M1 = 0.955;                // %kg, mass (upper segment)
const float M2 = 0.63;                 // %kg, mass (lower segment)
const float L1 = 0.4035;               // %m, length (upper segment)
const float L2 = 0.325;                // %m, length (lower segment)
const float D1 = L1/2;                 // %m, center mass (upper segment)
const float D2 = L2/2;                 // %m, center mass (lower segment)
const float I1 = (M1*L1*L1)/3;         // %moment of inertia, about fixed end (upper segment)
const float I2 = (M2*L2*L2)/12;        // %moment of inertia, about axis through center of mass (lower segment)
const float L1g = 0;                   // %projection L1 onto gravity vector
const float D1g = 0;                   // %projection D1 onto gravity vector
const float D2g = 0;                   // %projection D2 onto gravity vector

// % Specify restriction angles for upper and lower joints

const float A1min = (180-145)*M_PI/180; // %upper joint flexed (180-145)
const float A1max = (180+45)*M_PI/180;  // %upper joint extended (180+45)
const float A2min = (180-155)*M_PI/180; // %lower joint flexed (180-155)
const float A2max = (180+5)*M_PI/180;   // %lower joint extended (180+5)

// % Specify joint viscocity & joint restriction (elastic and viscosity components)

const float BJ = 0.05;

//const float KJr = 3.0;
//const float BJr = 50.0;

const float KJr = .3;
const float BJr = 1;

// % Specify properties of the muscle model
const float Fmax [6]= {420.0,  570.0, 1010.,  1880., 460.0,  630.0};  //  %maximal forces (N)
const float Lopt  [6]= {0.185,  0.170, 0.180,  0.055, 0.130,  0.150};   // % optimal lengths (m)
const float R [8]= {0.015, -0.008, 0.035, -0.021, 0.020, -0.005, 0.036, -0.021};  // %average moment arms (m)
const float Beta = 2.3, Omega = 1.26, Ro = 1.62;                   // %length active component
const float Av1 = 0.17, Bv1 = -0.69,Bv2 = 0.18;                 //   %velocity active component


// % Specify parameters of neural network
const int N_Mus = 6; //%number of muscles
const float eleak = -0.28; //%default bias 
const float EpsY = 1e-05;;
const float V12 = 0.5;
const float Slope = 0.15;
const float B [6] = {eleak,eleak,eleak,eleak,eleak,eleak};
const float Z [6] = {1,1,1,1,1,1};
// %============== To Solve the Drives.

 // %Ia feedback 
const float Lrest_I [6]={0.037,    0.0340,    0.0360,    0.0110,    0.0260,    0.0300}; // 0.2*Lopt;
// %velocity component
const float Vmax_Ia [6]= {2.2, 1.2, 5.9, 3.5, 6.7, 3.7};
const float pV_Ia [6]= {0.6, 0.6, 0.6, 0.6, 0.6, 0.6};
const float kV_Ia [6]= {1.8, 1.8, 1.8, 1.8, 1.8, 1.8};
// %lenght component
const float kL_Ia [6]= {0.8, 0.8, 0.8, 0.8, 0.8, 0.8};
const float kEMG_Ia [6] = {0.05, 0.05, 0.05, 0.05, 0.05, 0.05};
const float C_Ia [6]= {0.01, 0.01, 0.01, 0.01, 0.01, 0.01};

// %II feedback (range between 0,0.95)
const float Lrest_II [6]= {0.0185,    0.0170,    0.0180,    0.0055,    0.0130,    0.0150};  // 0.1*Lopt;
const float kL_II  [6]= {0.8, 0.8, 0.8, 0.8, 0.8, 0.8};
const float kEMG_II [6] = {0.05, 0.05, 0.05, 0.05, 0.05, 0.05};
const float C_II  [6]= {0.01, 0.01, 0.01, 0.01, 0.01, 0.01};

// %Ib feedback (range between 0,0.9)

 const float Fconst_Ib [6] = {21.0000,   28.5000,   50.5000,   94.0000,   23.0000,   31.5000}; // 0.05*Fmax; 



void AnglesVelocityAcceleration(float  x1,float x2, float y1,float y2, float T,float n, float * t1,float * t2, float * t1v,float * t2v,float * t1a,float * t2a);
void muscles(float*,float*,float*,float*);
void geometry_block(float*,float*,float*,float*,float*,float*);
void Inverse_muscles(float * MN, float* L,float* V, float* F);
void Force_MN( float* l,float* v,float q1m,float q2m,float  TqRatio, float  BaseL_Activation, float* f,float* mn);
//void calc_feedback ( float* l,float* v, float * mn, float* f, float FB[][3]);
void calc_feedback( float* l,float* v, float * mn, float* f,float* fbIa,float* fbIb,float* fbII);
void ParWeights(float WDRMN [][6],float WDRIa [][6], float WDRIb [][6], float WMNRC [][6],float WRCIa [][6], float WRCMN [][6], float WRCRC [][6],
                      float WIaIa [][6], float WIaMN [][6], float WIbMN [][6],float WfbIaMN [][6], float WfbIaIa [][6],float WfbIbIb [][6], float WMNMN [][6] );
void WeightedInput(float  W[][6], float* X, float* SumWX);
void sp_cord( float* fbIa,float* fbIb, float* DR,  float* MN);
void Activationfunction( float* Y_cell, float* CellOutput, float resSum);

void reach_init();
void reach(float* x,float* y,float alpha, float wcb[][6]);

