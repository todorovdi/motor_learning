

// Calculate motoneuronal activation
// MN - motoneuronal activation
// L - lenght of muscles
// V - muscles velocity
#include <math.h>
#include "par.h"

using namespace std;

void Inverse_muscles(float * MN, float* L,float* V, float* F)

 {
        float lnorm[N_Mus], Fl_active[N_Mus];
	float FL_passive[N_Mus], chooseMax[N_Mus];
	float  FV[N_Mus];
    for(int k=0;k<6;k++)
      {
       lnorm[k] = L[k]/Lopt[k]; // %normalized lenght
       Fl_active[k] =exp( -pow (fabs (  pow (lnorm[k],Beta)-1.0)/Omega, Ro) );
       FL_passive[k] = 3.5*log( exp( (lnorm[k]-1.4)/0.05 )+1.0 )- 0.02*( exp( -18.7*(lnorm[k]-0.79) )-1.0 );
         chooseMax[k]= ((lnorm[k]-1)>0)? lnorm[k]-1 : 0;
      FL_passive[k] = FL_passive[k]*chooseMax[k];  
     
 
    
        if (V[k] <= 0.0)
            {FV[k] = (Bv1-Av1*V[k])/(V[k]+Bv1);}
        else
            {FV[k] = (Bv2-(-5.34*lnorm[k]*lnorm[k]+ 8.41* lnorm[k]-4.7)*V[k])/(V[k]+Bv2);}
    
      

       MN[k]=  (( F[k]/Fmax[k])-FL_passive[k])*(1/(Fl_active[k]*FV[k]));
        }
  }
    


