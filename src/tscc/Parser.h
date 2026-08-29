#pragma once
#include "Diagnostic.h"
#include "Lexer.h"
#include "Source.h"
#include "Syntax.h"
#include <vector>

namespace tscc {

class Parser {
public:
    Parser(const SourceFile& source, const std::vector<Token>& tokens, Diagnostics& diagnostics);
    bool parse(Program& program);

private:
    const SourceFile& source_;
    const std::vector<Token>& tokens_;
    Diagnostics& diagnostics_;
    std::size_t i_ = 0;
    Program* program_ = nullptr;
    std::vector<std::size_t> recovery_tokens_;

    bool at_end() const;
    const Token& token(std::size_t offset = 0) const;
    bool is(const char* text, std::size_t offset = 0) const;
    bool consume(const char* text);
    void error_at(std::size_t token_index, const std::string& message);
    void erase_tokens(std::size_t first, std::size_t last_exclusive);

    SyntaxNode parse_top_level();
    SyntaxNode parse_interface();
    SyntaxNode parse_type_alias();
    SyntaxNode parse_function();
    SyntaxNode parse_class();
    SyntaxNode parse_enum();
    SyntaxNode parse_namespace();
    SyntaxNode parse_variable_statement();
    SyntaxNode parse_statement();

    std::size_t find_matching(std::size_t open_index, const char* open, const char* close) const;
    std::size_t find_statement_end(std::size_t start) const;
    void parse_parameter_list(std::size_t open_index, std::size_t close_index);
    void erase_type_annotation(std::size_t colon_index, std::size_t limit_index,
                               const std::vector<std::string>& terminators);
    void parse_class_body(std::size_t open_index, std::size_t close_index, bool derived);
    void parse_variable_declarators(std::size_t first, std::size_t end);
    void erase_generic_list(std::size_t open_index, std::size_t close_index);
    void erase_expression_type_arguments(std::size_t first, std::size_t end);
    void add_replacement(std::size_t begin, std::size_t end, std::string text);
    void parse_runtime_range(std::size_t first, std::size_t end);
    void parse_object_literal_range(std::size_t open, std::size_t close);
    bool validate_generic_list(std::size_t open, std::size_t close);
    void finalize_syntax(SyntaxNode&,SyntaxNodeId,SyntaxNodeId&);
};

} // namespace tscc
