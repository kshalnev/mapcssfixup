#include "common.hpp"

std::string GetFolderPath(std::string const & path)
{
  size_t const d1 = path.rfind('/');
  size_t const d2 = path.rfind('\\');
  if (d1 != std::string::npos && d2 != std::string::npos)
    return std::string(path.begin(), path.begin() + std::max(d1, d2) + 1);
  if (d1 == std::string::npos && d2 == std::string::npos)
    return std::string();
  else if (d1 != std::string::npos)
    return std::string(path.begin(), path.begin() + d1 + 1);
  else
    return std::string(path.begin(), path.begin() + d2 + 1);
}
