
// Weight values
// antogonists map

#include <iostream>
#include <fstream>


using namespace std;


void ParWeights(float WDRMN [][6],float WDRIa [][6], float WDRIb [][6], float WMNRC [][6],float WRCIa [][6], float WRCMN [][6], float WRCRC [][6],
                      float WIaIa [][6], float WIaMN [][6], float WIbMN [][6],float WfbIaMN [][6], float WfbIaIa [][6],float WfbIbIb [][6], float WMNMN [][6] )
{
const float ang_map [6][6]= {
                           {0, 1, 0, 0, 0, 0},
                           {1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 1, 0, 0},
                           {0, 0, 1, 0, 0, 0},
                           {0, 0, 0, 0, 0, 1},
                           {0, 0, 0, 0, 1, 0}
                          };
//  agonists map
const float ag_map [6][6]= {
                           {0, 0, 1, 0, 1, 0},
                           {0, 0, 0, 1, 0, 1},
                           {1, 0, 0, 0, 1, 0},
                           {0, 1, 0, 0, 0, 1},
                           {1, 0, 1, 0, 0, 0},
                           {0, 1, 0, 1, 0, 0}
                          };
// Excitation from upper level

  for(int i=0;i<6;i++)
   {  for(int j=0;j<6;j++)
     {
       WDRMN[i][j]=(i==j)?1.65:0;
       WDRIa[i][j]=(i==j)?0.3:0;
       WDRIb[i][j]=(i==j)?0.3:0;
   
     }
  }
// Motoneurons connections, excitation MN->RC

  for(int i=0;i<6;i++) for(int j=0;j<6;j++) WMNRC[i][j]=(i==j)?0.25:0;

// Renshaw cells connections

  for(int i=0;i<6;i++)
   {  for(int j=0;j<6;j++)
     {
       WRCIa[i][j]=(i==j)?-0.25:0; // inhibition RC->Ia
       WRCMN[i][j]=(i==j)?-0.25:0;  // inhibition RC->MN 
        WRCMN[i][j]-=0.125*ag_map[i][j];
       WRCRC[i][j]=-0.25*ang_map[i][j]; //  inhibition RC->RC ("antogonists")
      
    }
  }

// Ia reciprocal inhibition

  for(int i=0;i<6;i++)
   {  for(int j=0;j<6;j++)
     {
      WIaIa[i][j]=-0.25*ang_map[i][j]; // inhibition on antogonists Ia->Ia
       WIaMN[i][j]=-0.25*ang_map[i][j]; // inhibition on antogonists Ia->MN
      
     }
  }



// Ib non-reciprocal inhibition

  for(int i=0;i<6;i++)
   {  for(int j=0;j<6;j++)
     {
       WIbMN[i][j]=(i==j)?-0.25:0; // inhibition RC->Ia
   
      WIbMN[i][j]-=0.125*ag_map[i][j];
     }
  }

// Feedbacks connections

  for(int i=0;i<6;i++)
   {  for(int j=0;j<6;j++)
     {
     
      WfbIaMN[i][j]=(i==j)?0.15:0;
       WfbIaIa[i][j]=(i==j)?0.15:0;
      WfbIbIb[i][j]=(i==j)?0.15:0;
     
     }
  }


// MN to MN

  for(int i=0;i<6;i++)
   {  for(int j=0;j<6;j++)
     {
      WMNMN[i][j]=0; 
     
     }
  }


}
