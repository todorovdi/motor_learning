#include "suppl.h"
#include "BG_model.h"

float **wmBackup, **w2Backup, **w1Backup;
float* Rpre;

// using only prev data
void movingAverageFilter(float * input, unsigned int len, unsigned int windowSize, float * output)
{
    for(unsigned int i = 0; i<len; i++)
    {
        output[i] = 0;
        unsigned int n = std::min(windowSize,i+1);
        for(unsigned int j = 0; j<n; j++)
        {
            output[i] += input[i-j]/float(n);
        }
    }
}


float gauss()
{
	static int flag=0;
 	static float x1, x2, w, y1, y2;

 	if(flag)
 	{
 		flag=0;
 		return y2;
 	}
 	flag=1;
 
    do 
    {
    	x1 = 2.0 * rnd() - 1.0;
        x2 = 2.0 * rnd() - 1.0;
        w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );

    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    return y1;
}


