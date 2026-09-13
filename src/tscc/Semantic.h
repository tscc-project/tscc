#pragma once
#include "Lexer.h"
#include "Expression.h"
#include "Syntax.h"
#include <cstddef>
#include <vector>

namespace tscc {

enum class SemanticNodeKind {
    VariableDeclaration,
    BindingDeclaration,
    FunctionDeclaration,
    ArrowFunction,
    ClassDeclaration,
    ParameterDeclaration,
    BraceRegion,
    ReturnStatement,
    ExpressionRoot,
    LexicalRegion,
    CatchDeclaration,
    Statement,
    Condition,
};

using SemanticNodeId = std::size_t;
inline constexpr SemanticNodeId InvalidSemanticNodeId = static_cast<SemanticNodeId>(-1);

struct SemanticNode {
    SemanticNodeKind kind = SemanticNodeKind::BraceRegion;
    std::size_t begin_token = 0;
    std::size_t end_token = 0; // exclusive
    std::size_t name_token = static_cast<std::size_t>(-1);
    std::size_t variable_index = static_cast<std::size_t>(-1);
    std::size_t scope_token = static_cast<std::size_t>(-1);
    std::size_t scope_end_token = static_cast<std::size_t>(-1);
    SemanticNodeId id = InvalidSemanticNodeId;
    SemanticNodeId parent_id = InvalidSemanticNodeId;
    ExpressionId expression_id = InvalidExpressionId;
};

struct SemanticModel {
    std::vector<SemanticNode> nodes;
};

SemanticModel build_semantic_model(const std::vector<Token>&, const Program&);
void own_semantic_expressions(const std::vector<Token>&, SemanticModel&, ExpressionModel&);

} // namespace tscc
