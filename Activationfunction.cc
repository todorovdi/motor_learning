// Calculate the sum of the inputs from six different units multipled by the weight.

#include <math.h>
#include "par.h"

using namespace std;

void Activationfunction( float* Y_cell, float* CellOutput, float resSum)
 {
 

   for(int k=0;k<6;k++)

      {
     float Y_pre=CellOutput[k];
      CellOutput[k] = B[k]+Z[k]*Y_cell[k];
        CellOutput[k] = 1/( 1+exp( -(CellOutput[k]-V12 )/Slope ));
         resSum += (fabs(CellOutput[k]-Y_pre)) > EpsY? 1:0;
        }

 // return resSum;
 }





