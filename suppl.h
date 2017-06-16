#ifndef SUPPL_H
#define SUPPL_H

#define _USE_MATH_DEFINES

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <stdlib.h>

#include <vector>
#include <cmath>
// suppl.h 
using namespace std;

void movingAverageFilter(float * input, unsigned int len, unsigned int windowSize, float * output);
inline float weightRpre(unsigned int tau,float expCoefRpre, float normFactor) // number of steps back. Should be positive
{ return exp(-expCoefRpre * tau)*normFactor; }


std::string trim_spaces(std::string& str);

typedef map<string,string> parmap;

#ifndef RECALIB

#include "BG_model.h"
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/tokenizer.hpp>

#include <random>
#include <functional>
#include <thread>


//inline std::mt19937 get_prng() {
//    std::random_device r;
//    std::seed_seq seed{r(), r(), r(), r(), r(), r(), r(), r()};
//    return std::mt19937(seed);
//}

typedef std::mt19937 rng_type;
extern boost::variate_generator<rng_type&, boost::uniform_real<> > uni;
extern thread_local rng_type rng;   // thread-specific RNG

//extern boost::variate_generator<rng_type&, boost::uniform_real<> > uniInt;
//extern thread_local rng_type rng;   // thread-specific RNG

// boost::random::uniform_int_distribution<> six(1,6);

#define RND_BOOST

#endif

#ifdef RND_BOOST
inline float rnd() { 
//boost::uniform_real<> uni_dist(0,1);
//rng_type rng = get_prng();
    return uni(); } 
#else
inline float rnd() { return 1.*rand()/(RAND_MAX+1.); } 
#endif

inline float fmod_(float f,float reminder) 
{ 
  if( f >= 0 )
  { 
    while(f>=reminder)
    {
      f-= reminder;
    }
  }
  else
  {
    while(f<0.)
    { 
      f+= reminder;
    }
  }
  return f;
} 

inline float fmodAng(float f){return fmod_(f,360.);} 

inline float angDegAdd(float ang1, float ang2){return fmodAng(ang1+ang2);}

void parseCMDargs(int argc, char ** argv, parmap & params);

//inline float dirDiff(float ang1, float ang2) // in deg
//{
//  float res = 0;
//  if(ang1 < 180 && ang2 < 180)
//  {
//    res = ang1-ang2;
//  }
//  else if(ang1 < 180 && ang2 > 180)
//  {
//    res = fmin(  ang1 + (360 - ang2) );
//  }
//
//  return res;
//}

void readIni(string fname, parmap & configValues);

int genCortcalData(parmap & params);

void shuffle(vector<int> & elements);

void parseCueList(std::string str, vector<int> & cues, vector<int> & numShows, vector<int> & feedbackOn);

inline int getActiveCueInd(float * x, int nc)
{
  for(int i=0; i<nc; i++)
  {
    if(x[i]>0.01)
    {
      return i;
    } 
  }
  return -1;
}

#endif
