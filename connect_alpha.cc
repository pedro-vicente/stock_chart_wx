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

/////////////////////////////////////////////////////////////////////////////////////////////////////
//find_all
/////////////////////////////////////////////////////////////////////////////////////////////////////

void find_all(std::vector<size_t> & vec_pos, std::string buf, std::string str)
{
  size_t pos = buf.find(str);
  while (pos != std::string::npos)
  {
    vec_pos.push_back(pos);
    pos = buf.find(str, pos + str.size());
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//receive
/////////////////////////////////////////////////////////////////////////////////////////////////////

static size_t receive(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct receive_t *mem = (struct receive_t *)userp;
  mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
  return realsize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//connect_alpha
/////////////////////////////////////////////////////////////////////////////////////////////////////

int connect_alpha(const char *ticker, int interval, time_series series, const char *api_key, std::vector<time_price_t> &tp)
{
  enum csv_header
  {
    TIMESTAMP, OPEN, HIGH, LOW, CLOSE, VOLUME
  };
  CURL *curl_handle;
  struct receive_t chunk;
  char url[127];
  const char *outputsize = "full";

  if (series == TIME_SERIES_DAILY)
  {
    snprintf(url, sizeof url,
      "https://www.alphavantage.co/query"
      "?function=TIME_SERIES_DAILY"
      "&symbol=%s"
      "&outputsize=%s"
      "&datatype=csv"
      "&apikey=%s",
      ticker, outputsize, api_key);
  }
  else if (series == TIME_SERIES_INTRADAY)
  {
    snprintf(url, sizeof url,
      "https://www.alphavantage.co/query"
      "?function=TIME_SERIES_INTRADAY"
      "&symbol=%s"
      "&interval=%dmin"
      "&datatype=csv"
      "&apikey=%s",
      ticker, interval, api_key);
  }
  else
  {
    assert(0);
  }
  std::cout << url << "\n";

  chunk.memory = (char*)malloc(1);
  chunk.size = 0;
  curl_global_init(CURL_GLOBAL_ALL);
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_URL, url);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, receive);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
  if (curl_easy_perform(curl_handle) == CURLE_OK)
  {
    std::ofstream ofs("out.txt", std::ios::out | std::ios::binary);
    std::cout << chunk.size << " bytes retrieved\n";
    std::string buf(chunk.memory);
    size_t pos_buf = 0;
    size_t nbr_lines = 0;
    while (true)
    {
      size_t pos_end_line = buf.find("\n", pos_buf);
      if (pos_end_line == std::string::npos)
      {
        break;
      }
      size_t len = pos_end_line - pos_buf;
      std::string line = buf.substr(pos_buf, len);
      std::cout << line << "\n";
      ofs << line << "\n";
      pos_buf = pos_end_line + 1;
      nbr_lines++;
      if (nbr_lines == 1)
      {
        continue;
      }

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //find all positions for ","
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      std::vector<size_t> vec_pos;
      find_all(vec_pos, line, ",");

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //get time
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      std::string cell;
      std::time_t time;
      len = vec_pos[TIMESTAMP];
      cell = line.substr(0, len);
      struct std::tm tm;
      std::istringstream ss(cell);
      ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
      time = mktime(&tm);

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //get close
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      len = vec_pos[CLOSE] - vec_pos[CLOSE - 1];
      cell = line.substr(vec_pos[CLOSE - 1] + 1, len);
      float close = atof(cell.c_str());

      /////////////////////////////////////////////////////////////////////////////////////////////////////
      //store
      /////////////////////////////////////////////////////////////////////////////////////////////////////

      time_price_t tv(time, close, "-");
      tp.push_back(tv);
    }
    std::cout << nbr_lines << " lines retrieved\n";
    std::reverse(tp.begin(), tp.end());
  }
  curl_easy_cleanup(curl_handle);
  free(chunk.memory);
  curl_global_cleanup();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::read
/////////////////////////////////////////////////////////////////////////////////////////////////////

int model_t::read(const std::string &file_name)
{
  std::string str;
  std::ifstream ifs(file_name);
  if (!ifs.is_open())
  {
    assert(0);
    return -1;
  }

  std::getline(ifs, str);
  m_ticker = str;
  std::getline(ifs, str);
  m_period = str;
  std::getline(ifs, str);
  m_interval = std::stoi(str);

  std::getline(ifs, str);
  size_t nbr_rows = std::stoull(str);
  for (size_t idx = 0; idx < nbr_rows; idx++)
  {
    std::getline(ifs, str);
    size_t pos_c1 = str.find(",");
    size_t pos_c2 = str.find(",", pos_c1 + 1);
    size_t pos_c3 = str.find(",", pos_c2 + 1);
    std::string str_time = str.substr(0, pos_c1);
    std::string str_value = str.substr(pos_c1 + 1, pos_c2 - pos_c1 - 1);
    std::string str_wave;
    std::string str_trade;
    if (pos_c3 != std::string::npos)
    {
      str_wave = str.substr(pos_c2 + 1, pos_c3 - pos_c2 - 1);
      str_trade = str.substr(pos_c3 + 1);
    }
    else
    {
      str_wave = str.substr(pos_c2 + 1);
    }
    time_t time = std::stoull(str_time);
    float value = std::stof(str_value);
    time_price_t tp(time, value, str_wave);
    if (pos_c3 != std::string::npos)
    {
      tp.trade = str_trade;
    }
    m_tp.push_back(tp);
  }
  ifs.close();
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::write
/////////////////////////////////////////////////////////////////////////////////////////////////////

void model_t::write(const std::string &file_name)
{
  std::ofstream ofs(file_name, std::ios::out | std::ios::binary);
  ofs << m_ticker << "\n";
  ofs << m_period << "\n";
  ofs << m_interval << "\n";
  size_t size = m_tp.size();
  ofs << size << "\n";
  for (size_t idx = 0; idx < size; idx++)
  {
    ofs << m_tp.at(idx).time << ","
      << m_tp.at(idx).value << ","
      << m_tp.at(idx).wave << ","
      << m_tp.at(idx).trade << "\n";
  }
  ofs.close();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//model_t::reset_wave
/////////////////////////////////////////////////////////////////////////////////////////////////////

void model_t::reset_wave(const std::string &wave)
{
  size_t size = m_tp.size();
  for (size_t idx = 0; idx < size; idx++)
  {
    if (m_tp.at(idx).wave == wave)
    {
      m_tp.at(idx).wave = "-";
    }
  }
}
