#include "Checker.h"
#include "Type.h"
#include <unordered_map>
#include <unordered_set>

namespace tscc {
namespace {
std::vector<std::size_t> significant_tokens(const std::vector<Token>& tokens,
                                            std::size_t begin, std::size_t end) {
    std::vector<std::size_t> result;
    for (auto i = begin; i < end; ++i)
        if (tokens[i].kind != TokenKind::Comment) result.push_back(i);
    return result;
}
std::string literal_value(const std::string&text){return text.size()>=2&&(text.front()=='\''||text.front()=='"')?text.substr(1,text.size()-2):text;}

struct ExpressionResult {
    TypeId type = 0;
    bool complete = false;
    std::size_t error_token = static_cast<std::size_t>(-1);
    std::string error;
};

class NodeExpressionTyper {
public:
    NodeExpressionTyper(const std::vector<Token>& tokens, const ExpressionModel&expressions,const ExpressionNode&expression, const BindingModel& binding,
                             const TypeModel& types,const std::unordered_map<std::size_t,TypeId>*facts=nullptr,bool allow_object=false,TypeId expected=0)
        : tokens_(tokens),expressions_(expressions),root_(expression.id),binding_(binding), types_(types), facts_(facts),allow_object_(allow_object),expected_(expected) {}

    ExpressionResult run() {
        ExpressionResult result;
        result.type = type(root_, expected_);
        result.complete = expressions_.node(root_).kind != ExpressionKind::Unknown;
        result.error_token = error_token_;
        result.error = error_;
        if (!result.complete) result.type = types_.store.unknown();
        return result;
    }

private:
    const std::vector<Token>& tokens_;
    const ExpressionModel&expressions_;
    ExpressionId root_;
    const BindingModel& binding_;
    const TypeModel& types_;
    const std::unordered_map<std::size_t,TypeId>*facts_;
    std::size_t error_token_ = static_cast<std::size_t>(-1);
    std::string error_;
    bool allow_object_ = false;
    TypeId expected_ = 0;
    std::unordered_map<std::size_t,TypeId>contextual_facts_;

    TypeId parameter_annotation(std::size_t token) const {
        auto i=token+1;while(i<tokens_.size()&&tokens_[i].kind==TokenKind::Comment)++i;
        if(i<tokens_.size()&&tokens_[i].text=="?"){++i;while(i<tokens_.size()&&tokens_[i].kind==TokenKind::Comment)++i;}
        if(i>=tokens_.size()||tokens_[i].text!=":")return types_.store.unknown();
        do{++i;}while(i<tokens_.size()&&tokens_[i].kind==TokenKind::Comment);
        if(i>=tokens_.size())return types_.store.unknown();
        if(tokens_[i].text=="number")return types_.store.number();
        if(tokens_[i].text=="string")return types_.store.string();
        if(tokens_[i].text=="boolean")return types_.store.boolean();
        if(tokens_[i].text=="bigint")return types_.store.bigint();
        const auto named=types_.named_types.find(tokens_[i].text);return named==types_.named_types.end()?types_.store.unknown():named->second;
    }

    TypeId type(ExpressionId id, TypeId contextual = 0) {
        const auto&node=expressions_.node(id);
        if(node.kind==ExpressionKind::Unknown)return types_.store.unknown();
        if(node.kind==ExpressionKind::Parenthesized)return node.children.size()==1?type(node.children[0]):types_.store.unknown();
        if(node.kind==ExpressionKind::Literal){const auto&token=tokens_[node.operator_token];if(token.kind==TokenKind::Number)return types_.store.literal(!token.text.empty()&&token.text.back()=='n'?types_.store.bigint():types_.store.number(),token.text);if(token.kind==TokenKind::String||token.kind==TokenKind::Template)return types_.store.literal(types_.store.string(),literal_value(token.text));if(token.text=="true"||token.text=="false")return types_.store.literal(types_.store.boolean(),token.text);if(token.text=="null")return types_.store.null();return types_.store.unknown();}
        if(node.kind==ExpressionKind::Identifier){if(node.text=="undefined")return types_.store.undefined();const auto symbol=binding_.symbol_for_reference(node.operator_token);auto contextual=contextual_facts_.find(symbol);if(contextual!=contextual_facts_.end())return contextual->second;if(facts_){auto fact=facts_->find(symbol);if(fact!=facts_->end())return fact->second;}return symbol<types_.symbol_types.size()?types_.symbol_types[symbol]:types_.store.unknown();}
        if(node.kind==ExpressionKind::ObjectLiteral){if(!allow_object_)return types_.store.unknown();std::vector<TypeProperty>properties;for(auto property_id:node.children){const auto&property=expressions_.node(property_id);if(property.kind!=ExpressionKind::Property||property.children.size()!=1)return types_.store.unknown();properties.push_back({property.text,type(property.children[0]),false,false});}return types_.store.object_of(std::move(properties));}
        if(node.kind==ExpressionKind::Property){if(node.children.size()!=1)return types_.store.unknown();const auto value=type(node.children[0]);const auto*property=types_.store.property(value,node.text);if(!property){if(value!=types_.store.unknown()&&error_.empty()){error_token_=node.operator_token;error_="Property '"+node.text+"' does not exist on type '"+types_.store.name(value)+"'.";}return types_.store.unknown();}return property->type;}
        if(node.kind==ExpressionKind::Call){if(node.children.empty())return types_.store.unknown();const auto&callee=expressions_.node(node.children[0]);if(callee.kind!=ExpressionKind::Identifier)return types_.store.unknown();const auto symbol=binding_.symbol_for_reference(callee.operator_token);if(symbol>=types_.symbol_types.size())return types_.store.unknown();TypeModel::FunctionSignature signature;if(symbol<types_.function_signatures.size()&&types_.function_signatures[symbol].valid)signature=types_.function_signatures[symbol];else{const auto*callable=types_.store.callable(types_.symbol_types[symbol]);if(!callable)return types_.store.unknown();signature.parameters=callable->parameters;signature.result=callable->result;signature.required_parameters=callable->required_parameters;signature.rest=callable->rest;signature.valid=true;}std::vector<TypeId>arguments;for(std::size_t i=1;i<node.children.size();++i){const auto parameter=i-1<signature.parameters.size()?i-1:signature.parameters.empty()?0:signature.parameters.size()-1;const auto expected_argument=signature.parameters.empty()?types_.store.unknown():signature.parameters[parameter];arguments.push_back(type(node.children[i],expected_argument));}if((arguments.size()<signature.required_parameters||(!signature.rest&&arguments.size()>signature.parameters.size()))&&error_.empty()){error_token_=callee.operator_token;error_="Expected "+std::to_string(signature.parameters.size())+" arguments, but got "+std::to_string(arguments.size())+".";}else for(std::size_t i=0;i<arguments.size()&&!signature.parameters.empty();++i){const auto parameter=i<signature.parameters.size()?i:signature.parameters.size()-1,expected=signature.parameters[parameter],actual=arguments[i];if(expected!=types_.store.unknown()&&actual!=types_.store.unknown()&&!types_.store.assignable(actual,expected)&&error_.empty()){error_token_=callee.operator_token;error_="Argument of type '"+types_.store.name(types_.store.widen(actual))+"' is not assignable to parameter of type '"+types_.store.name(expected)+"'.";}}return signature.result;}
        if(node.kind==ExpressionKind::Function){const auto*expected=types_.store.callable(contextual);std::vector<TypeId>parameters;std::size_t required=0;bool rest=false;if(expected){if(node.parameter_tokens.size()>expected->parameters.size()){if(error_.empty()){error_token_=node.operator_token;error_="Function provides "+std::to_string(node.parameter_tokens.size())+" parameters but the target accepts "+std::to_string(expected->parameters.size())+".";}return types_.store.unknown();}parameters=expected->parameters;}else{for(auto token:node.parameter_tokens){const auto annotation=parameter_annotation(token);parameters.push_back(annotation);auto p=token+1;while(p<tokens_.size()&&tokens_[p].kind==TokenKind::Comment)++p;const bool optional=p<tokens_.size()&&(tokens_[p].text=="?"||tokens_[p].text=="=");bool parameter_rest=false;for(auto q=token;q>node.begin_token&&q+3>token;){--q;if(tokens_[q].kind==TokenKind::Comment)continue;parameter_rest=tokens_[q].text=="...";break;}rest=rest||parameter_rest;if(!optional&&!parameter_rest)++required;}}
            std::vector<std::size_t>symbols;for(std::size_t i=0;i<node.parameter_tokens.size();++i){std::size_t symbol=binding_.symbols.size();for(std::size_t s=0;s<binding_.symbols.size();++s)if(binding_.symbols[s].declaration_token==node.parameter_tokens[i]){symbol=s;break;}if(symbol<binding_.symbols.size()){contextual_facts_[symbol]=i<parameters.size()?parameters[i]:types_.store.unknown();symbols.push_back(symbol);}}
            TypeId result=expected?expected->result:types_.store.unknown();for(auto child:node.children){const auto actual=type(child,result);if(!expected&&actual!=types_.store.unknown())result=result==types_.store.unknown()?types_.store.widen(actual):types_.store.union_of({result,types_.store.widen(actual)});else if(expected&&actual!=types_.store.unknown()&&result!=types_.store.unknown()&&!types_.store.assignable(actual,result)&&error_.empty()){error_token_=expressions_.node(child).begin_token;error_="Type '"+types_.store.name(types_.store.widen(actual))+"' is not assignable to type '"+types_.store.name(result)+"'.";}}for(auto symbol:symbols)contextual_facts_.erase(symbol);if(expected)return contextual;if(result==types_.store.unknown())return types_.store.unknown();return types_.store.function_of(std::move(parameters),result,required,rest);}
        if(node.kind==ExpressionKind::Unary){if(node.children.size()!=1)return types_.store.unknown();const auto value=type(node.children[0]);const auto&op=node.text;
        const auto kind = types_.store.kind(types_.store.widen(value));
        if (value == types_.store.unknown()) return value;
        if (op == "!") return types_.store.boolean();
        if (op == "typeof") return types_.store.string();
        if ((op == "-" || op == "~") && kind == TypeKind::BigInt) return types_.store.bigint();
        if (kind == TypeKind::Number) return types_.store.number();
        fail_unary(node.operator_token, op, value);
        return types_.store.unknown();
        }
        if(node.kind==ExpressionKind::Binary){if(node.children.size()!=2)return types_.store.unknown();auto left=type(node.children[0]),right=type(node.children[1]);if(node.text=="+"&&left!=types_.store.unknown()&&right!=types_.store.unknown()&&
                (types_.store.kind(types_.store.widen(left)) == TypeKind::String ||
                 types_.store.kind(types_.store.widen(right)) == TypeKind::String))
                return types_.store.string();
            return arithmetic(node.operator_token,node.text,left,right);}
        if(node.kind==ExpressionKind::Assignment&&node.children.size()==2)return type(node.children[1]);
        return types_.store.unknown();
    }
    TypeId arithmetic(std::size_t token, const std::string& op, TypeId left, TypeId right) {
        if (left == types_.store.unknown() || right == types_.store.unknown())
            return types_.store.unknown();
        left=types_.store.widen(left);right=types_.store.widen(right);const auto left_kind = types_.store.kind(left), right_kind = types_.store.kind(right);
        if (left_kind == TypeKind::Number && right_kind == TypeKind::Number)
            return types_.store.number();
        if (left_kind == TypeKind::BigInt && right_kind == TypeKind::BigInt)
            return types_.store.bigint();
        if (error_.empty()) {
            error_token_ = token;
            error_ = std::string("Operator '") + op + "' cannot be applied to types '" +
                     types_.store.name(left) + "' and '" + types_.store.name(right) + "'.";
        }
        return types_.store.unknown();
    }
    void fail_unary(std::size_t token, const std::string& op, TypeId value) {
        if (!error_.empty()) return;
        error_token_ = token;
        error_ = std::string("Operator '") + op + "' cannot be applied to type '" +
                 types_.store.name(value) + "'.";
    }
};

ExpressionResult expression_type(const std::vector<Token>& tokens, ExpressionModel&expressions,std::size_t begin,
                                 std::size_t end, const BindingModel& binding,
                                 const TypeModel& types,const std::unordered_map<std::size_t,TypeId>*facts=nullptr,bool allow_object=false,TypeId expected=0) {
    const auto&root=expressions.intern(tokens,begin,end);
    return NodeExpressionTyper(tokens,expressions,root,binding,types,facts,allow_object,expected).run();
}

void report_expression_error(const SourceFile& source, const std::vector<Token>& tokens,
                             const ExpressionResult& result, Diagnostics& diagnostics) {
    if (result.error.empty() || result.error_token >= tokens.size()) return;
    const auto [line, column] = source.line_col(tokens[result.error_token].begin);
    diagnostics.error(source.path, line, column, result.error, source.line_text(line));
}

std::string structural_mismatch(TypeId actual,TypeId expected,const TypeStore&types,const std::string&path={}){if(types.kind(actual)!=TypeKind::Object||types.kind(expected)!=TypeKind::Object)return {};for(const auto&wanted:types.properties(expected)){const auto next=path.empty()?wanted.name:path+"."+wanted.name;const auto*got=types.property(actual,wanted.name);if(!got&&!wanted.optional)return "Property '"+next+"' is missing in source object but required in target type.";if(got&&!types.assignable(got->type,wanted.type)){if(auto nested=structural_mismatch(got->type,wanted.type,types,next);!nested.empty())return nested;return "Type '"+types.name(types.widen(got->type))+"' of property '"+next+"' is not assignable to type '"+types.name(wanted.type)+"'.";}}return {};}
void report_mismatch(const SourceFile& source, const std::vector<Token>& tokens,
                     std::size_t token, TypeId actual, TypeId expected,
                     const TypeStore& types,
                     Diagnostics& diagnostics) {
    const auto [line, column] = source.line_col(tokens[token].begin);
    std::string message;
    message=structural_mismatch(actual,expected,types);
    if(message.empty())message=std::string("Type '") + types.name(types.widen(actual)) + "' is not assignable to type '" + types.name(expected) + "'.";
    diagnostics.error(source.path, line, column,message, source.line_text(line));
}

TypeId compound_result(const std::string& op, TypeId left, TypeId right,
                       const TypeStore& types) {
    if (left == types.unknown() || right == types.unknown()) return types.unknown();
    left=types.widen(left);right=types.widen(right);
    const auto left_kind = types.kind(left), right_kind = types.kind(right);
    if (op == "+=" && (left_kind == TypeKind::String || right_kind == TypeKind::String))
        return types.string();
    if (left_kind == TypeKind::Number && right_kind == TypeKind::Number) return types.number();
    if (left_kind == TypeKind::BigInt && right_kind == TypeKind::BigInt) return types.bigint();
    return types.unknown();
}

void report_operator_error(const SourceFile& source, const std::vector<Token>& tokens,
                           std::size_t token, const std::string& op, TypeId left,
                           TypeId right, const TypeStore& types, Diagnostics& diagnostics) {
    const auto [line, column] = source.line_col(tokens[token].begin);
    diagnostics.error(source.path, line, column,
        std::string("Operator '") + op + "' cannot be applied to types '" +
        types.name(types.widen(left)) + "' and '" + types.name(types.widen(right)) + "'.", source.line_text(line));
}
}

bool check_program(const SourceFile& source, const std::vector<Token>& tokens,
                   const Program& program, const SemanticModel& model,
                   const BindingModel& binding, TypeModel& types,
                   ExpressionModel& expressions, Diagnostics& diagnostics) {
    struct FlowFact{std::size_t begin,end,symbol;TypeId type;};std::vector<FlowFact>flow;std::vector<std::pair<std::size_t,std::size_t>>owned_expression_ranges;
    auto next_sig=[&](std::size_t i){while(i<tokens.size()&&tokens[i].kind==TokenKind::Comment)++i;return i;};
    auto match=[&](std::size_t open,const char*l,const char*r){int depth=0;for(std::size_t i=open;i<tokens.size();++i){if(tokens[i].text==l)++depth;else if(tokens[i].text==r&&--depth==0)return i;}return tokens.size();};
    for(std::size_t i=0;i<tokens.size();++i){if(tokens[i].text!="if")continue;auto open=next_sig(i+1);if(open>=tokens.size()||tokens[open].text!="(")continue;auto close=match(open,"(",")");if(close>=tokens.size())continue;auto sig=significant_tokens(tokens,open+1,close);std::size_t ref=static_cast<std::size_t>(-1);TypeId narrowed=types.store.unknown();if(sig.size()==4&&tokens[sig[0]].text=="typeof"&&tokens[sig[1]].kind==TokenKind::Identifier&&tokens[sig[2]].text=="==="&&tokens[sig[3]].kind==TokenKind::String){ref=sig[1];const auto t=literal_value(tokens[sig[3]].text);if(t=="string")narrowed=types.store.string();else if(t=="number")narrowed=types.store.number();else if(t=="boolean")narrowed=types.store.boolean();}else if(sig.size()==3&&tokens[sig[0]].kind==TokenKind::Identifier&&tokens[sig[1]].text=="==="){ref=sig[0];const auto&v=tokens[sig[2]];if(v.text=="null")narrowed=types.store.null();else if(v.text=="undefined")narrowed=types.store.undefined();else if(v.kind==TokenKind::String)narrowed=types.store.literal(types.store.string(),literal_value(v.text));else if(v.kind==TokenKind::Number)narrowed=types.store.literal(v.text.back()=='n'?types.store.bigint():types.store.number(),v.text);else if(v.text=="true"||v.text=="false")narrowed=types.store.literal(types.store.boolean(),v.text);}auto body=next_sig(close+1);if(ref==static_cast<std::size_t>(-1)||narrowed==types.store.unknown()||body>=tokens.size()||tokens[body].text!="{")continue;auto body_end=match(body,"{","}");const auto symbol=binding.symbol_for_reference(ref);if(symbol<binding.symbols.size()&&body_end<tokens.size())flow.push_back({body+1,body_end,symbol,narrowed});}
    for (const auto& node : model.nodes) {
        if (node.kind != SemanticNodeKind::VariableDeclaration ||
            node.variable_index >= program.variables.size()) continue;
        const auto& declaration = program.variables[node.variable_index];
        TypeId expected = types.store.unknown();
        for (std::size_t i = 0; i < binding.symbols.size(); ++i)
            if (binding.symbols[i].declaration_token == declaration.name_token) {
                expected = types.symbol_types[i]; break;
            }
        if (declaration.initializer_end_token <= declaration.initializer_begin_token)
            continue;
        owned_expression_ranges.push_back({declaration.initializer_begin_token,declaration.initializer_end_token});
        std::unordered_map<std::size_t,TypeId>facts;std::size_t best=tokens.size();for(const auto&fact:flow)if(declaration.initializer_begin_token>=fact.begin&&declaration.initializer_end_token<=fact.end&&fact.end-fact.begin<=best){facts[fact.symbol]=fact.type;best=fact.end-fact.begin;}
        const auto expression = expression_type(tokens, expressions, declaration.initializer_begin_token,
                                                declaration.initializer_end_token,
                                                binding, types,facts.empty()?nullptr:&facts,true,expected);
        report_expression_error(source, tokens, expression, diagnostics);
        const auto actual = expression.type;
        if(expected==types.store.unknown()&&types.store.callable(actual))for(std::size_t i=0;i<binding.symbols.size();++i)if(binding.symbols[i].declaration_token==declaration.name_token){types.symbol_types[i]=actual;break;}
        if (!expression.error.empty() || expected == types.store.unknown() ||
            actual == types.store.unknown() || types.store.assignable(actual,expected)) continue;
        report_mismatch(source, tokens, declaration.initializer_begin_token,
                        actual, expected, types.store, diagnostics);
    }

    for (const auto& node : model.nodes) {
        if (node.kind != SemanticNodeKind::ReturnStatement ||
            node.begin_token + 1 >= node.end_token) continue;
        std::size_t owner = static_cast<std::size_t>(-1), owner_span = static_cast<std::size_t>(-1);
        for (std::size_t i = 0; i < binding.symbols.size(); ++i) {
            if (binding.symbols[i].kind != SymbolKind::Function ||
                binding.symbols[i].semantic_node >= model.nodes.size()) continue;
            const auto& function = model.nodes[binding.symbols[i].semantic_node];
            if (node.begin_token <= function.begin_token || node.end_token > function.end_token) continue;
            const auto span = function.end_token - function.begin_token;
            if (span < owner_span) { owner = i; owner_span = span; }
        }
        if (owner >= types.function_signatures.size()) continue;
        std::size_t same_name=0;for(const auto&symbol:binding.symbols)if(symbol.kind==SymbolKind::Function&&symbol.name==binding.symbols[owner].name&&symbol.scope==binding.symbols[owner].scope)++same_name;if(same_name>1)continue;
        const auto expected = types.function_signatures[owner].result;
        if (expected == types.store.unknown()) continue;
        owned_expression_ranges.push_back({node.begin_token+1,node.end_token});
        const auto expression = expression_type(tokens, expressions, node.begin_token + 1, node.end_token,
                                                binding, types);
        report_expression_error(source, tokens, expression, diagnostics);
        if (expression.error.empty() && expression.type != types.store.unknown() &&
            !types.store.assignable(expression.type,expected))
            report_mismatch(source, tokens, node.begin_token + 1,
                            expression.type, expected, types.store, diagnostics);
    }

    // Calls are checked wherever they occur, including standalone statements,
    // loop/branch headers and throw expressions. The retained call node remains
    // the sole owner of argument typing; this scan only discovers source roots.
    for (const auto& reference : binding.references) {
        bool already_owned=false;for(const auto&range:owned_expression_ranges)if(reference.token>=range.first&&reference.token<range.second){already_owned=true;break;}if(already_owned)continue;
        if (reference.symbol >= types.symbol_types.size()) continue;
        const bool declared_function = reference.symbol < types.function_signatures.size() &&
                                       types.function_signatures[reference.symbol].valid;
        if (!declared_function && !types.store.callable(types.symbol_types[reference.symbol])) continue;
        std::size_t open = reference.token + 1;
        while (open < tokens.size() && tokens[open].kind == TokenKind::Comment) ++open;
        if (open >= tokens.size() || tokens[open].text != "(") continue;
        std::size_t end = open + 1; int depth = 1;
        for (; end < tokens.size() && depth; ++end) {
            if (tokens[end].text == "(") ++depth;
            else if (tokens[end].text == ")") --depth;
        }
        if (depth) continue;
        const auto expression = expression_type(tokens, expressions, reference.token, end, binding, types);
        report_expression_error(source, tokens, expression, diagnostics);
    }

    std::unordered_set<std::size_t> declaration_names;
    for (const auto& symbol : binding.symbols) declaration_names.insert(symbol.declaration_token);
    for (std::size_t assignment = 1; assignment + 1 < tokens.size(); ++assignment) {
        const auto& op = tokens[assignment].text;
        if (op != "=" && op != "+=" && op != "-=" && op != "*=" &&
            op != "/=" && op != "%=" && op != "**=") continue;
        std::size_t left = assignment;
        while (left > 0 && tokens[left-1].kind == TokenKind::Comment) --left;
        if (left == 0) continue;
        --left;
        if (tokens[left].kind != TokenKind::Identifier || declaration_names.count(left)) continue;
        if(left>=2&&tokens[left-1].text=="."&&tokens[left-2].kind==TokenKind::Identifier){
            const auto owner=binding.symbol_for_reference(left-2);if(owner<types.symbol_types.size())if(const auto*property=types.store.property(types.symbol_types[owner],tokens[left].text);property&&property->readonly){const auto[line,column]=source.line_col(tokens[left].begin);diagnostics.error(source.path,line,column,"Cannot assign to '"+tokens[left].text+"' because it is a read-only property.",source.line_text(line));}continue;
        }
        const auto target_symbol = binding.symbol_for_reference(left);
        if (target_symbol >= types.symbol_types.size()) continue;
        if (binding.symbols[target_symbol].variable_kind == VariableKind::Const) {
            const auto [line, column] = source.line_col(tokens[left].begin);
            diagnostics.error(source.path, line, column,
                std::string("Cannot assign to '") + binding.symbols[target_symbol].name +
                "' because it is a constant.", source.line_text(line));
            continue;
        }
        const auto expected = types.symbol_types[target_symbol];
        if (expected == types.store.unknown()) continue;
        std::size_t begin = assignment + 1;
        while (begin < tokens.size() && tokens[begin].kind == TokenKind::Comment) ++begin;
        std::size_t end = begin;
        while (end < tokens.size() && tokens[end].text != ";" &&
               tokens[end].text != "," && tokens[end].kind != TokenKind::End) ++end;
        const auto expression = expression_type(tokens, expressions, begin, end, binding, types);
        report_expression_error(source, tokens, expression, diagnostics);
        const auto rhs = expression.type;
        if (!expression.error.empty() || rhs == types.store.unknown()) continue;
        if (op == "=") {
            if (!types.store.assignable(rhs,expected))
                report_mismatch(source, tokens, begin, rhs, expected, types.store, diagnostics);
            continue;
        }
        const auto result = compound_result(op, expected, rhs, types.store);
        if (result == types.store.unknown() || result != expected)
            report_operator_error(source, tokens, assignment, op, expected, rhs,
                                  types.store, diagnostics);
    }
    return !diagnostics.has_errors();
}
}
