// Calculate the sum of the inputs from six different units multipled by the weight.

#include <math.h>
#include "par.h"

using namespace std;

void WeightedInput(float W[][6], float* X, float* SumWX)

 {
    for(int i=0;i<6;i++)   
    { for(int k=0;k<6;k++)
      {
       SumWX[i] += (W[i][k]*X[k]);
       
        }
   }
 }
   



