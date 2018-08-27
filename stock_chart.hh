#ifndef STOCK_CHART
#define STOCK_CHART 1

#include <vector>
#include <fstream>
#include <iomanip>
#include <string>
#include <assert.h>
#include <stdint.h>
#include <math.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////
//receive_t
/////////////////////////////////////////////////////////////////////////////////////////////////////

struct receive_t
{
  char *memory;
  size_t size;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////
//time_series
/////////////////////////////////////////////////////////////////////////////////////////////////////

enum time_series
{
  TIME_SERIES_INTRADAY,
  TIME_SERIES_DAILY
};

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

int connect_alpha(const char *ticker, int interval, time_series series, const char *api_key, std::vector<time_price_t> &tp);

#endif
