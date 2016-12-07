

#include <math.h>
#include "par.h"
#include <stdlib.h>
using namespace std;

  void sp_cord( float* fbIa,float* fbIb, float* DR,  float* MN)
  {
        //  Calculating Weighted input
	float WDRMN [6][6], WDRIa [6][6], WDRIb [6][6], WMNRC [6][6];
        float WRCIa [6][6], WRCMN [6][6], WRCRC [6][6];
	float WIaIa [6][6], WIaMN [6][6], WIbMN [6][6];
	float WfbIaMN [6][6], WfbIaIa [6][6], WfbIbIb [6][6],WMNMN [6][6];
	

	 ParWeights( WDRMN,WDRIa,  WDRIb , WMNRC, WRCIa, WRCMN ,  WRCRC ,
		    WIaIa ,  WIaMN ,  WIbMN , WfbIaMN,  WfbIaIa, WfbIbIb ,  WMNMN);

 
        float Y_DRMN[N_Mus]={0,0,0,0,0,0}, Y_DRIa[N_Mus]={0,0,0,0,0,0}, Y_DRIb[N_Mus]={0,0,0,0,0,0},Y_MNRC[N_Mus]={0,0,0,0,0,0};
	float Y_RCIa[N_Mus]={0,0,0,0,0,0},Y_RCMN[N_Mus]={0,0,0,0,0,0},Y_RCRC[N_Mus]={0,0,0,0,0,0},Y_IaIa[N_Mus]={0,0,0,0,0,0};
        float Y_IaMN[N_Mus]={0,0,0,0,0,0},Y_IbMN[N_Mus]={0,0,0,0,0,0}, Y_fbIaMN[N_Mus]={0,0,0,0,0,0}, Y_fbIaIa[N_Mus]={0,0,0,0,0,0},Y_fbIbIb[N_Mus]={0,0,0,0,0,0};
	    
        float RC [N_Mus], Ia [N_Mus], Ib [N_Mus],  Y_MN [N_Mus],  Y_RC [N_Mus], Y_Ia [N_Mus], Y_Ib [N_Mus];
	

      for(int j=0;j<N_Mus;j++)
        { RC[j]=0.001*rand()/(float)RAND_MAX;  // randomize the outputs of the neurons (try to avoid bistability) 
          Ia[j]=0.001*rand()/(float)RAND_MAX; 
          Ib[j]=0.001*rand()/(float)RAND_MAX;  	
        }
        
           WeightedInput(WDRMN, DR, Y_DRMN);
	   WeightedInput(WDRIa, DR, Y_DRIa);
	   WeightedInput(WDRIb, DR, Y_DRIb);
	   WeightedInput(WfbIaMN, fbIa, Y_fbIaMN);
	   WeightedInput(WfbIaIa, fbIa, Y_fbIaIa);
           WeightedInput(WfbIbIb, fbIb, Y_fbIbIb);

  
 float resSum1=0, resSum2=0, resSum3=0, resSum4=0;
    do {
           WeightedInput(WMNRC, MN, Y_MNRC);
	   WeightedInput(WRCIa, RC, Y_RCIa);  
	   WeightedInput(WRCMN, RC, Y_RCMN);  
	   WeightedInput(WRCRC, RC, Y_RCRC);  
	   WeightedInput(WIaIa, Ia, Y_IaIa);
	   WeightedInput(WIaMN, Ia, Y_IaMN);
	   WeightedInput(WIbMN, Ib, Y_IbMN);

         

           for(int j=0;j<N_Mus;j++)
	    {
	     Y_MN[j]=Y_DRMN[j]+ Y_RCMN[j]+Y_IaMN[j]+Y_IbMN[j]+Y_fbIaMN[j];
	     Y_RC[j]=Y_MNRC[j]+Y_RCRC[j];
	     Y_Ia[j]=Y_DRIa[j]+Y_RCIa[j]+Y_IaIa[j]+Y_fbIaIa[j];
	     Y_Ib [j]=Y_DRIb[j]+Y_fbIbIb[j];
             }
	     
                Activationfunction( Y_MN, MN, resSum1);
	        Activationfunction( Y_RC, RC,resSum2);
                Activationfunction( Y_Ia, Ia, resSum3);
	        Activationfunction( Y_Ib, Ib, resSum4);

       } while ((resSum1+resSum2+resSum3+resSum4)>0);

 


  }


