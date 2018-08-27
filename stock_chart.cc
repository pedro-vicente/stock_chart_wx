#include "stock_chart.hh"
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include <algorithm>
#include <memory.h>

int connect_alpha(const char *ticker, int interval, time_series series, const char *api_key, std::vector<time_price_t> &tp);

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
  std::string api_key;
  for (int i = 1; i < argc && argv[i][0] == '-'; i++)
  {
    switch (argv[i][1])
    {
    case 'k':
      api_key = argv[i + 1];
      i++;
      break;
    }
  }

  if (api_key.empty())
  {
    std::cout << "Usage: ./stock_chart -k API_KEY\n";
    exit(1);
  }

  std::vector<time_price_t> tp;
  connect_alpha("^DJI", 60, TIME_SERIES_INTRADAY, api_key.c_str(), tp);
  return 0;
}

