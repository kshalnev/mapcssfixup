#include "common.hpp"
#include "mapcss_parse.hpp"

#include <functional>
#include <iostream>
#include <list>
#include <queue>
#include <sstream>
#include <string>
#include <unordered_map>

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

class MapcssConverter
{
public:
  MapcssConverter(TCoefficientTable && table)
    : m_table(move(table))
  {}

  void Process(std::string const & filePath, std::string && fileContent)
  {
    bool const touched = ProcessFile(fileContent);

    if (touched)
      m_files[filePath] = move(fileContent);
  }

  void Flush()
  {
    for (auto const & fc : m_files)
      WriteFile(fc.first, fc.second);
    m_files.clear();
  }

  std::list<std::string> GetAffectedFiles() const
  {
    std::list<std::string> files;
    for (auto const & fc : m_files)
      files.emplace_back(fc.first);
    return files;
  }

private:
  bool ProcessFile(std::string & content)
  {
    bool touched = false;

    for (auto const & cc : m_table)
    {
      if (cc.second == 1.0)
        continue;

      size_t pos = 0;
      while (true)
      {
        std::string value;
        auto const ppos = mapcss::FindProperty(content, pos, cc.first, value);

        if (ppos.first == std::string::npos)
          break;

        value = CorrectValue(value, cc.second);

        std::string newProperty = cc.first + ": " + value + ";";
        content.replace(ppos.first, ppos.second, newProperty);

        pos = ppos.first + newProperty.length();

        touched = true;
      }
    }

    return touched;
  }

  static std::string CorrectValue(std::string const & str, double ratio)
  {
    // Value can be:
    //   <double>
    // or
    //   <double> , <double>
    // or
    //   eval(formula)
    // or
    //   <string> (like "butt")

    if (str.find("eval") != std::string::npos)
      return str;

    try
    {
      std::ostringstream o;

      size_t const d = str.find(',');
      if (d == std::string::npos)
      {
        std::string s = str;
        Trim(s);

        double v = std::stod(s);
        v *= ratio;

        o << v;
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

        o << v1 << "," << v2;
      }

      return o.str();
    }
    catch (std::invalid_argument & e)
    {
      std::cout << "WARNING. Value \"" << str << "\" cannot be processed. " << e.what() << std::endl;
      return str;
    }
  }

  static void WriteFile(std::string const & file, std::string const & content)
  {
    std::ofstream o(file);
    o << content;
    o.close();
  }

  TCoefficientTable const m_table;
  std::unordered_map<std::string, std::string> m_files;
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
    std::cout << "Input mapcss project: " << file << std::endl;

    TCoefficientTable table;
    InitCoefficientTable(table, argc, argv);

    for (auto const & kv : table)
      std::cout << "Correction: " << kv.first << " x " << kv.second << std::endl;

    MapcssConverter converter(move(table));
    mapcss::ReadProject(file, std::bind(&MapcssConverter::Process, &converter, std::placeholders::_1, std::placeholders::_2));

    auto const & affectedFiles = converter.GetAffectedFiles();
    std::cout << affectedFiles.size() << " files has been affected" << (affectedFiles.empty() ? "" : ":") << std::endl;
    for (auto const & f : affectedFiles)
      std::cout << "  " << f << std::endl;

    converter.Flush();

    std::cout << "Done." << std::endl;
  }
  catch (std::exception & e)
  {
    std::cout << "Something went wrong. " << e.what() << std::endl;
  }

  return 0;
}
