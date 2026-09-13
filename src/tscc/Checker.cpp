#include "Checker.h"
#include "TypeParser.h"
#include "Type.h"
#include <algorithm>
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
        result.complete = expressions_.supported(root_);
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

    std::size_t containing_class(std::size_t token) const {std::size_t found=static_cast<std::size_t>(-1),span=static_cast<std::size_t>(-1);for(std::size_t i=0;i<types_.classes.size();++i){const auto&info=types_.classes[i];if(token>info.body_begin_token&&token<info.body_end_token&&info.body_end_token-info.body_begin_token<span){found=i;span=info.body_end_token-info.body_begin_token;}}return found;}
    bool derives_from(std::size_t candidate,std::size_t base)const{std::size_t depth=0;while(candidate<types_.classes.size()&&depth++<types_.classes.size()){if(candidate==base)return true;candidate=types_.classes[candidate].base_class;}return false;}
    void check_access(TypeId owner,const std::string&name,std::size_t token){std::size_t class_index=static_cast<std::size_t>(-1);bool static_side=false;for(std::size_t i=0;i<types_.classes.size();++i){if(types_.classes[i].instance_type==owner){class_index=i;break;}if(types_.classes[i].static_type==owner){class_index=i;static_side=true;break;}}if(class_index>=types_.classes.size())return;const TypeModel::ClassMember*member=nullptr;for(const auto&candidate:types_.classes[class_index].members)if(candidate.name==name&&candidate.is_static==static_side){member=&candidate;break;}if(!member||member->accessibility==TypeModel::Accessibility::Public)return;std::size_t declaring=class_index;for(std::size_t i=0;i<types_.classes.size();++i)if(member->begin_token>types_.classes[i].body_begin_token&&member->begin_token<types_.classes[i].body_end_token){declaring=i;break;}const auto caller=containing_class(token);const bool allowed=caller==declaring||(member->accessibility==TypeModel::Accessibility::Protected&&caller<types_.classes.size()&&derives_from(caller,declaring));if(!allowed&&error_.empty()){error_token_=token;error_="Property '"+name+"' is "+(member->accessibility==TypeModel::Accessibility::Private?"private":"protected")+" and only accessible within class '"+types_.classes[declaring].name+"'.";}}

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
        if(node.kind==ExpressionKind::Parenthesized)return node.children.size()==1?type(node.children[0],contextual):types_.store.unknown();
        if(node.kind==ExpressionKind::NonNull)return node.children.size()==1?types_.store.non_nullable(type(node.children[0],contextual)):types_.store.unknown();
        if(node.kind==ExpressionKind::Assertion||node.kind==ExpressionKind::Satisfies){if(node.children.size()!=1||node.type_argument_ranges.size()!=1)return types_.store.unknown();auto at=node.type_argument_ranges[0].first;const auto asserted=parse_type_annotation(tokens_,at,node.type_argument_ranges[0].second,types_.store,&types_.named_types);const auto actual=type(node.children[0],asserted);if(node.kind==ExpressionKind::Satisfies&&actual!=types_.store.unknown()&&asserted!=types_.store.unknown()&&!types_.store.assignable(actual,asserted)&&error_.empty()){error_token_=node.operator_token;error_="Type '"+types_.store.name(types_.store.widen(actual))+"' does not satisfy the expected type '"+types_.store.name(asserted)+"'.";}return node.kind==ExpressionKind::Assertion?asserted:actual;}
        if(node.kind==ExpressionKind::Literal){const auto&token=tokens_[node.operator_token];if(token.kind==TokenKind::Number)return types_.store.literal(!token.text.empty()&&token.text.back()=='n'?types_.store.bigint():types_.store.number(),token.text);if(token.kind==TokenKind::String||token.kind==TokenKind::Template)return types_.store.literal(types_.store.string(),literal_value(token.text));if(token.text=="true"||token.text=="false")return types_.store.literal(types_.store.boolean(),token.text);if(token.text=="null")return types_.store.null();return types_.store.unknown();}
        if(node.kind==ExpressionKind::Identifier){if(node.text=="undefined")return types_.store.undefined();if(node.text=="this"||node.text=="super"){const auto owner=containing_class(node.operator_token);if(owner>=types_.classes.size())return types_.store.unknown();if(node.text=="super"){const auto base=types_.classes[owner].base_class;return base<types_.classes.size()?types_.classes[base].instance_type:types_.store.unknown();}return types_.classes[owner].instance_type;}const auto symbol=binding_.symbol_for_reference(node.operator_token);auto contextual=contextual_facts_.find(symbol);if(contextual!=contextual_facts_.end())return contextual->second;if(facts_){auto fact=facts_->find(symbol);if(fact!=facts_->end())return fact->second;}return symbol<types_.symbol_types.size()?types_.symbol_types[symbol]:types_.store.unknown();}
        if(node.kind==ExpressionKind::New){if(node.children.size()!=1)return types_.store.unknown();return type_new(node.children[0]);}
        if(node.kind==ExpressionKind::ObjectLiteral){if(!allow_object_)return types_.store.unknown();std::vector<TypeProperty>properties;for(auto property_id:node.children){const auto&property=expressions_.node(property_id);if(property.kind!=ExpressionKind::Property||property.children.size()!=1)return types_.store.unknown();const auto*expected_property=types_.store.property(contextual,property.text);properties.push_back({property.text,type(property.children[0],expected_property?expected_property->type:types_.store.unknown()),false,false});}return types_.store.object_of(std::move(properties));}
        if(node.kind==ExpressionKind::ArrayLiteral){std::vector<TypeId>elements;const auto&tuple=types_.store.tuple_elements(contextual);const auto array_element=types_.store.array_element(contextual);for(std::size_t i=0;i<node.children.size();++i){const auto expected=i<tuple.size()?tuple[i]:array_element;elements.push_back(types_.store.widen(type(node.children[i],expected)));}if(!tuple.empty())return types_.store.tuple_of(std::move(elements));if(array_element!=types_.store.unknown())return types_.store.array_of(types_.store.union_of(std::move(elements)));return types_.store.tuple_of(std::move(elements));}
        if(node.kind==ExpressionKind::Property||node.kind==ExpressionKind::OptionalProperty){
            if(node.computed){
                if(node.children.size()!=2)return types_.store.unknown();
                const auto value=type(node.children[0]),key=type(node.children[1]);
                const auto key_kind=types_.store.kind(types_.store.widen(key));
                const auto&key_node=expressions_.node(node.children[1]);
                if(key_kind==TypeKind::String){
                    if(key_node.kind==ExpressionKind::Literal){const auto exact=literal_value(tokens_[key_node.operator_token].text);for(const auto&candidate:types_.store.properties(value))if(candidate.name==exact)return candidate.type;}
                    const auto indexed=types_.store.string_index(value);if(indexed!=types_.store.unknown())return indexed;
                }else if(key_kind==TypeKind::Number){
                    if(key_node.kind==ExpressionKind::Literal){const auto exact=tokens_[key_node.operator_token].text;for(const auto&candidate:types_.store.properties(value))if(candidate.name==exact)return candidate.type;}
                    auto indexed=types_.store.number_index(value);if(indexed==types_.store.unknown())indexed=types_.store.string_index(value);if(indexed!=types_.store.unknown())return indexed;
                }
                if(types_.store.kind(value)==TypeKind::Object&&key!=types_.store.unknown()&&error_.empty()){error_token_=node.operator_token;error_="Type '"+types_.store.name(value)+"' has no matching index signature for '"+types_.store.name(types_.store.widen(key))+"'.";}
                return types_.store.unknown();
            }
            if(node.children.size()!=1)return types_.store.unknown();
            const auto value=type(node.children[0]);
            const auto*property=types_.store.property(value,node.text);check_access(value,node.text,node.operator_token);
            if(!property){if(value!=types_.store.unknown()&&error_.empty()){error_token_=node.operator_token;error_="Property '"+node.text+"' does not exist on type '"+types_.store.name(value)+"'.";}return types_.store.unknown();}
            return node.kind==ExpressionKind::OptionalProperty?types_.store.union_of({property->type,types_.store.undefined()}):property->type;
        }
        if(node.kind==ExpressionKind::Call){
            if(node.children.empty())return types_.store.unknown();
            const auto&callee=expressions_.node(node.children[0]);
            std::vector<TypeModel::FunctionSignature>signatures;
            if(callee.kind==ExpressionKind::Identifier){if(callee.text=="super"){const auto owner=containing_class(callee.operator_token);if(owner<types_.classes.size()){const auto base=types_.classes[owner].base_class;if(base<types_.classes.size()){const auto&info=types_.classes[base];signatures=info.constructor_overloads.empty()?std::vector<TypeModel::FunctionSignature>{info.constructor}:info.constructor_overloads;}}}else{const auto symbol=binding_.symbol_for_reference(callee.operator_token);if(symbol>=types_.symbol_types.size())return types_.store.unknown();if(symbol<types_.overload_sets.size()&&!types_.overload_sets[symbol].empty())signatures=types_.overload_sets[symbol];else if(const auto*callable=types_.store.callable(types_.symbol_types[symbol])){TypeModel::FunctionSignature signature;signature.parameters=callable->parameters;signature.result=callable->result;signature.required_parameters=callable->required_parameters;signature.rest=callable->rest;signature.valid=true;signatures.push_back(std::move(signature));}}}
            else {if(callee.kind==ExpressionKind::Property&&!callee.computed&&callee.children.size()==1){const auto owner=type(callee.children[0]);bool static_side=false;const TypeModel::ClassInfo*class_info=nullptr;for(const auto&info:types_.classes){if(info.instance_type==owner){class_info=&info;break;}if(info.static_type==owner){class_info=&info;static_side=true;break;}}if(class_info)for(const auto&member:class_info->members)if(member.name==callee.text&&member.is_static==static_side&&!member.overloads.empty()){signatures=member.overloads;break;}}if(signatures.empty())if(const auto*callable=types_.store.callable(type(node.children[0]))){TypeModel::FunctionSignature signature;signature.parameters=callable->parameters;signature.result=callable->result;signature.required_parameters=callable->required_parameters;signature.rest=callable->rest;signature.valid=true;signatures.push_back(std::move(signature));}}
            if(signatures.empty())return types_.store.unknown();
            std::vector<TypeId>chosen;for(const auto&range:node.type_argument_ranges){auto at=range.first;chosen.push_back(parse_type_annotation(tokens_,at,range.second,types_.store,&types_.named_types));}
            bool needs_pretyping=signatures.size()>1;for(const auto&signature:signatures)needs_pretyping=needs_pretyping||!signature.type_parameters.empty();
            std::vector<TypeId>pre_arguments;for(std::size_t i=1;i<node.children.size();++i)pre_arguments.push_back(needs_pretyping?type(node.children[i]):types_.store.unknown());
            auto instantiate=[&](TypeModel::FunctionSignature signature,std::string&failure){
                if(chosen.size()>signature.type_parameters.size()){failure="Expected "+std::to_string(signature.type_parameters.size())+" type arguments, but got "+std::to_string(chosen.size())+".";signature.valid=false;return signature;}
                std::unordered_map<TypeId,std::vector<TypeId>>candidates;if(chosen.size()<signature.type_parameters.size()){for(std::size_t i=0;i<pre_arguments.size()&&!signature.parameters.empty();++i){const auto parameter=i<signature.parameters.size()?i:signature.parameters.size()-1;types_.store.infer_candidates(signature.parameters[parameter],pre_arguments[i],candidates);}if(contextual!=types_.store.unknown())types_.store.infer_candidates(signature.result,contextual,candidates);}
                std::unordered_map<TypeId,TypeId>map;for(std::size_t i=0;i<signature.type_parameters.size();++i){TypeId selected=i<chosen.size()?chosen[i]:types_.store.unknown();if(selected==types_.store.unknown()){auto found=candidates.find(signature.type_parameters[i].type);if(found!=candidates.end()){auto values=found->second;std::sort(values.begin(),values.end());values.erase(std::unique(values.begin(),values.end()),values.end());if(values.size()==1)selected=values.front();else if(values.size()>1){failure="Conflicting inferences for type parameter '"+signature.type_parameters[i].name+"'.";signature.valid=false;return signature;}}}if(selected==types_.store.unknown())selected=signature.type_parameters[i].default_type;if(selected==types_.store.unknown()){failure="Could not infer type parameter '"+signature.type_parameters[i].name+"'.";signature.valid=false;return signature;}const auto constraint=types_.store.substitute(signature.type_parameters[i].constraint,map);if(constraint!=types_.store.unknown()&&!types_.store.assignable(selected,constraint)){failure="Type argument '"+types_.store.name(selected)+"' does not satisfy constraint '"+types_.store.name(constraint)+"'.";signature.valid=false;return signature;}map[signature.type_parameters[i].type]=selected;}
                for(auto&parameter:signature.parameters)
                    parameter=types_.store.substitute(parameter,map);
                signature.result=types_.store.substitute(signature.result,map);
                return signature;
            };
            struct Applicable{TypeModel::FunctionSignature signature;int score=0;};std::vector<Applicable>applicable;std::string first_failure;
            for(auto candidate:signatures){std::string failure;candidate=instantiate(std::move(candidate),failure);if(!candidate.valid){if(first_failure.empty())first_failure=failure;continue;}if(pre_arguments.size()<candidate.required_parameters||(!candidate.rest&&pre_arguments.size()>candidate.parameters.size())){if(first_failure.empty())first_failure="Expected "+std::to_string(candidate.parameters.size())+" arguments, but got "+std::to_string(pre_arguments.size())+".";continue;}int score=0;bool valid=true;for(std::size_t i=0;i<pre_arguments.size()&&!candidate.parameters.empty();++i){const auto parameter=i<candidate.parameters.size()?i:candidate.parameters.size()-1,expected=candidate.parameters[parameter],actual=pre_arguments[i];if(actual==types_.store.unknown()||expected==types_.store.unknown())continue;if(!types_.store.assignable(actual,expected)){valid=false;if(first_failure.empty())first_failure="Argument of type '"+types_.store.name(types_.store.widen(actual))+"' is not assignable to parameter of type '"+types_.store.name(expected)+"'.";break;}score+=actual==expected?6:types_.store.widen(actual)==expected?4:1;}if(valid)applicable.push_back({std::move(candidate),score});}
            if(applicable.empty()){error_token_=callee.operator_token;error_=signatures.size()>1?"No overload matches this call.":first_failure;return types_.store.unknown();}
            // TypeScript overload order is observable: equally specific
            // applicable declarations resolve to the first declaration.
            auto best=std::max_element(applicable.begin(),applicable.end(),[](const auto&a,const auto&b){return a.score<b.score;});
            auto signature=best->signature;
            for(std::size_t i=1;i<node.children.size();++i){const auto parameter=i-1<signature.parameters.size()?i-1:signature.parameters.empty()?0:signature.parameters.size()-1;const auto expected_argument=signature.parameters.empty()?types_.store.unknown():signature.parameters[parameter];auto actual=pre_arguments[i-1];if(actual==types_.store.unknown())actual=type(node.children[i],expected_argument);if(expected_argument!=types_.store.unknown()&&actual!=types_.store.unknown()&&!types_.store.assignable(actual,expected_argument)&&error_.empty()){error_token_=callee.operator_token;error_="Argument of type '"+types_.store.name(types_.store.widen(actual))+"' is not assignable to parameter of type '"+types_.store.name(expected_argument)+"'.";}}
            return node.text=="optional"?types_.store.union_of({signature.result,types_.store.undefined()}):signature.result;
        }
        if(node.kind==ExpressionKind::Function){const auto*expected=types_.store.callable(contextual);std::vector<TypeId>parameters;std::size_t required=0;bool rest=false;if(expected){if(node.parameter_tokens.size()>expected->parameters.size()){if(error_.empty()){error_token_=node.operator_token;error_="Function provides "+std::to_string(node.parameter_tokens.size())+" parameters but the target accepts "+std::to_string(expected->parameters.size())+".";}return types_.store.unknown();}parameters=expected->parameters;}else{for(std::size_t parameter_index=0;parameter_index<node.parameter_tokens.size();++parameter_index){const auto token=node.parameter_tokens[parameter_index];const auto annotation=parameter_annotation(token);parameters.push_back(annotation);const auto end=parameter_index+1<node.parameter_tokens.size()?node.parameter_tokens[parameter_index+1]:node.operator_token;bool optional=false;for(auto p=token+1;p<end;++p)optional=optional||tokens_[p].text=="?"||tokens_[p].text=="=";bool parameter_rest=false;for(auto q=token;q>node.begin_token&&q+3>token;){--q;if(tokens_[q].kind==TokenKind::Comment)continue;parameter_rest=tokens_[q].text=="...";break;}rest=rest||parameter_rest;if(!optional&&!parameter_rest)++required;}}
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
        if(node.kind==ExpressionKind::Conditional){if(node.children.size()!=3)return types_.store.unknown();type(node.children[0]);return types_.store.union_of({types_.store.widen(type(node.children[1],contextual)),types_.store.widen(type(node.children[2],contextual))});}
        if(node.kind==ExpressionKind::Update){if(node.children.size()!=1)return types_.store.unknown();const auto value=types_.store.widen(type(node.children[0]));if(value==types_.store.unknown())return value;const auto kind=types_.store.kind(value);if(kind==TypeKind::Number||kind==TypeKind::BigInt)return value;fail_unary(node.operator_token,node.text,value);return types_.store.unknown();}
        if(node.kind==ExpressionKind::Binary){if(node.children.size()!=2)return types_.store.unknown();auto left=type(node.children[0]),right=type(node.children[1]);if(node.text=="&&"||node.text=="||"||node.text=="??")return types_.store.union_of({types_.store.widen(left),types_.store.widen(right)});if(node.text=="=="||node.text=="!="||node.text=="==="||node.text=="!=="||node.text=="<"||node.text==">"||node.text=="<="||node.text==">="||node.text=="in"||node.text=="instanceof")return types_.store.boolean();if(node.text=="+"&&left!=types_.store.unknown()&&right!=types_.store.unknown()&&
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
    TypeId type_new(ExpressionId id) {
        const auto&node=expressions_.node(id);
        if(node.kind==ExpressionKind::Property&&node.children.size()==1){const auto value=type_new(node.children[0]);const auto*property=types_.store.property(value,node.text);if(!property&&error_.empty()){error_token_=node.operator_token;error_="Property '"+node.text+"' does not exist on type '"+types_.store.name(value)+"'.";}return property?property->type:types_.store.unknown();}
        if(node.kind!=ExpressionKind::Call||node.children.empty())return types_.store.unknown();
        const auto&callee=expressions_.node(node.children[0]);
        if(callee.kind!=ExpressionKind::Identifier)return types_.store.unknown();
        const auto symbol=binding_.symbol_for_reference(callee.operator_token);
        if(symbol>=types_.symbol_classes.size()||types_.symbol_classes[symbol]>=types_.classes.size())return types_.store.unknown();
        const auto&info=types_.classes[types_.symbol_classes[symbol]];if(info.abstract_class&&error_.empty()){error_token_=callee.operator_token;error_="Cannot create an instance of an abstract class.";}std::vector<TypeModel::FunctionSignature>signatures=info.constructor_overloads.empty()?std::vector<TypeModel::FunctionSignature>{info.constructor}:info.constructor_overloads;TypeId instance=info.instance_type;std::unordered_map<TypeId,TypeId>class_map;std::vector<TypeId>actuals;for(std::size_t i=1;i<node.children.size();++i)actuals.push_back(type(node.children[i]));
        if(!info.type_parameters.empty()){std::vector<TypeId>arguments;for(const auto&range:node.type_argument_ranges){auto at=range.first;arguments.push_back(parse_type_annotation(tokens_,at,range.second,types_.store,&types_.named_types));}std::unordered_map<TypeId,std::vector<TypeId>>candidates;if(arguments.size()<info.type_parameters.size()&&!signatures.empty())for(std::size_t i=0;i<actuals.size()&&!signatures.front().parameters.empty();++i){const auto parameter=i<signatures.front().parameters.size()?i:signatures.front().parameters.size()-1;types_.store.infer_candidates(signatures.front().parameters[parameter],actuals[i],candidates);}for(std::size_t i=arguments.size();i<info.type_parameters.size();++i){TypeId selected=types_.store.unknown();auto found=candidates.find(info.type_parameters[i].type);if(found!=candidates.end()&&!found->second.empty()){selected=found->second.front();for(auto candidate:found->second)if(candidate!=selected){selected=types_.store.unknown();break;}}if(selected==types_.store.unknown())selected=info.type_parameters[i].default_type;arguments.push_back(selected);}bool valid=false;auto instantiated=types_.store.instantiate_generic(info.name,arguments,&valid);if(!valid){if(error_.empty()){error_token_=callee.operator_token;error_="Invalid or unresolved type arguments for class '"+info.name+"'.";}return types_.store.unknown();}instance=instantiated;for(std::size_t i=0;i<arguments.size()&&i<info.type_parameters.size();++i)class_map[info.type_parameters[i].type]=arguments[i];for(auto&signature:signatures)for(auto&parameter:signature.parameters)parameter=types_.store.substitute(parameter,class_map);}
        const auto count=node.children.size()-1;TypeModel::FunctionSignature*selected=nullptr;int selected_score=-1;std::string first_failure;for(auto&candidate:signatures){if(count<candidate.required_parameters||(!candidate.rest&&count>candidate.parameters.size())){if(first_failure.empty())first_failure="Expected "+std::to_string(candidate.parameters.size())+" arguments, but got "+std::to_string(count)+".";continue;}bool valid=true;int score=0;for(std::size_t i=0;i<actuals.size()&&!candidate.parameters.empty();++i){const auto parameter=i<candidate.parameters.size()?i:candidate.parameters.size()-1;if(actuals[i]!=types_.store.unknown()&&candidate.parameters[parameter]!=types_.store.unknown()&&!types_.store.assignable(actuals[i],candidate.parameters[parameter])){if(first_failure.empty())first_failure="Argument of type '"+types_.store.name(types_.store.widen(actuals[i]))+"' is not assignable to parameter of type '"+types_.store.name(candidate.parameters[parameter])+"'.";valid=false;break;}score+=actuals[i]==candidate.parameters[parameter]?6:types_.store.widen(actuals[i])==candidate.parameters[parameter]?4:1;}if(valid&&score>selected_score){selected=&candidate;selected_score=score;}}
        if(!selected){if(error_.empty()){error_token_=callee.operator_token;error_=signatures.size()>1?"No overload matches this call.":first_failure;}return instance;}for(std::size_t i=1;i<node.children.size()&&!selected->parameters.empty();++i){const auto parameter=i-1<selected->parameters.size()?i-1:selected->parameters.size()-1;if(actuals[i-1]==types_.store.unknown())type(node.children[i],selected->parameters[parameter]);}return instance;
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
    auto class_error=[&](std::size_t token,const std::string&message){if(token>=tokens.size())return;const auto[line,column]=source.line_col(tokens[token].begin);diagnostics.error(source.path,line,column,message,source.line_text(line));};
    for(const auto&issue:types.class_issues)class_error(issue.token,issue.message);
    auto own_member=[&](const TypeModel::ClassInfo&info,const TypeModel::ClassMember&member){return member.begin_token>info.body_begin_token&&member.begin_token<info.body_end_token;};
    auto access_rank=[](TypeModel::Accessibility access){return access==TypeModel::Accessibility::Public?2:access==TypeModel::Accessibility::Protected?1:0;};
    for(const auto&info:types.classes){
        for(std::size_t implemented=0;implemented<info.implements_names.size();++implemented){const auto&name=info.implements_names[implemented];auto expected=implemented<info.implements_types.size()?info.implements_types[implemented]:types.store.unknown();if(expected==types.store.unknown()){auto named=types.named_types.find(name);if(named!=types.named_types.end())expected=named->second;}if(expected==types.store.unknown()){class_error(info.declaration_token,"Cannot find interface '"+name+"'.");continue;}if(!types.store.assignable(info.instance_type,expected))class_error(info.declaration_token,"Class '"+info.name+"' incorrectly implements interface '"+name+"'.");}
        for(const auto&member:info.members){if(!own_member(info,member))continue;if(member.abstract_member&&!info.abstract_class)class_error(member.begin_token,"Abstract members can only appear within an abstract class.");const TypeModel::ClassMember*base_member=nullptr;if(info.base_class<types.classes.size())for(const auto&candidate:types.classes[info.base_class].members)if(candidate.name==member.name&&candidate.is_static==member.is_static){base_member=&candidate;break;}if(member.override_member&&!base_member)class_error(member.begin_token,"This member cannot have an 'override' modifier because it is not declared in the base class.");if(base_member){if(access_rank(member.accessibility)<access_rank(base_member->accessibility))class_error(member.begin_token,"Class member '"+member.name+"' cannot narrow the accessibility of the base member.");TypeId expected_base=base_member->type;if(!member.is_static&&info.declared_base_type!=types.store.unknown())if(const auto*property=types.store.property(info.declared_base_type,member.name))expected_base=property->type;if(member.type!=types.store.unknown()&&expected_base!=types.store.unknown()&&!types.store.assignable(member.type,expected_base))class_error(member.begin_token,"Property '"+member.name+"' in class '"+info.name+"' is not assignable to the same property in base class '"+types.classes[info.base_class].name+"'.");}}
        if(!info.abstract_class&&info.base_class<types.classes.size())for(const auto&required:types.classes[info.base_class].members)if(required.abstract_member){bool implemented=false;for(const auto&member:info.members)if(own_member(info,member)&&member.name==required.name&&!member.abstract_member){implemented=true;break;}if(!implemented)class_error(info.declaration_token,"Non-abstract class '"+info.name+"' does not implement inherited abstract member '"+required.name+"'.");}
    }
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

    // Standalone, branch/loop-header and throw calls are retained semantic
    // roots. Initializers and returns already own their complete expressions.
    for (const auto& node : model.nodes) {
        if (node.kind != SemanticNodeKind::ExpressionRoot&&node.kind!=SemanticNodeKind::Condition) continue;
        bool already_owned=false;for(const auto&range:owned_expression_ranges)if(node.begin_token>=range.first&&node.end_token<=range.second){already_owned=true;break;}if(already_owned)continue;
        const auto expression=node.expression_id<expressions.nodes().size()?NodeExpressionTyper(tokens,expressions,expressions.node(node.expression_id),binding,types).run():expression_type(tokens,expressions,node.begin_token,node.end_token,binding,types);
        report_expression_error(source,tokens,expression,diagnostics);
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
        if(left>=2&&tokens[left-1].text=="."&&(tokens[left-2].kind==TokenKind::Identifier||tokens[left-2].text=="this")){
            TypeId owner_type=types.store.unknown();const auto owner=binding.symbol_for_reference(left-2);if(owner<types.symbol_types.size())owner_type=types.symbol_types[owner];else if(tokens[left-2].text=="this")for(const auto&info:types.classes)if(left-2>info.body_begin_token&&left-2<info.body_end_token){owner_type=info.instance_type;break;}if(const auto*property=types.store.property(owner_type,tokens[left].text);property&&property->readonly){const auto[line,column]=source.line_col(tokens[left].begin);diagnostics.error(source.path,line,column,"Cannot assign to '"+tokens[left].text+"' because it is a read-only property.",source.line_text(line));}continue;
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
