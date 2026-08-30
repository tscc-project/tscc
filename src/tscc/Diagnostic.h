#pragma once
#include <cstddef>
#include <string>
#include <vector>
namespace tscc {
enum class Severity { Error, Warning, Note };
struct Diagnostic {
    Severity severity = Severity::Error;
    std::string code = "TSCC1000";
    std::string file;
    std::size_t line = 1;
    std::size_t column = 1;
    std::string message;
    std::string source_line;
};
class Diagnostics {
public:
    void error(const std::string& file, std::size_t line, std::size_t column,
               const std::string& message, const std::string& source_line = {},
               const std::string& code = "TSCC1000");
    void warning(const std::string& file, std::size_t line, std::size_t column,
                 const std::string& message, const std::string& source_line = {},
                 const std::string& code = "TSCC2000");
    bool has_errors() const;
    std::size_t size() const { return items_.size(); }
    const std::vector<Diagnostic>& items() const { return items_; }
    void append(Diagnostics&& other);
    void append(const Diagnostics& other);
    void print(bool color = true) const;
private:
    std::vector<Diagnostic> items_;
};
}
