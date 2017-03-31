
#include <math.h>
#include "par.h"

using namespace std;

void Force_MN( float* l,float* v,float q1m,float q2m,float  TqRatio, float  BaseL_Activation, float* f,float* mn)
{


               float BaseL_NeuronalActivation[N_Mus];
		for(int j=0;j<N_Mus;j++) BaseL_NeuronalActivation[j] = BaseL_Activation;

		float BaseL_f[N_Mus];
		muscles(BaseL_NeuronalActivation,l,v,BaseL_f);
		
		float BaseL_q1m=R[0]*BaseL_f[0]+ R[1]*BaseL_f[1]+ R[4]*BaseL_f[4]+ R[5]*BaseL_f[5];
		float BaseL_q2m=R[2]*BaseL_f[2]+ R[3]*BaseL_f[3]+ R[6]*BaseL_f[4]+ R[7]*BaseL_f[5];

		float New_BaseL_f[N_Mus]={0,0,0,0,0,0};

		if(BaseL_q1m<=0) BaseL_f[0]-=BaseL_q1m/R[0];
		else BaseL_f[1]-=BaseL_q1m/R[1];

    	if(BaseL_q2m<=0) BaseL_f[2]-=BaseL_q2m/R[2];
		else BaseL_f[3]-=BaseL_q2m/R[3];

		Inverse_muscles(BaseL_NeuronalActivation,l,v,BaseL_f);
     
		
     
		if (q1m >= 0)
         { 
           f[0]=(TqRatio*q1m)/R[0];
           f[4]=(1-TqRatio)*q1m/R[4];
           Inverse_muscles(mn, l, v, f);
       
              
            if ((mn[0]+ BaseL_NeuronalActivation[0])>=0.99)
			{
               mn[0]=0.99-BaseL_NeuronalActivation[0];
               muscles(mn, l, v, f);
               f[4]=(q1m-R[0]*f[0])/R[4];
			}
			else if ((mn[4]+ BaseL_NeuronalActivation[4])>=0.99)
			{
				mn[4]=0.99-BaseL_NeuronalActivation[4];
				muscles(mn, l, v, f);
				f[0]=(q1m-R[4]*f[4])/R[0];
			}
		} 
        else
        {
	   f[1]=(TqRatio*q1m)/R[1];
            f[5]=(1-TqRatio)*q1m/R[5];
            Inverse_muscles(mn, l, v, f);
       
			if ((mn[1]+ BaseL_NeuronalActivation[1])>=0.99)
			{
				mn[1]=0.99-BaseL_NeuronalActivation[1];
				muscles(mn, l, v, f);
				f[5]=(q1m-R[1]*f[1])/R[5];
			}
			else if ((mn[5]+ BaseL_NeuronalActivation[5])>=0.99)
			{
				mn[5]=0.99-BaseL_NeuronalActivation[5];
				muscles(mn, l, v, f);
				f[1]=(q1m-R[5]*f[5])/R[1];
			}
	}
     
		if ((q2m- (f[4]*R[6] + f[5]*R[7])) >= 0) f[2]=(q2m- (f[4]*R[6] + f[5]*R[7]))/R[2];
		else f[3]=(q2m- (f[4]*R[6] + f[5]*R[7]))/R[3];
		Inverse_muscles(mn, l, v, f);
  

		for(int j=0;j<N_Mus;j++)
		{ 
			f[j]+=BaseL_f[j];   // % total forces from tonic and motion 
			mn[j]+=BaseL_NeuronalActivation[j]; //  % total motoneuron signals from tonic and motion
			
               }
  		

}
