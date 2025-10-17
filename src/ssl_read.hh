#ifndef SSL_READ_HH_
#define SSL_READ_HH_

#include <string>

int ssl_read(const std::string& host, const std::string& port_num, const std::string& http, std::string& response);

#endif
