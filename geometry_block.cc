#include <math.h>
#include "par.h"

using namespace std;

void geometry_block(float * theta,float * thetaV, float* L,float* V, float * Hup,float * Hdw)
{



//    % specify joint restrictions
	float a1min = M_PI/2-A1max;
	float a1max = M_PI/2-A1min;
	float a2min = M_PI-A2max;
	float a2max = M_PI-A2min;


// % muscles map
// % [shoulder_flexor shoulder_extensor elbow_flexor elbow_extensor
// % bifunc_flexor bifunc_extensor] 

    double lm[2] = {0.95*A1max-0.95*A1min, 0.95*A2max-0.95*A2min};              // %range
    double phi[2] = {M_PI/2-theta[0], M_PI-theta[1]};
    double alpha[4] = {phi[0]-A1min, A1max-phi[0], phi[1]-A2min, A2max-phi[1]};
    
    for(int k=0;k<4;k++)
      alpha[k] = (alpha[k]>0)? alpha[k] : 0;

//     a = {alpha[0]/lm[0], alpha[1]/lm[0], alpha[1/lm[0], alpha[2]/lm[1], (alpha[0]+alpha[2])/(lm[0]+lm[1]), (alpha[0]+alpha[3])/(lm[0]+lm[1])};
    
       L[0] = (alpha[0]/lm[0])*Lopt[0];
       L[1] = (alpha[1]/lm[0])*Lopt[1];
       L[2] = (alpha[2]/lm[1])*Lopt[2];
       L[3] = (alpha[3]/lm[1])*Lopt[3];
       L[4] = ((alpha[0]+alpha[2])/(lm[0]+lm[1]))*Lopt[4];
       L[5] = ((alpha[1]+alpha[3])/(lm[0]+lm[1]))*Lopt[5];
  
                                  // %muscle lenghts
//    vA = {-thetaV[0], -thetaV[1]};                   // %positive then extension, negative then flexion
//    V = {vA[0]*R[0], vA[0]*R[1], vA[1]*R[2], vA[1]*R[3], vA[1]*R[4]+vA[1]*R[6], vA[0]*R[5]+vA[1]*R[7]};//  %muscles velocities    
	V[0] = -thetaV[0]*R[0];
	V[1] = -thetaV[0]*R[1];
	V[2] = -thetaV[1]*R[2];
	V[3] = -thetaV[1]*R[3];
	V[4] = -(thetaV[0]*R[4] + thetaV[1]*R[6]);
	V[5] = -(thetaV[0]*R[5] + thetaV[1]*R[7]);
    
	
    Hup[0] = R[0]; Hup[1] = R[1]; Hup[2]= 0;Hup[3] = 0;Hup[4] = R[4];Hup[5] = R[5];
    Hdw[0] = 0; Hdw[1] = 0; Hdw[2] = R[2]; Hdw[3] = R[3]; Hdw[4] = R[6]; Hdw[5]= R[7];


}
