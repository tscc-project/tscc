#include "Type.h"
#include "TypeParser.h"
#include "TypeParser.cpp"
#include <algorithm>
#include <unordered_map>

namespace tscc {
namespace {
std::string literal_value(const std::string&text){return text.size()>=2&&(text.front()=='\''||text.front()=='"')?text.substr(1,text.size()-2):text;}
TypeId named_type(const std::string& text, const TypeStore& store,const std::unordered_map<std::string,TypeId>*named=nullptr);
#if 0 // CP39: superseded by the decomposed AnnotationParser in TypeParser.cpp.
TypeId parse_annotation(const std::vector<Token>&tokens,std::size_t&i,std::size_t end,const TypeStore&store,const std::unordered_map<std::string,TypeId>*named=nullptr){skip_type_noise(tokens,i,end);if(i>=end)return store.unknown();if(tokens[i].text=="("){++i;std::vector<TypeId>parameters;std::size_t required=0;bool rest=false;skip_type_noise(tokens,i,end);while(i<end&&tokens[i].text!=")"){bool parameter_rest=false,optional=false;if(tokens[i].text=="..."){parameter_rest=rest=true;++i;skip_type_noise(tokens,i,end);}if(i>=end||tokens[i].kind!=TokenKind::Identifier)return store.unknown();++i;skip_type_noise(tokens,i,end);if(i<end&&tokens[i].text=="?"){optional=true;++i;skip_type_noise(tokens,i,end);}if(i>=end||tokens[i].text!=":")return store.unknown();++i;auto parameter=parse_annotation(tokens,i,end,store,named);if(parameter==store.unknown())return parameter;parameters.push_back(parameter);if(!optional&&!parameter_rest)++required;skip_type_noise(tokens,i,end);if(i<end&&tokens[i].text==","){++i;skip_type_noise(tokens,i,end);continue;}break;}if(i>=end||tokens[i].text!=")")return store.unknown();++i;skip_type_noise(tokens,i,end);if(i>=end||tokens[i].text!="=>")return store.unknown();++i;auto result=parse_annotation(tokens,i,end,store,named);return result==store.unknown()?result:store.function_of(std::move(parameters),result,required,rest);}if(tokens[i].text!="{")return named_type(tokens[i++].text,store,named);++i;std::vector<TypeProperty>properties;TypeId string_index=store.unknown(),number_index=store.unknown(),call_signature=store.unknown();while(i<end){skip_type_noise(tokens,i,end);while(i<end&&(tokens[i].text==";"||tokens[i].text==",")){++i;skip_type_noise(tokens,i,end);}if(i<end&&tokens[i].text=="}"){++i;return store.object_of(std::move(properties),string_index,number_index,call_signature);}if(i<end&&tokens[i].text=="["){++i;if(i>=end||tokens[i].kind!=TokenKind::Identifier)return store.unknown();++i;skip_type_noise(tokens,i,end);if(i>=end||tokens[i].text!=":")return store.unknown();++i;skip_type_noise(tokens,i,end);if(i>=end)return store.unknown();const auto key_kind=tokens[i++].text;skip_type_noise(tokens,i,end);if(i>=end||tokens[i].text!="]")return store.unknown();++i;skip_type_noise(tokens,i,end);if(i>=end||tokens[i].text!=":")return store.unknown();++i;auto indexed=parse_annotation(tokens,i,end,store,named);if(indexed==store.unknown())return indexed;if(key_kind=="string")string_index=indexed;else if(key_kind=="number")number_index=indexed;else return store.unknown();continue;}if(i<end&&tokens[i].text=="("){++i;std::vector<TypeId>parameters;std::size_t required=0;bool rest=false;skip_type_noise(tokens,i,end);while(i<end&&tokens[i].text!=")"){bool parameter_rest=false,optional=false;if(tokens[i].text=="..."){parameter_rest=rest=true;++i;skip_type_noise(tokens,i,end);}if(i>=end||tokens[i].kind!=TokenKind::Identifier)return store.unknown();++i;skip_type_noise(tokens,i,end);if(i<end&&tokens[i].text=="?"){optional=true;++i;skip_type_noise(tokens,i,end);}if(i>=end||tokens[i].text!=":")return store.unknown();++i;auto parameter=parse_annotation(tokens,i,end,store,named);if(parameter==store.unknown())return parameter;parameters.push_back(parameter);if(!optional&&!parameter_rest)++required;skip_type_noise(tokens,i,end);if(i<end&&tokens[i].text==","){++i;skip_type_noise(tokens,i,end);continue;}break;}if(i>=end||tokens[i].text!=")")return store.unknown();++i;skip_type_noise(tokens,i,end);if(i>=end||tokens[i].text!=":")return store.unknown();++i;auto result=parse_annotation(tokens,i,end,store,named);if(result==store.unknown())return result;call_signature=store.function_of(std::move(parameters),result,required,rest);continue;}bool readonly=false,optional=false;if(i<end&&tokens[i].text=="readonly"){readonly=true;++i;skip_type_noise(tokens,i,end);}if(i>=end||tokens[i].kind!=TokenKind::Identifier)return store.unknown();auto name=tokens[i++].text;skip_type_noise(tokens,i,end);if(i<end&&tokens[i].text=="?"){optional=true;++i;skip_type_noise(tokens,i,end);}TypeId type=store.unknown();if(i<end&&tokens[i].text=="("){++i;std::vector<TypeId>parameters;std::size_t required=0;bool rest=false;skip_type_noise(tokens,i,end);while(i<end&&tokens[i].text!=")"){bool parameter_rest=false,parameter_optional=false;if(tokens[i].text=="..."){parameter_rest=rest=true;++i;skip_type_noise(tokens,i,end);}if(i>=end||tokens[i].kind!=TokenKind::Identifier)return store.unknown();++i;skip_type_noise(tokens,i,end);if(i<end&&tokens[i].text=="?"){parameter_optional=true;++i;skip_type_noise(tokens,i,end);}if(i>=end||tokens[i].text!=":")return store.unknown();++i;auto parameter=parse_annotation(tokens,i,end,store,named);if(parameter==store.unknown())return parameter;parameters.push_back(parameter);if(!parameter_optional&&!parameter_rest)++required;skip_type_noise(tokens,i,end);if(i<end&&tokens[i].text==","){++i;skip_type_noise(tokens,i,end);continue;}break;}if(i>=end||tokens[i].text!=")")return store.unknown();++i;skip_type_noise(tokens,i,end);if(i>=end||tokens[i].text!=":")return store.unknown();++i;auto result=parse_annotation(tokens,i,end,store,named);if(result==store.unknown())return result;type=store.function_of(std::move(parameters),result,required,rest);}else{if(i>=end||tokens[i++].text!=":")return store.unknown();type=parse_annotation(tokens,i,end,store,named);}if(type==store.unknown())return type;properties.push_back({std::move(name),type,optional,readonly});}return store.unknown();}
#endif
#define parse_annotation parse_type_annotation
TypeId annotation_type(const std::vector<Token>& tokens,
                       const VariableDeclaration& declaration,
                       const TypeStore& store,const std::unordered_map<std::string,TypeId>*named=nullptr) {
    auto first=declaration.type_begin_token;while(first<declaration.type_end_token&&tokens[first].kind==TokenKind::Comment)++first;
    if(first<declaration.type_end_token&&(tokens[first].text=="{"||tokens[first].text=="("))return parse_type_annotation(tokens,first,declaration.type_end_token,store,named);
    std::vector<TypeId> members;
    for (auto i = declaration.type_begin_token; i < declaration.type_end_token; ++i) {
        if (tokens[i].kind == TokenKind::Comment || tokens[i].text == "|") continue;
        TypeId type=store.unknown();
        type=named_type(tokens[i].text,store,named);
        if(tokens[i].kind==TokenKind::String)type=store.literal(store.string(),literal_value(tokens[i].text));
        else if(tokens[i].kind==TokenKind::Number)type=store.literal(tokens[i].text.back()=='n'?store.bigint():store.number(),tokens[i].text);
        else if(tokens[i].text=="true"||tokens[i].text=="false")type=store.literal(store.boolean(),tokens[i].text);
        if(type==store.unknown())return type;
        members.push_back(type);
    }
    return store.union_of(std::move(members));
}
TypeId named_type(const std::string& text, const TypeStore& store,const std::unordered_map<std::string,TypeId>*named) {
    if (text == "number") return store.number();
    if (text == "string") return store.string();
    if (text == "boolean") return store.boolean();
    if (text == "bigint") return store.bigint();
    if (text == "null") return store.null();
    if (text == "undefined") return store.undefined();
    if(named){auto found=named->find(text);if(found!=named->end())return found->second;}return store.unknown();
}
TypeId node_annotation(const std::vector<Token>& tokens, const SemanticNode& node,
                       const TypeStore& store,const std::unordered_map<std::string,TypeId>*named=nullptr) {
    bool colon = false;std::vector<TypeId>members;
    for (auto i = node.name_token + 1; i < node.end_token && i < tokens.size(); ++i) {
        if (tokens[i].kind == TokenKind::Comment) continue;
        if (tokens[i].text == ":") { colon = true; continue; }
        if (!colon||tokens[i].text=="|"||tokens[i].text=="?"||tokens[i].text=="...") continue;
        if(tokens[i].text=="=")break;
        TypeId type=named_type(tokens[i].text,store,named);
        if(tokens[i].kind==TokenKind::String)type=store.literal(store.string(),literal_value(tokens[i].text));
        else if(tokens[i].kind==TokenKind::Number)type=store.literal(tokens[i].text.back()=='n'?store.bigint():store.number(),tokens[i].text);
        else if(tokens[i].text=="true"||tokens[i].text=="false")type=store.literal(store.boolean(),tokens[i].text);
        if(type==store.unknown())return type;
        members.push_back(type);
    }
    return store.union_of(std::move(members));
}
}

TypeStore::TypeStore()
    : types_{{TypeKind::Unknown,0,"",{},{},{},0,0,false}, {TypeKind::Number,0,"",{},{},{},0,0,false},
             {TypeKind::String,0,"",{},{},{},0,0,false}, {TypeKind::Boolean,0,"",{},{},{},0,0,false},
             {TypeKind::BigInt,0,"",{},{},{},0,0,false}, {TypeKind::Function,0,"",{},{},{},0,0,false},
             {TypeKind::Null,0,"",{},{},{},0,0,false}, {TypeKind::Undefined,0,"",{},{},{},0,0,false}} {}

TypeKind TypeStore::kind(TypeId id) const {
    return id < types_.size() ? types_[id].kind : TypeKind::Unknown;
}
TypeId TypeStore::widen(TypeId id)const{return kind(id)==TypeKind::Literal?types_[id].base:id;}

std::string TypeStore::name(TypeId id) const {
    switch (kind(id)) {
        case TypeKind::Number: return "number";
        case TypeKind::String: return "string";
        case TypeKind::Boolean: return "boolean";
        case TypeKind::BigInt: return "bigint";
        case TypeKind::Function:{if(id==function())return "function";std::string out="(";for(std::size_t i=0;i<types_[id].parameters.size();++i){if(i)out+=", ";out+=name(types_[id].parameters[i]);}return out+") => "+name(types_[id].result);}
        case TypeKind::Null:return "null";case TypeKind::Undefined:return "undefined";
        case TypeKind::Literal:return types_[id].base==string()?"'"+types_[id].literal+"'":types_[id].literal;
        case TypeKind::Union:{std::string out;for(auto member:types_[id].members){if(!out.empty())out+=" | ";out+=name(member);}return out;}
        case TypeKind::Object:return "object";
        default: return "unknown";
    }
}

TypeId TypeStore::literal(TypeId base,const std::string&value)const{for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Literal&&types_[i].base==base&&types_[i].literal==value)return i;types_.push_back({TypeKind::Literal,base,value,{},{},{},0,0,false});return types_.size()-1;}
TypeId TypeStore::union_of(std::vector<TypeId> members)const{std::vector<TypeId>flat;for(auto id:members){if(id==unknown())return id;if(kind(id)==TypeKind::Union)flat.insert(flat.end(),types_[id].members.begin(),types_[id].members.end());else flat.push_back(id);}std::sort(flat.begin(),flat.end());flat.erase(std::unique(flat.begin(),flat.end()),flat.end());std::vector<TypeId>bases;for(auto id:flat)if(kind(id)!=TypeKind::Literal)bases.push_back(id);flat.erase(std::remove_if(flat.begin(),flat.end(),[&](auto x){return kind(x)==TypeKind::Literal&&std::find(bases.begin(),bases.end(),types_[x].base)!=bases.end();}),flat.end());if(flat.empty())return unknown();if(flat.size()==1)return flat[0];for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Union&&types_[i].members==flat)return i;types_.push_back({TypeKind::Union,0,"",flat,{},{},0,0,false});return types_.size()-1;}
bool TypeStore::assignable(TypeId actual,TypeId expected)const{if(actual==unknown()||expected==unknown()||actual==expected)return true;if(kind(expected)==TypeKind::Union){for(auto member:types_[expected].members)if(assignable(actual,member))return true;return false;}if(kind(actual)==TypeKind::Union){for(auto member:types_[actual].members)if(!assignable(member,expected))return false;return true;}if(kind(actual)==TypeKind::Literal)return types_[actual].base==expected;if(kind(actual)==TypeKind::Function&&kind(expected)==TypeKind::Function){const auto*a=callable(actual),*e=callable(expected);if(!a||!e)return actual==function()||expected==function();if(a->required_parameters>e->required_parameters||(!a->rest&&a->parameters.size()<e->parameters.size()))return false;for(std::size_t i=0;i<e->parameters.size();++i){const auto ai=i<a->parameters.size()?i:a->parameters.size()-1;if(a->parameters.empty()||!assignable(e->parameters[i],a->parameters[ai]))return false;}return assignable(a->result,e->result);}if(kind(expected)==TypeKind::Object&&types_[expected].call_signature!=unknown()&&kind(actual)==TypeKind::Function)return assignable(actual,types_[expected].call_signature);if(kind(actual)==TypeKind::Object&&kind(expected)==TypeKind::Object){if(types_[expected].call_signature!=unknown()&&(types_[actual].call_signature==unknown()||!assignable(types_[actual].call_signature,types_[expected].call_signature)))return false;for(const auto&wanted:types_[expected].properties){const auto*got=property(actual,wanted.name);if(!got){if(wanted.optional)continue;return false;}if(!assignable(got->type,wanted.type))return false;}if(types_[expected].string_index!=unknown()){for(const auto&got:types_[actual].properties)if(!assignable(got.type,types_[expected].string_index))return false;if(types_[actual].string_index!=unknown()&&!assignable(types_[actual].string_index,types_[expected].string_index))return false;}if(types_[expected].number_index!=unknown()&&types_[actual].number_index!=unknown()&&!assignable(types_[actual].number_index,types_[expected].number_index))return false;return true;}return false;}
TypeId TypeStore::object_of(std::vector<TypeProperty>properties,TypeId string_index,TypeId number_index,TypeId call_signature)const{std::sort(properties.begin(),properties.end(),[](const auto&a,const auto&b){return a.name<b.name;});for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Object&&types_[i].properties.size()==properties.size()&&types_[i].string_index==string_index&&types_[i].number_index==number_index&&types_[i].call_signature==call_signature){bool same=true;for(std::size_t p=0;p<properties.size();++p){const auto&a=types_[i].properties[p];const auto&b=properties[p];same= same&&a.name==b.name&&a.type==b.type&&a.optional==b.optional&&a.readonly==b.readonly;}if(same)return i;}Type type;type.kind=TypeKind::Object;type.properties=std::move(properties);type.string_index=string_index;type.number_index=number_index;type.call_signature=call_signature;types_.push_back(std::move(type));return types_.size()-1;}
const TypeProperty*TypeStore::property(TypeId id,const std::string&name)const{if(kind(id)!=TypeKind::Object)return nullptr;for(const auto&property:types_[id].properties)if(property.name==name)return &property;if(types_[id].string_index!=unknown()){static thread_local TypeProperty indexed;indexed={name,types_[id].string_index,false,false};return &indexed;}return nullptr;}
const std::vector<TypeProperty>&TypeStore::properties(TypeId id)const{static const std::vector<TypeProperty>empty;return kind(id)==TypeKind::Object?types_[id].properties:empty;}
TypeId TypeStore::function_of(std::vector<TypeId>parameters,TypeId result,std::size_t required,bool rest)const{for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Function&&types_[i].parameters==parameters&&types_[i].result==result&&types_[i].required_parameters==required&&types_[i].rest==rest)return i;Type type;type.kind=TypeKind::Function;type.parameters=std::move(parameters);type.result=result;type.required_parameters=required;type.rest=rest;types_.push_back(std::move(type));return types_.size()-1;}
const Type*TypeStore::callable(TypeId id)const{if(kind(id)==TypeKind::Object&&types_[id].call_signature!=unknown())id=types_[id].call_signature;return kind(id)==TypeKind::Function&&id!=function()?&types_[id]:nullptr;}

TypeModel build_type_model(const std::vector<Token>& tokens, const Program& program,
                           const SemanticModel& semantic, const BindingModel& binding) {
    TypeModel model;
    for(const auto&node:program.root.children){if(node.kind!=SyntaxKind::InterfaceDeclaration&&node.kind!=SyntaxKind::TypeAliasDeclaration)continue;auto name=node.begin_token+1;while(name<tokens.size()&&tokens[name].kind==TokenKind::Comment)++name;if(name>=tokens.size()||tokens[name].kind!=TokenKind::Identifier)continue;std::size_t begin=name+1;if(node.kind==SyntaxKind::InterfaceDeclaration){while(begin<=node.end_token&&tokens[begin].text!="{")++begin;}else{while(begin<=node.end_token&&tokens[begin].text!="=")++begin;if(begin<=node.end_token)++begin;}if(begin>node.end_token)continue;auto at=begin;auto shape=parse_annotation(tokens,at,node.end_token+1,model.store,&model.named_types);while(at<=node.end_token&&(tokens[at].kind==TokenKind::Comment||tokens[at].text==";"))++at;if(shape==model.store.unknown()||at<=node.end_token)continue;if(node.kind==SyntaxKind::InterfaceDeclaration){std::vector<TypeProperty>inherited;bool extends=false,valid=true;for(auto p=name+1;p<begin;++p){if(tokens[p].kind==TokenKind::Comment||tokens[p].text==",")continue;if(tokens[p].text=="extends"){extends=true;continue;}if(extends&&tokens[p].kind==TokenKind::Identifier){auto base=model.named_types.find(tokens[p].text);if(base==model.named_types.end()||model.store.kind(base->second)!=TypeKind::Object){valid=false;break;}for(const auto&property:model.store.properties(base->second)){auto existing=std::find_if(inherited.begin(),inherited.end(),[&](const auto&item){return item.name==property.name;});if(existing==inherited.end())inherited.push_back(property);else if(existing->type!=property.type||existing->optional!=property.optional||existing->readonly!=property.readonly){valid=false;break;}}}}if(!valid)continue;for(const auto&property:model.store.properties(shape)){auto existing=std::find_if(inherited.begin(),inherited.end(),[&](const auto&item){return item.name==property.name;});if(existing==inherited.end())inherited.push_back(property);else *existing=property;}if(!inherited.empty())shape=model.store.object_of(std::move(inherited));}auto found=model.named_types.find(tokens[name].text);if(found!=model.named_types.end()&&node.kind==SyntaxKind::InterfaceDeclaration&&model.store.kind(found->second)==TypeKind::Object&&model.store.kind(shape)==TypeKind::Object){auto merged=model.store.properties(found->second);for(const auto&property:model.store.properties(shape)){auto existing=std::find_if(merged.begin(),merged.end(),[&](const auto&p){return p.name==property.name;});if(existing==merged.end())merged.push_back(property);else if(existing->type==property.type&&existing->optional==property.optional&&existing->readonly==property.readonly){}else{merged.clear();break;}}if(!merged.empty())shape=model.store.object_of(std::move(merged));else continue;}model.named_types[tokens[name].text]=shape;}
    model.symbol_types.assign(binding.symbols.size(), model.store.unknown());
    model.function_signatures.resize(binding.symbols.size());
    for (std::size_t i = 0; i < binding.symbols.size(); ++i) {
        const auto node_index = binding.symbols[i].semantic_node;
        if (node_index >= semantic.nodes.size()) continue;
        const auto& node = semantic.nodes[node_index];
        if (node.kind == SemanticNodeKind::VariableDeclaration &&
            node.variable_index < program.variables.size())
            model.symbol_types[i] = annotation_type(tokens, program.variables[node.variable_index],
                                                    model.store,&model.named_types);
        else if (node.kind == SemanticNodeKind::ParameterDeclaration)
            model.symbol_types[i] = node_annotation(tokens, node, model.store,&model.named_types);
        else if (binding.symbols[i].kind == SymbolKind::Function)
            model.symbol_types[i] = model.store.function();
    }
    for (std::size_t symbol = 0; symbol < binding.symbols.size(); ++symbol) {
        if (binding.symbols[symbol].kind != SymbolKind::Function) continue;
        const auto node_index = binding.symbols[symbol].semantic_node;
        if (node_index >= semantic.nodes.size()) continue;
        const auto& function = semantic.nodes[node_index];
        std::vector<std::size_t> parameters;
        for (std::size_t candidate = 0; candidate < binding.symbols.size(); ++candidate) {
            if (binding.symbols[candidate].kind != SymbolKind::Parameter) continue;
            const auto parameter_node = binding.symbols[candidate].semantic_node;
            if (parameter_node < semantic.nodes.size() &&
                semantic.nodes[parameter_node].scope_token == function.scope_token)
                parameters.push_back(candidate);
        }
        std::sort(parameters.begin(), parameters.end(), [&](auto a, auto b) {
            return binding.symbols[a].declaration_token < binding.symbols[b].declaration_token;
        });
        auto& signature = model.function_signatures[symbol];
        for (std::size_t parameter_index=0;parameter_index<parameters.size();++parameter_index) {
            const auto parameter=parameters[parameter_index];
            signature.parameters.push_back(model.symbol_types[parameter]);
            const auto& parameter_node = semantic.nodes[binding.symbols[parameter].semantic_node];
            bool optional = false, rest = false;
            for (auto token = parameter_node.begin_token; token < parameter_node.end_token; ++token) {
                optional = optional || tokens[token].text == "?" || tokens[token].text == "=";
                rest = rest || tokens[token].text == "...";
            }
            const auto parameter_end=parameter_index+1<parameters.size()?binding.symbols[parameters[parameter_index+1]].declaration_token:function.scope_token;int parameter_depth=0;
            for(auto token=binding.symbols[parameter].declaration_token;token<parameter_end;++token){if(tokens[token].text=="("){++parameter_depth;continue;}if(tokens[token].text==")"){if(parameter_depth){--parameter_depth;continue;}break;}optional=optional||tokens[token].text=="?"||tokens[token].text=="=";rest=rest||tokens[token].text=="...";}
            signature.rest = signature.rest || rest;
            if (!optional && !rest) ++signature.required_parameters;
        }
        std::size_t open = function.begin_token, close = function.begin_token;
        int depth = 0; bool seen = false;
        for (auto i = function.begin_token; i < function.scope_token; ++i) {
            if (tokens[i].text == "(") { if(!seen)open=i;++depth; seen = true; }
            else if (tokens[i].text == ")" && depth && --depth == 0) { close = i; break; }
        }
        if(seen&&!parameters.empty()){signature.required_parameters=0;signature.rest=false;bool content=false,optional=false;int nested=0;for(auto i=open+1;i<=close;++i){const auto&t=tokens[i].text;if(i==close||(t==","&&nested==0)){if(content&&!optional)++signature.required_parameters;content=false;optional=false;continue;}if(tokens[i].kind==TokenKind::Comment)continue;if(t=="("||t=="{"||t=="["){++nested;content=true;}else if(t==")"||t=="}"||t=="]"){if(nested)--nested;}else{content=true;if(nested==0&&(t=="?"||t=="="))optional=true;if(nested==0&&t=="..."){optional=true;signature.rest=true;}}}}
        if(parameters.empty()&&seen){std::size_t count=0;bool content=false;int nested=0;for(auto i=open+1;i<close;++i){if(tokens[i].kind==TokenKind::Comment)continue;const auto&t=tokens[i].text;if(t=="("||t=="{"||t=="["){++nested;content=true;}else if(t==")"||t=="}"||t=="]"){if(nested)--nested;}else if(t==","&&nested==0){++count;}else content=true;}if(content){++count;signature.parameters.assign(count,model.store.unknown());signature.required_parameters=count;}}
        for (auto i = close + 1; seen && i < function.scope_token; ++i) {
            if (tokens[i].kind == TokenKind::Comment) continue;
            if (tokens[i].text == ":") {
                std::vector<TypeId>members;for(++i;i<function.scope_token;++i){if(tokens[i].kind==TokenKind::Comment||tokens[i].text=="|")continue;auto type=named_type(tokens[i].text,model.store);if(tokens[i].kind==TokenKind::String)type=model.store.literal(model.store.string(),literal_value(tokens[i].text));else if(tokens[i].kind==TokenKind::Number)type=model.store.literal(tokens[i].text.back()=='n'?model.store.bigint():model.store.number(),tokens[i].text);if(type==model.store.unknown()){members.clear();break;}members.push_back(type);}signature.result=model.store.union_of(std::move(members));
                break;
            }
        }
        signature.valid = true;
        model.symbol_types[symbol]=model.store.function_of(signature.parameters,signature.result,signature.required_parameters,signature.rest);
    }
    return model;
}

} // namespace tscc
