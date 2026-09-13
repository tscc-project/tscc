#pragma once
#include "Lexer.h"
#include <cstddef>
#include <map>
#include <string>
#include <utility>
#include <vector>
namespace tscc {
using ExpressionId=std::size_t;
inline constexpr ExpressionId InvalidExpressionId=static_cast<ExpressionId>(-1);
enum class ExpressionKind { Unknown,Identifier,Literal,Parenthesized,Unary,Binary,Call,Property,ObjectLiteral,ArrayLiteral,Assignment,Function };
enum class UnknownExpressionReason { None, Empty, Malformed, UnsupportedSyntax };
struct ExpressionNode { ExpressionId id=InvalidExpressionId;ExpressionKind kind=ExpressionKind::Unknown;UnknownExpressionReason unknown_reason=UnknownExpressionReason::None;std::size_t begin_token=0,end_token=0,operator_token=InvalidExpressionId;std::string text;std::vector<ExpressionId>children;std::vector<std::size_t>parameter_tokens;std::vector<std::pair<std::size_t,std::size_t>>type_argument_ranges;bool expression_body=false;bool computed=false; };
class ExpressionModel {
 struct Builder {
  ExpressionModel&model;const std::vector<Token>&tokens;std::vector<std::size_t>sig;std::size_t pos=0;
  ExpressionId add(ExpressionKind kind,std::size_t begin,std::size_t end,std::string text={},std::vector<ExpressionId>children={},std::size_t op=InvalidExpressionId,UnknownExpressionReason reason=UnknownExpressionReason::None){ExpressionNode n;n.id=model.nodes_.size();n.kind=kind;n.unknown_reason=kind==ExpressionKind::Unknown?(reason==UnknownExpressionReason::None?UnknownExpressionReason::UnsupportedSyntax:reason):UnknownExpressionReason::None;n.begin_token=begin;n.end_token=end;n.text=std::move(text);n.children=std::move(children);n.operator_token=op;model.nodes_.push_back(std::move(n));return model.nodes_.back().id;}
  bool accept(const char*t){if(pos>=sig.size()||tokens[sig[pos]].text!=t)return false;++pos;return true;}
  ExpressionId primary(){if(pos>=sig.size())return add(ExpressionKind::Unknown,0,0);const auto start=pos;if(accept("(")){const auto begin=pos;int depth=1;while(pos<sig.size()&&depth){if(tokens[sig[pos]].text=="(")++depth;else if(tokens[sig[pos]].text==")"&&--depth==0)break;++pos;}if(depth||begin==pos)return add(ExpressionKind::Unknown,sig[start],sig.back()+1);auto value=sub(begin,pos);++pos;return add(ExpressionKind::Parenthesized,sig[start],sig[pos-1]+1,{}, {value});}if(accept("{")){std::vector<ExpressionId>properties;while(pos<sig.size()&&tokens[sig[pos]].text!="}"){if(tokens[sig[pos]].kind!=TokenKind::Identifier)return add(ExpressionKind::Unknown,sig[start],sig.back()+1);auto key=sig[pos++];if(!accept(":"))return add(ExpressionKind::Unknown,sig[start],sig.back()+1);const auto begin=pos;int paren=0,square=0,brace=0;while(pos<sig.size()){const auto&t=tokens[sig[pos]].text;if(!paren&&!square&&!brace&&(t==","||t=="}"))break;if(t=="(")++paren;else if(t==")"&&paren)--paren;else if(t=="[")++square;else if(t=="]"&&square)--square;else if(t=="{")++brace;else if(t=="}"&&brace)--brace;++pos;}if(begin==pos)return add(ExpressionKind::Unknown,sig[start],sig.back()+1);auto value=sub(begin,pos);properties.push_back(add(ExpressionKind::Property,key,model.nodes_[value].end_token,tokens[key].text,{value},key));if(!accept(","))break;}if(!accept("}"))return add(ExpressionKind::Unknown,sig[start],sig.back()+1);return add(ExpressionKind::ObjectLiteral,sig[start],sig[pos-1]+1,{},std::move(properties));}if(accept("[")){std::vector<ExpressionId>items;while(pos<sig.size()&&tokens[sig[pos]].text!="]"){const auto begin=pos;int paren=0,square=0,brace=0;while(pos<sig.size()){const auto&t=tokens[sig[pos]].text;if(!paren&&!square&&!brace&&(t==","||t=="]"))break;if(t=="(")++paren;else if(t==")"&&paren)--paren;else if(t=="[")++square;else if(t=="]"&&square)--square;else if(t=="{")++brace;else if(t=="}"&&brace)--brace;++pos;}if(begin<pos)items.push_back(sub(begin,pos));if(!accept(","))break;}if(!accept("]"))return add(ExpressionKind::Unknown,sig[start],sig.back()+1);return add(ExpressionKind::ArrayLiteral,sig[start],sig[pos-1]+1,{},std::move(items));}auto token=sig[pos++];const auto&item=tokens[token];const bool keyword_literal=item.text=="true"||item.text=="false"||item.text=="null";auto kind=(item.kind==TokenKind::Identifier&&!keyword_literal)||item.text=="undefined"?ExpressionKind::Identifier:ExpressionKind::Literal;return add(kind,token,token+1,item.text,{},token);}
  ExpressionId postfix(){
   auto value=primary();
   std::vector<std::pair<std::size_t,std::size_t>>type_arguments;
   for(;;){
    if(accept(".")){if(pos>=sig.size()||tokens[sig[pos]].kind!=TokenKind::Identifier)return add(ExpressionKind::Unknown,model.nodes_[value].begin_token,model.nodes_[value].end_token);auto property=sig[pos++];value=add(ExpressionKind::Property,model.nodes_[value].begin_token,property+1,tokens[property].text,{value},property);continue;}
    if(accept("[")){const auto open=sig[pos-1],begin=pos;int depth=1;while(pos<sig.size()&&depth){if(tokens[sig[pos]].text=="[")++depth;else if(tokens[sig[pos]].text=="]"&&--depth==0)break;++pos;}if(depth||begin==pos)return add(ExpressionKind::Unknown,model.nodes_[value].begin_token,model.nodes_[value].end_token);auto key=sub(begin,pos);++pos;auto property=add(ExpressionKind::Property,model.nodes_[value].begin_token,sig[pos-1]+1,{}, {value,key},open);model.nodes_[property].computed=true;value=property;continue;}
    if(accept("<")){std::size_t begin=pos;int angle=1,paren=0,square=0,brace=0;while(pos<sig.size()&&angle){const auto&t=tokens[sig[pos]].text;if(t=="<")++angle;else if(t==">"&&!--angle)break;else if(t=="(")++paren;else if(t==")"&&paren)--paren;else if(t=="[")++square;else if(t=="]"&&square)--square;else if(t=="{")++brace;else if(t=="}"&&brace)--brace;if(angle==1&&!paren&&!square&&!brace&&t==","){if(begin==pos)return add(ExpressionKind::Unknown,model.nodes_[value].begin_token,model.nodes_[value].end_token);type_arguments.push_back({sig[begin],sig[pos-1]+1});begin=pos+1;}++pos;}if(angle||begin==pos)return add(ExpressionKind::Unknown,model.nodes_[value].begin_token,model.nodes_[value].end_token);type_arguments.push_back({sig[begin],sig[pos-1]+1});++pos;continue;}
    if(accept("(")){
     std::vector<ExpressionId>children{value};
     while(pos<sig.size()&&tokens[sig[pos]].text!=")"){
      const auto begin=pos;int paren=0,square=0,brace=0;
      while(pos<sig.size()){
       const auto&t=tokens[sig[pos]].text;
       if(!paren&&!square&&!brace&&(t==","||t==")"))break;
       if(t=="(")++paren;else if(t==")"&&paren)--paren;else if(t=="[")++square;else if(t=="]"&&square)--square;else if(t=="{")++brace;else if(t=="}"&&brace)--brace;
       ++pos;
      }
      if(begin==pos)return add(ExpressionKind::Unknown,model.nodes_[value].begin_token,model.nodes_[value].end_token);
      children.push_back(sub(begin,pos));
      if(!accept(","))break;
     }
     if(!accept(")"))return add(ExpressionKind::Unknown,model.nodes_[value].begin_token,model.nodes_[value].end_token);
     value=add(ExpressionKind::Call,model.nodes_[value].begin_token,sig[pos-1]+1,{},std::move(children));model.nodes_[value].type_argument_ranges=std::move(type_arguments);continue;
    }
    break;
   }
   return value;
  }
  ExpressionId unary(){if(pos<sig.size()){const auto op=sig[pos];const auto&t=tokens[op].text;if(t=="+"||t=="-"||t=="~"||t=="!"||t=="typeof"){++pos;auto child=unary();return add(ExpressionKind::Unary,op,model.nodes_[child].end_token,t,{child},op);}}return postfix();}
  ExpressionId binary(int minimum){auto left=unary();for(;;){if(pos>=sig.size())break;const auto op=sig[pos];const auto&t=tokens[op].text;int precedence=t=="**"?30:(t=="*"||t=="/"||t=="%")?20:(t=="+"||t=="-")?10:0;if(precedence<minimum||precedence==0)break;++pos;auto right=binary(precedence+(t=="**"?0:1));left=add(ExpressionKind::Binary,model.nodes_[left].begin_token,model.nodes_[right].end_token,t,{left,right},op);}return left;}
  ExpressionId assignment(){auto left=binary(1);if(pos<sig.size()&&(tokens[sig[pos]].text=="="||tokens[sig[pos]].text=="+="||tokens[sig[pos]].text=="-="||tokens[sig[pos]].text=="*="||tokens[sig[pos]].text=="/=")){auto op=sig[pos++];auto right=assignment();return add(ExpressionKind::Assignment,model.nodes_[left].begin_token,model.nodes_[right].end_token,tokens[op].text,{left,right},op);}return left;}
  ExpressionId sub(std::size_t begin,std::size_t end){Builder nested{model,tokens,{},0};nested.sig.assign(sig.begin()+begin,sig.begin()+end);return nested.build();}
  std::vector<std::size_t>parameters(std::size_t begin,std::size_t end){std::vector<std::size_t>result;bool expect=true;int nested=0;for(auto p=begin;p<end;++p){const auto&t=tokens[sig[p]].text;if(t=="("||t=="{"||t=="["){++nested;continue;}if(t==")"||t=="}"||t=="]"){if(nested)--nested;continue;}if(nested)continue;if(t==","){expect=true;continue;}if(t=="...")continue;if(expect&&tokens[sig[p]].kind==TokenKind::Identifier){result.push_back(sig[p]);expect=false;}}return result;}
  ExpressionId function(std::size_t arrow=InvalidExpressionId){std::vector<std::size_t>params;std::vector<ExpressionId>body;bool expression_body=false;std::size_t body_begin=0;if(arrow!=InvalidExpressionId){if(arrow==1&&tokens[sig[0]].kind==TokenKind::Identifier)params.push_back(sig[0]);else if(tokens[sig[0]].text=="(")params=parameters(1,arrow-1);body_begin=arrow+1;}else{std::size_t open=1;while(open<sig.size()&&tokens[sig[open]].text!="(")++open;if(open>=sig.size())return add(ExpressionKind::Unknown,sig.front(),sig.back()+1);std::size_t close=open+1;int depth=1;for(;close<sig.size()&&depth;++close){if(tokens[sig[close]].text=="(")++depth;else if(tokens[sig[close]].text==")")--depth;}if(depth)return add(ExpressionKind::Unknown,sig.front(),sig.back()+1);params=parameters(open+1,close-1);body_begin=close;}
   if(body_begin<sig.size()&&tokens[sig[body_begin]].text=="{"){for(std::size_t p=body_begin+1;p+1<sig.size();++p)if(tokens[sig[p]].text=="return"){auto end=p+1;int depth=0;for(;end<sig.size();++end){const auto&t=tokens[sig[end]].text;if(t=="("||t=="{"||t=="[")++depth;else if(t==")"||t=="}"||t=="]"){if(depth)--depth;else break;}if(!depth&&t==";")break;}if(end>p+1)body.push_back(sub(p+1,end));p=end;}}else if(body_begin<sig.size()){body.push_back(sub(body_begin,sig.size()));expression_body=true;}
   auto id=add(ExpressionKind::Function,sig.front(),sig.back()+1,arrow==InvalidExpressionId?"function":"arrow",std::move(body),arrow==InvalidExpressionId?sig.front():sig[arrow]);model.nodes_[id].parameter_tokens=std::move(params);model.nodes_[id].expression_body=expression_body;return id;}
  ExpressionId build(){if(sig.empty())return add(ExpressionKind::Unknown,0,0,{}, {},InvalidExpressionId,UnknownExpressionReason::Empty);if(tokens[sig[0]].text=="function")return function();int depth=0;for(std::size_t p=0;p<sig.size();++p){const auto&t=tokens[sig[p]].text;if(t=="("||t=="["||t=="{")++depth;else if(t==")"||t=="]"||t=="}"){if(depth)--depth;}else if(t=="=>"&&depth==0)return function(p);}auto root=assignment();if(pos!=sig.size())return add(ExpressionKind::Unknown,sig.front(),sig.back()+1,{}, {root},InvalidExpressionId,UnknownExpressionReason::UnsupportedSyntax);return root;}
 };
public:
 const ExpressionNode&intern(const std::vector<Token>&tokens,std::size_t begin,std::size_t end){auto key=std::make_pair(begin,end);auto found=index_.find(key);if(found!=index_.end())return nodes_[found->second];Builder builder{*this,tokens,{},0};for(auto i=begin;i<end;++i)if(tokens[i].kind!=TokenKind::Comment)builder.sig.push_back(i);auto root=builder.build();index_[key]=root;return nodes_[root];}
 const ExpressionNode&node(ExpressionId id)const{return nodes_[id];}const std::vector<ExpressionNode>&nodes()const{return nodes_;}
 bool supported(ExpressionId id)const{if(id>=nodes_.size()||nodes_[id].kind==ExpressionKind::Unknown)return false;for(auto child:nodes_[id].children)if(!supported(child))return false;return true;}
 std::vector<ExpressionId>unknown_descendants(ExpressionId id)const{std::vector<ExpressionId>result;collect_unknown(id,result);return result;}
private:std::vector<ExpressionNode>nodes_;std::map<std::pair<std::size_t,std::size_t>,ExpressionId>index_;
 void collect_unknown(ExpressionId id,std::vector<ExpressionId>&out)const{if(id>=nodes_.size())return;if(nodes_[id].kind==ExpressionKind::Unknown)out.push_back(id);for(auto child:nodes_[id].children)collect_unknown(child,out);}
};
}
