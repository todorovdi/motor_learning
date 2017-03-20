#include "suppl.h"
#include "BG_model.h"

#ifdef RND_BOOST
thread_local std::random_device rd;    // used once only per thread to initialise RNG
//std::random_device rd;    
thread_local rng_type rng(rd());   // thread-specific RNG
boost::uniform_real<> uni_dist(0,1);
boost::variate_generator<rng_type&, boost::uniform_real<> > uni(rng,uni_dist);
#endif

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

std::string trim_spaces(std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

void readIni(std::string fname, parmap & configValues)
{
    std::ifstream fileStream(fname);

    std::string line;
    while (std::getline(fileStream, line))
    {
        std::istringstream is_line(line);
        std::string key;
        if (std::getline(is_line, key, '='))              // key=value, no spaces
        {
            std::string value;
            if (key[0] == '#')
                continue;

            if (std::getline(is_line, value, '#'))
            {
              key = trim_spaces(key);
              value = trim_spaces(value);
              parmap::iterator iter = configValues.find(key);
              if(iter!= configValues.end())
              { 
                cout<<"readIni overwriting key "<<key<<" old val = "<<iter->second<<" new val = "<<value<<endl;
              } 
              configValues[key] = value;
            }
        }
    }
}

void parseCMDargs(int argc, char ** argv, parmap & configValues)
{
  for(int i = 0; i<argc; i++)
  {
    string s = argv[i];
    if( s.length() >= 5 && s[0] == '-' && s[1] == '-' )  // --a=b
    {
      std::istringstream is_line(argv[i]+2);
      std::string key, value;
      if (std::getline(is_line, key, '='))   // key=value, no spaces
      {
        if (std::getline(is_line, value))
        {
          key = trim_spaces(key);
          value = trim_spaces(value);
          parmap::iterator iter = configValues.find(key);
          if(iter!= configValues.end())
          { 
            cout<<"parseCMDargs overwriting key "<<key<<" old val = "<<iter->second<<" new val = "<<value<<endl;
          } 
          configValues[key] = value;
        }
      }
    }
  }
}
