#pragma once

#include <string>
#include <utility>
#include <functional>

namespace mapcss
{

using TPositionAndLength = std::pair<size_t, size_t>;

TPositionAndLength FindProperty(std::string const & str, size_t pos, std::string const & property, std::string & value);

bool IsImportDirective(std::string const & str, std::string & filePath);

void ReadProject(std::string const & filePath, std::function<void(std::string const & filePath, std::string && fileContent)> fn);

} // namespace mapcss
