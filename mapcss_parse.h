#pragma once

#include <string>
#include <utility>
#include <functional>

namespace mapcss
{

using TPositionAndLength = std::pair<size_t, size_t>;

TPositionAndLength FindProperty(std::string const & str, std::string const & property, std::string & value);

bool IsImportDirective(std::string const & str, std::string & filePath);

struct LineInfo
{
  std::string FilePath;
  size_t LineNo;
};

void ReadProject(std::string const & filePath,
                 std::function<void(LineInfo const & lineInfo, std::string & line)> const & fn);

} // namespace mapcss
