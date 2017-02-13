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

//inline float rnd() { 
////boost::uniform_real<> uni_dist(0,1);
////rng_type rng = get_prng();
//    return uni(); } 

inline float rnd() { return 1.*rand()/(RAND_MAX+1.); } 

#endif
