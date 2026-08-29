#pragma once
#include <cstddef>
#include <string>
#include <vector>

namespace tscc {
using SyntaxNodeId = std::size_t;
inline constexpr SyntaxNodeId InvalidSyntaxNodeId = static_cast<SyntaxNodeId>(-1);

enum class SyntaxKind {
    Program,
    InterfaceDeclaration,
    TypeAliasDeclaration,
    FunctionDeclaration,
    ClassDeclaration,
    EnumDeclaration,
    VariableStatement,
    Recovery,
    Statement,
};

struct SyntaxNode {
    SyntaxKind kind = SyntaxKind::Statement;
    std::size_t begin_token = 0;
    std::size_t end_token = 0; // inclusive
    std::vector<SyntaxNode> children;
    SyntaxNodeId id = InvalidSyntaxNodeId;
    SyntaxNodeId parent_id = InvalidSyntaxNodeId;
    std::size_t begin_offset = 0;
    std::size_t end_offset = 0;
    bool recovered = false;
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

struct VariableDeclaration {
    std::size_t name_token = 0;
    std::size_t type_begin_token = 0;
    std::size_t type_end_token = 0; // exclusive; empty when unannotated
    std::size_t initializer_begin_token = 0;
    std::size_t initializer_end_token = 0; // exclusive; empty when absent
};

struct Program {
    SyntaxNode root;
    std::vector<EraseRange> erasures;
    std::vector<Replacement> replacements;
    std::vector<VariableDeclaration> variables;

    const SyntaxNode* find_node(SyntaxNodeId wanted) const {
        std::vector<const SyntaxNode*> pending{&root};
        while(!pending.empty()){const auto*node=pending.back();pending.pop_back();if(node->id==wanted)return node;for(const auto&child:node->children)pending.push_back(&child);}
        return nullptr;
    }
    std::size_t node_count() const {
        std::size_t count=0;std::vector<const SyntaxNode*>pending{&root};while(!pending.empty()){const auto*node=pending.back();pending.pop_back();++count;for(const auto&child:node->children)pending.push_back(&child);}return count;
    }
};

} // namespace tscc
