#include "stock_chart.hh"
#include "tls_socket.hh"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include <algorithm>
#include <memory.h>

enum csv_header
{
  TIMESTAMP, OPEN, HIGH, LOW, CLOSE, VOLUME
};

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

  char query[127];
  const char *outputsize = "full";
  const char *ticker = "^DJI";
  int interval = 60;
  time_series series = TIME_SERIES_INTRADAY;
  if (series == TIME_SERIES_DAILY)
  {
    snprintf(query, sizeof query,
      "/query"
      "?function=TIME_SERIES_DAILY"
      "&symbol=%s"
      "&outputsize=%s"
      "&datatype=csv"
      "&apikey=%s",
      ticker, outputsize, api_key.c_str());
  }
  else if (series == TIME_SERIES_INTRADAY)
  {
    snprintf(query, sizeof query,
      "/query"
      "?function=TIME_SERIES_INTRADAY"
      "&symbol=%s"
      "&interval=%dmin"
      "&datatype=csv"
      "&apikey=%s",
      ticker, interval, api_key.c_str());
  }
  else
  {
    assert(0);
  }

  std::string request = "GET ";
  request += query;
  request += " HTTP/1.1\r\nHost: www.alphavantage.co\r\nConnection: close\r\n\r\n";
  std::cout << request << "\n";

  tls_socket_t ssl;
  ssl.open("www.alphavantage.co");
  ssl.send(request.c_str());
  ssl.receive();
  return 0;
}

