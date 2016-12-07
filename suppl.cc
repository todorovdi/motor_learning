#include "suppl.h"

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
