#include "common.hpp"
#include "mapcss_parse.h"

#include <iostream>
#include <list>
#include <queue>
#include <string>
#include <strstream>
#include <unordered_map>
#include <unordered_set>

using TCoefficientTable = std::unordered_map<std::string, double>;

void InitCoefficientTable(TCoefficientTable & table, int argc, const char * argv[])
{
  table["width"] = 1.0;
  table["dashes"] = 1.0;
  table["casing-width"] = 1.0;
  table["casing-dashes"] = 1.0;
  table["font-size"] = 1.0;
  table["text-offset"] = 1.0;
  table["text-halo-radius"] = 1.0;
  table["shield-font-size"] = 1.0;
  table["shield-text-halo-radius"] = 1.0;

  for (int i = 2; i < argc; ++i)
  {
    std::string const s = argv[i];
    size_t const d = s.find('=');
    if (d == std::string::npos)
      continue;
    std::string s1(s.begin(), s.begin() + d);
    std::string s2(s.begin() + d + 1, s.end());
    Trim(s1);
    Trim(s2);
    table[s1] = std::stod(s2);
  }
}

std::string MultiplyValue(std::string const & str, double ratio)
{
  // Value can be
  // <double>
  // or
  // <double> , <double>

  std::strstream os;

  size_t d = str.find(',');
  if (d == std::string::npos)
  {
    std::string s = str;
    Trim(s);

    double v = std::stod(s);
    v *= ratio;

    os << v;
  }
  else
  {
    std::string s1(str.begin(), str.begin() + d);
    std::string s2(str.begin() + d + 1, str.end());
    Trim(s1);
    Trim(s2);

    double v1 = std::stod(s1);
    double v2 = std::stod(s2);
    v1 *= ratio;
    v2 *= ratio;

    os << v1 << "," << v2;
  }

  return os.str();
}

template <typename T>
void Read(std::string const & project, T fn)
{
  std::queue<std::string> q;
  q.push(project);

  while (!q.empty())
  {
    std::string file = move(q.front());
    q.pop();

    std::string folder = GetFolderPath(file);
    size_t line = 0;

    ForEachLine(file, [&](std::string & s)
    {
      std::string import_file;
      if (mapcss::IsImportDirective(s, import_file))
      {
        import_file = folder + import_file;
        q.push(move(import_file));
      }

      fn(file, line, s);
      ++line;
    });
  }
}

class MapcssConverter
{
public:
  MapcssConverter(TCoefficientTable && table)
    : m_table(move(table))
  {}

  void operator()(std::string const & file, size_t /* line */, std::string & str)
  {
    bool const touched = Process(str);

    AppendFileContent(file, str);

    if (touched)
      m_touched.insert(file);
  }

  void Flush()
  {
    for (auto const & f : m_touched)
      WriteFile(f, m_files[f]);
  }

private:
  bool Process(std::string & str)
  {
    bool touched = false;

    for (auto const & cc : m_table)
    {
      if (cc.second == 1.0)
        continue;

      std::string value;
      auto const pos = mapcss::FindProperty(str, cc.first, value);
      if (pos.first == std::string::npos)
        continue;

      value = MultiplyValue(value, cc.second);

      str.erase(pos.first, pos.second);
      str.insert(pos.first, cc.first + ": " + value + ";");

      touched = true;
    }

    return touched;
  }

  void AppendFileContent(std::string const & file, std::string const & content)
  {
    m_files[file].emplace_back(content);
  }

  void WriteFile(std::string const & file, std::list<std::string> const & content)
  {
    std::ofstream out(file);
    for (auto const & s : content)
      out << s << std::endl;
    out.close();
  }

  std::unordered_set<std::string> m_touched;
  std::unordered_map<std::string, std::list<std::string>> m_files;

  TCoefficientTable m_table;
};


int main(int argc, const char * argv [])
{
  if (argc < 2)
  {
    std::cout << "Specify path to mapcss and correction parameters" << std::endl;
    return -1;
  }

  try
  {
    std::string const file = argv[1];
    std::cout << "Input mapcss file is " << file << std::endl;

    TCoefficientTable table;
    InitCoefficientTable(table, argc, argv);

    for (auto const & kv : table)
      std::cout << "Correction: " << kv.first << " x " << kv.second << std::endl;

    MapcssConverter converter(move(table));
    Read(file, std::ref(converter));

    converter.Flush();

    std::cout << "Done." << std::endl;
  }
  catch (std::exception & e)
  {
    std::cout << "Something went wrong. " << e.what() << std::endl;
  }

  return 0;
}
