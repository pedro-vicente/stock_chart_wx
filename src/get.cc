#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "ssl_read.hh"
#include "get.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// get_intraday
// https://www.alphavantage.co/query?function=TIME_SERIES_INTRADAY&symbol=IBM&interval=5min&month=2009-01&outputsize=full&apikey=demo
/////////////////////////////////////////////////////////////////////////////////////////////////////

int get(const std::string& key, const std::string& month)
{
  const std::string host = "www.alphavantage.co";
  const std::string port_num = "443";
  std::stringstream http;
  if (month.empty())
  {
    http << "GET /query?function=TIME_SERIES_INTRADAY&symbol=SPY&interval=60min&outputsize=full&apikey=" << key << "&datatype=csv";
  }
  else
  {
    http << "GET /query?function=TIME_SERIES_INTRADAY&symbol=SPY&interval=60min&month=" << month << "&outputsize=full&apikey=" << key << "&datatype=csv";
  }
  http << " HTTP/1.1\r\n";
  http << "Host: " << host;
  http << "\r\n";
  http << "Accept: */*\r\n";
  http << "Connection: close\r\n\r\n";
  std::cout << http.str() << std::endl;

  std::string buf;
  ssl_read(host, port_num, http.str(), buf);

  if (!buf.size())
  {
    return -1;
  }

  std::cout << buf.c_str() << std::endl;
  buf.erase(std::remove(buf.begin(), buf.end(), '\r'), buf.end());
  std::cout << buf.c_str() << std::endl;

  const std::string fname = "intraday.csv";
  std::ofstream ofs(fname);
  ofs << buf;
  ofs.close();
  reverse(fname);
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// reverse
/////////////////////////////////////////////////////////////////////////////////////////////////////

void reverse(const std::string& fname)
{
  std::ifstream ifs(fname);
  if (!ifs.is_open())
  {
    return;
  }
  std::string header;
  std::getline(ifs, header);
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(ifs, line))
  {
    lines.push_back(line);
  }
  ifs.close();
  std::reverse(lines.begin(), lines.end());

  std::ofstream ofs("SPY_intraday.csv");
  ofs << header << '\n';
  for (size_t idx = 0; idx < lines.size(); ++idx)
  {
    ofs << lines[idx] << '\n';
  }
  ofs.close();

}
