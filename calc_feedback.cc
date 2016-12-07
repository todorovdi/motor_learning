


// Map output vector to input vector for neural network
// Calculate muscle forces
// MN - motoneuronal activation
// L - lenght of muscles
// V - muscles velocity

#include <math.h>
#include "par.h"

using namespace std;


void calc_feedback( float* l,float* v, float * mn, float* f,float* fbIa,float* fbIb,float* fbII)

 {
        float d_Ia[N_Mus], IaV[N_Mus], II[N_Mus], Ib[N_Mus];
	float  IaL[N_Mus],signv[N_Mus],IIL[N_Mus],IIEMG[N_Mus];
	float  IaEMG[N_Mus], Ia[N_Mus],   d_II[N_Mus];
    for(int k=0;k<6;k++)
      {

       d_Ia[k]= (l[k]-Lrest_I[k])/Lopt[k]; // %normalize d_Ia 
       signv[k] = (v[k] > 0) ? 1 : ((v[k] < 0) ? -1 : 0);
       IaV[k]= kV_Ia[k]* signv[k]* pow(fabs(v[k]/Vmax_Ia[k]),pV_Ia[k]);
       IaL[k] = ((kL_Ia[k]*d_Ia[k])>0)? kL_Ia[k]*d_Ia[k] : 0;
       IaEMG[k] = kEMG_Ia[k]*mn[k];
       Ia[k]=((IaV[k]+IaL[k]+IaEMG[k]+C_Ia[k])>0)? IaV[k]+IaL[k]+IaEMG[k]+C_Ia[k] : 0;
       d_II[k] = (l[k]-Lrest_II[k])/Lopt[k]; // %normalize d_II
       IIL[k] =((kL_II[k]*d_II[k])>0)? kL_II[k]*d_II[k] : 0;
      IIEMG[k] = kEMG_II[k]*mn[k];
      II[k] = IIL[k]+IIEMG[k]+C_II[k];
      Ib[k] =(((f[k]-Fconst_Ib[k] )/Fmax[k])>0)? (f[k]-Fconst_Ib[k] )/Fmax[k] : 0;  
      fbIa[k]=Ia[k];
      fbIb[k]=Ib[k];
      fbII[k]=II[k];
     
      
        }
  }
   
