#include "environment.h"

//Environment::Environment(Hand * hand_)
Environment::Environment()
{
    nsessions = 1;
    numTrials = 100;
    successDist = 0.1;

    //hand = hand_;
}

//float Environment::moveHand(float * y, float* out, float wcb[][6], float ffield)  
//{
//    hand->move(y,out,ffield);
//}
