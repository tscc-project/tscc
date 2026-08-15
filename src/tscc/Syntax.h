#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace tscc {

enum class SyntaxKind {
    Program,
    InterfaceDeclaration,
    TypeAliasDeclaration,
    FunctionDeclaration,
    ClassDeclaration,
    EnumDeclaration,
    VariableStatement,
    Statement,
};

struct SyntaxNode {
    SyntaxKind kind = SyntaxKind::Statement;
    std::size_t begin_token = 0;
    std::size_t end_token = 0; // inclusive
    std::vector<SyntaxNode> children;
};

struct EraseRange {
    std::size_t begin = 0;
    std::size_t end = 0;
};

struct Replacement {
    std::size_t begin = 0;
    std::size_t end = 0;
    std::string text;
};

struct Program {
    SyntaxNode root;
    std::vector<EraseRange> erasures;
    std::vector<Replacement> replacements;
};

} // namespace tscc
