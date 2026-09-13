#pragma once
#include "Expression.h"
#include "Lexer.h"
#include "Semantic.h"
#include <cstddef>
#include <vector>
namespace tscc {
using BasicBlockId=std::size_t;
inline constexpr BasicBlockId InvalidBasicBlockId=static_cast<BasicBlockId>(-1);
enum class BasicBlockKind{Entry,Statement,Branch,ShortCircuit,Loop,Return,Throw,Try,Catch,Finally,Join,Exit};
enum class ControlFlowEdgeKind{Normal,TrueBranch,FalseBranch,ShortCircuit,BackEdge,Exception,Finally};
struct BasicBlock{BasicBlockId id=InvalidBasicBlockId;BasicBlockKind kind=BasicBlockKind::Statement;std::size_t begin_token=0,end_token=0;std::vector<BasicBlockId>predecessors;bool reachable=false;};
struct ControlFlowEdge{BasicBlockId from=InvalidBasicBlockId,to=InvalidBasicBlockId;ControlFlowEdgeKind kind=ControlFlowEdgeKind::Normal;};
struct FunctionControlFlow{SemanticNodeId function_node=InvalidSemanticNodeId;BasicBlockId entry=0,exit=0;std::vector<BasicBlock>blocks;std::vector<ControlFlowEdge>edges;};
struct ControlFlowModel{std::vector<FunctionControlFlow>functions;};
ControlFlowModel build_control_flow(const std::vector<Token>&,const SemanticModel&,const ExpressionModel&);
}
