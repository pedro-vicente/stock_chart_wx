#ifndef _MODEL__
#define _MODEL__ 1

#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include <assert.h>
#include <stdint.h>
#include <math.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//time_price_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class time_price_t
{
public:
  time_t time; //unix time
  float value; //price, y scale
  std::string wave; //name
  std::string trade; //trade or '-'
  time_price_t(time_t t, float v, const std::string& w) :
    time(t),
    value(v),
    wave(w),
    trade("-")
  {
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

class model_t
{
public:
  model_t()
  {};
  std::vector<time_price_t> m_tp;
  int read(const std::string &file_name);
  void write(const std::string &file_name);
  void reset_wave(const std::string &wave);
  int m_interval; //seconds
  std::string m_period;
  std::string m_ticker;
};


#endif
