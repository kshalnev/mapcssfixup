#pragma once

#include <algorithm>
#include <fstream>
#include <functional>
#include <string>

inline std::string & TrimLeft(std::string & s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
  return s;
}

inline std::string & TrimRight(std::string & s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
  return s;
}

inline std::string & Trim(std::string & s)
{
  return TrimLeft(TrimRight(s));
}

template <typename T>
void ForEachLine(std::string const & filePath, T fn)
{
  std::ifstream in(filePath);
  std::string s;
  while (std::getline(in, s)) fn(s);
}

std::string GetFolderPath(std::string const & filePath);
