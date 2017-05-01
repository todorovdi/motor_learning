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
//                cout<<"readIni overwriting key "<<key<<" old val = "<<iter->second<<" new val = "<<value<<endl;
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

// standard Fisher-Yates modern shuffle algorithm
void shuffle(vector<int> & elements)
{
  // the counter to be used to generate the random number
  int currentIndexCounter = elements.size();
  for (auto currentIndex = elements.rbegin(); currentIndex != elements.rend() - 1;
       currentIndex++ , --currentIndexCounter)
      {
          int randomIndex = rand() % currentIndexCounter;
          // if its not pointing to the same index      
          if (*currentIndex != elements.at(randomIndex))
          {
              //then swap the elements
              swap(elements.at(randomIndex), *currentIndex);
          }
      }
}


// "cueNum:numTrialsShow:feedbackOn"   "0:10:1|1:20:0|2:30:0" 
// just to show off, I use boost for tokenizing here 
void parseCueList(std::string str, vector<int> & cues, vector<int> & numShows, vector<int> & feedbackOn)
{ 
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep("|");
  tokenizer tokens(str, sep);

  //cues.reserve(tokens.size());
  //numShows.reserve(tokens.size());
  //feedbackOn.reserve(tokens.size());
  cues.resize(0);
  numShows.resize(0);
  feedbackOn.resize(0);

  boost::char_separator<char> sep2(":");
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer2;
  for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
  {
    tokenizer2 subtokens(*tok_iter, sep2);
    tokenizer2::iterator it=subtokens.begin();
    cues.push_back(stoi(*it)); it++;
    numShows.push_back(stoi(*it)); it++;
    feedbackOn.push_back(stoi(*it));
  }

  //std::vector<std::string> cueNumList;
  //strtk::parse(str,"|",cueNumList);

  //cues.reserve(cueNumList.size());
  //cues.resize(0);
  //numShows.reserve(cueNumList.size());
  //numShows.resize(0);
  //feedbackOn.reserve(cueNumList.size());
  //feedbackOn.resize(0);

  //std::vector<std::string>::iterator it = cueNumList.begin();
  //for(;it != cueNumList.end(); it++)
  //{
  //  std::vector<int> sublist;
  //  strtk::parse(str,":",sublist);
  //  cue.push_back(sublist[0]);
  //  numShows.push_back(sublist[1]);
  //  feedbackOn.push_back(sublist[2]);
  //}
}
