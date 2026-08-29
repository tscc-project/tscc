#pragma once
#include "Lexer.h"
#include <cstddef>
#include <map>
#include <utility>
#include <vector>
namespace tscc {
using ExpressionId=std::size_t;
struct ExpressionNode { ExpressionId id=0;std::size_t begin_token=0,end_token=0;std::vector<std::size_t>significant_tokens; };
class ExpressionModel {
public:
 const ExpressionNode&intern(const std::vector<Token>&tokens,std::size_t begin,std::size_t end){auto key=std::make_pair(begin,end);auto found=index_.find(key);if(found!=index_.end())return nodes_[found->second];ExpressionNode node;node.id=nodes_.size();node.begin_token=begin;node.end_token=end;for(auto i=begin;i<end;++i)if(tokens[i].kind!=TokenKind::Comment)node.significant_tokens.push_back(i);nodes_.push_back(std::move(node));index_[key]=nodes_.back().id;return nodes_.back();}
 const std::vector<ExpressionNode>&nodes()const{return nodes_;}
private:std::vector<ExpressionNode>nodes_;std::map<std::pair<std::size_t,std::size_t>,ExpressionId>index_;
};
}
