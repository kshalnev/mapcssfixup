#include "mapcss_parse.h"
#include "common.hpp"

#include <queue>

namespace mapcss
{

namespace
{

const TPositionAndLength InvalidValue = TPositionAndLength(std::string::npos, 0);

} // namespace

TPositionAndLength FindProperty(std::string const & str, std::string const & property, std::string & value)
{
  size_t pos = 0;

  while (true)
  {
    pos = str.find(property, pos);
    if (pos == std::string::npos)
      return InvalidValue; // no such value

    if ((pos == 0) || (!std::isalnum(str[pos-1]) && str[pos-1] != '-'))
        break;

    pos += property.length();
  }

  size_t start = str.find(':', pos + 1);
  if (start == std::string::npos)
    return InvalidValue; // invalid value format

  size_t end = str.find(';', start + 1);
  if (end == std::string::npos)
    return InvalidValue; // invalid value format

  value = std::string(str.begin() + start + 1, str.begin() + end);

  return TPositionAndLength(pos, end - pos + 1);
}

bool IsImportDirective(std::string const & s, std::string & path)
{
  // @import ( " path " );

  size_t i = 0;
  while (i < s.length() && std::isspace(s[i])) ++i;
  if (i == s.length())
    return false;

  char const import_directive[] = "@import";
  size_t const import_directive_len = sizeof(import_directive) - 1;

  if (0 != s.compare(i, import_directive_len, import_directive))
    return false; // not an @import

  size_t start = s.find('(', i + import_directive_len);
  if (start == std::string::npos)
    return false; // invalid format

  size_t end = s.find(')', start + 1);
  if (end == std::string::npos)
    return false; // invalid format

  std::string value(s.begin() + start + 1, s.begin() + end);
  Trim(value);

  if (value.empty())
    return false; // invalid value format

  start = value.find('"', 0);
  if (start == std::string::npos)
    return false; // invalid value format

  end = value.find('"', start + 1);
  if (end == std::string::npos)
    return false; // invalid value format

  value = std::string(value.begin() + start + 1, value.begin() + end);
  Trim(value);

  path = move(value);
  return true;
}

void ReadProject(std::string const & filePath,
                 std::function<void(LineInfo const & lineInfo, std::string & line)> const & fn)
{
  std::queue<std::string> q;
  q.push(filePath);

  while (!q.empty())
  {
    std::string file = move(q.front());
    q.pop();

    LineInfo li;
    li.FilePath = file;
    li.LineNo = 0;

    std::string folder = GetFolderPath(file);

    ForEachLine(file, [&](std::string & s)
    {
      std::string import_file;
      if (mapcss::IsImportDirective(s, import_file))
      {
        import_file = folder + import_file;
        q.push(move(import_file));
      }

      fn(li, s);
      ++li.LineNo;
    });
  }
}

} // namespace mapcss
