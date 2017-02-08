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

#endif
