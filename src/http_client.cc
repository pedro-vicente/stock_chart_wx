#include <string>
#include <fstream>
#include "get.hh"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//main
/////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
  std::ifstream key_file("api_key.txt");
  if (!key_file.is_open())
  {
    return -1;
  }

  std::string key;
  std::getline(key_file, key);
  key_file.close();

  if (key.empty())
  {
    return -1;
  }
  if (get(key) < 0)
  {
    return -1;
  }
  return 0;
}

