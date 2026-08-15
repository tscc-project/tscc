#pragma once
#include <string>
#include <vector>
namespace tscc {
struct SourceFile { std::string path,text; std::vector<std::size_t> line_starts; std::pair<std::size_t,std::size_t> line_col(std::size_t offset) const; std::string line_text(std::size_t line) const; };
bool load_source(const std::string& path, SourceFile& out, std::string& error);
}
