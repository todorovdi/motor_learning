#ifndef SUPPL_H
#define SUPPL_H

#include "BG_model.h"
#include <string>
#include <map>
#include <iostream>
#include <sstream>
// suppl.h 

void movingAverageFilter(float * input, unsigned int len, unsigned int windowSize, float * output);
inline float weightRpre(unsigned int tau,float expCoefRpre, float normFactor) // number of steps back. Should be positive
{ return exp(-expCoefRpre * tau)*normFactor; }


std::string trim_spaces(std::string& str);


//typedef std::map<std::string, std::string> parmap;

void readIni(string fname, parmap & configValues);


#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/generator_iterator.hpp>

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

#define RND_BOOST

#ifdef RND_BOOST
inline float rnd() { 
//boost::uniform_real<> uni_dist(0,1);
//rng_type rng = get_prng();
    return uni(); } 
#else
inline float rnd() { return 1.*rand()/(RAND_MAX+1.); } 
#endif

inline float fmod(float f,float reminder) 
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

inline float fmodAng(float f){return fmod(f,360.);} 

inline float angDegAdd(float ang1, float ang2){return fmodAng(ang1+ang2);}

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

#endif
