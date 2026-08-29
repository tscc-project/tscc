#include "Type.h"
#include <algorithm>

namespace tscc {
namespace {
std::string literal_value(const std::string&text){return text.size()>=2&&(text.front()=='\''||text.front()=='"')?text.substr(1,text.size()-2):text;}
TypeId annotation_type(const std::vector<Token>& tokens,
                       const VariableDeclaration& declaration,
                       const TypeStore& store) {
    std::vector<TypeId> members;
    for (auto i = declaration.type_begin_token; i < declaration.type_end_token; ++i) {
        if (tokens[i].kind == TokenKind::Comment || tokens[i].text == "|") continue;
        TypeId type=store.unknown();
        if(tokens[i].text=="number")type=store.number();else if(tokens[i].text=="string")type=store.string();else if(tokens[i].text=="boolean")type=store.boolean();else if(tokens[i].text=="bigint")type=store.bigint();else if(tokens[i].text=="null")type=store.null();else if(tokens[i].text=="undefined")type=store.undefined();
        else if(tokens[i].kind==TokenKind::String)type=store.literal(store.string(),literal_value(tokens[i].text));
        else if(tokens[i].kind==TokenKind::Number)type=store.literal(tokens[i].text.back()=='n'?store.bigint():store.number(),tokens[i].text);
        else if(tokens[i].text=="true"||tokens[i].text=="false")type=store.literal(store.boolean(),tokens[i].text);
        if(type==store.unknown())return type;
        members.push_back(type);
    }
    return store.union_of(std::move(members));
}
TypeId named_type(const std::string& text, const TypeStore& store) {
    if (text == "number") return store.number();
    if (text == "string") return store.string();
    if (text == "boolean") return store.boolean();
    if (text == "bigint") return store.bigint();
    if (text == "null") return store.null();
    if (text == "undefined") return store.undefined();
    return store.unknown();
}
TypeId node_annotation(const std::vector<Token>& tokens, const SemanticNode& node,
                       const TypeStore& store) {
    bool colon = false;std::vector<TypeId>members;
    for (auto i = node.name_token + 1; i < node.end_token && i < tokens.size(); ++i) {
        if (tokens[i].kind == TokenKind::Comment) continue;
        if (tokens[i].text == ":") { colon = true; continue; }
        if (!colon||tokens[i].text=="|"||tokens[i].text=="?"||tokens[i].text=="...") continue;
        if(tokens[i].text=="=")break;
        TypeId type=named_type(tokens[i].text,store);
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
    : types_{{TypeKind::Unknown,0,"",{},{}}, {TypeKind::Number,0,"",{},{}},
             {TypeKind::String,0,"",{},{}}, {TypeKind::Boolean,0,"",{},{}},
             {TypeKind::BigInt,0,"",{},{}}, {TypeKind::Function,0,"",{},{}},
             {TypeKind::Null,0,"",{},{}}, {TypeKind::Undefined,0,"",{},{}}} {}

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
        case TypeKind::Function: return "function";
        case TypeKind::Null:return "null";case TypeKind::Undefined:return "undefined";
        case TypeKind::Literal:return types_[id].base==string()?"'"+types_[id].literal+"'":types_[id].literal;
        case TypeKind::Union:{std::string out;for(auto member:types_[id].members){if(!out.empty())out+=" | ";out+=name(member);}return out;}
        case TypeKind::Object:return "object";
        default: return "unknown";
    }
}

TypeId TypeStore::literal(TypeId base,const std::string&value)const{for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Literal&&types_[i].base==base&&types_[i].literal==value)return i;types_.push_back({TypeKind::Literal,base,value,{}, {}});return types_.size()-1;}
TypeId TypeStore::union_of(std::vector<TypeId> members)const{std::vector<TypeId>flat;for(auto id:members){if(id==unknown())return id;if(kind(id)==TypeKind::Union)flat.insert(flat.end(),types_[id].members.begin(),types_[id].members.end());else flat.push_back(id);}std::sort(flat.begin(),flat.end());flat.erase(std::unique(flat.begin(),flat.end()),flat.end());std::vector<TypeId>bases;for(auto id:flat)if(kind(id)!=TypeKind::Literal)bases.push_back(id);flat.erase(std::remove_if(flat.begin(),flat.end(),[&](auto x){return kind(x)==TypeKind::Literal&&std::find(bases.begin(),bases.end(),types_[x].base)!=bases.end();}),flat.end());if(flat.empty())return unknown();if(flat.size()==1)return flat[0];for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Union&&types_[i].members==flat)return i;types_.push_back({TypeKind::Union,0,"",flat,{}});return types_.size()-1;}
bool TypeStore::assignable(TypeId actual,TypeId expected)const{if(actual==unknown()||expected==unknown()||actual==expected)return true;if(kind(expected)==TypeKind::Union){for(auto member:types_[expected].members)if(assignable(actual,member))return true;return false;}if(kind(actual)==TypeKind::Union){for(auto member:types_[actual].members)if(!assignable(member,expected))return false;return true;}if(kind(actual)==TypeKind::Literal)return types_[actual].base==expected;return false;}
TypeId TypeStore::object_of(std::vector<TypeProperty>properties)const{std::sort(properties.begin(),properties.end(),[](const auto&a,const auto&b){return a.name<b.name;});for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Object&&types_[i].properties.size()==properties.size()){bool same=true;for(std::size_t p=0;p<properties.size();++p){const auto&a=types_[i].properties[p];const auto&b=properties[p];same= same&&a.name==b.name&&a.type==b.type&&a.optional==b.optional&&a.readonly==b.readonly;}if(same)return i;}types_.push_back({TypeKind::Object,0,"",{},std::move(properties)});return types_.size()-1;}
const TypeProperty*TypeStore::property(TypeId id,const std::string&name)const{if(kind(id)!=TypeKind::Object)return nullptr;for(const auto&property:types_[id].properties)if(property.name==name)return &property;return nullptr;}

TypeModel build_type_model(const std::vector<Token>& tokens, const Program& program,
                           const SemanticModel& semantic, const BindingModel& binding) {
    TypeModel model;
    model.symbol_types.assign(binding.symbols.size(), model.store.unknown());
    model.function_signatures.resize(binding.symbols.size());
    for (std::size_t i = 0; i < binding.symbols.size(); ++i) {
        const auto node_index = binding.symbols[i].semantic_node;
        if (node_index >= semantic.nodes.size()) continue;
        const auto& node = semantic.nodes[node_index];
        if (node.kind == SemanticNodeKind::VariableDeclaration &&
            node.variable_index < program.variables.size())
            model.symbol_types[i] = annotation_type(tokens, program.variables[node.variable_index],
                                                    model.store);
        else if (node.kind == SemanticNodeKind::ParameterDeclaration)
            model.symbol_types[i] = node_annotation(tokens, node, model.store);
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
        for (auto parameter : parameters) {
            signature.parameters.push_back(model.symbol_types[parameter]);
            const auto& parameter_node = semantic.nodes[binding.symbols[parameter].semantic_node];
            bool optional = false, rest = false;
            for (auto token = parameter_node.begin_token; token < parameter_node.end_token; ++token) {
                optional = optional || tokens[token].text == "?" || tokens[token].text == "=";
                rest = rest || tokens[token].text == "...";
            }
            signature.rest = signature.rest || rest;
            if (!optional && !rest) ++signature.required_parameters;
        }
        std::size_t close = function.begin_token;
        int depth = 0; bool seen = false;
        for (auto i = function.begin_token; i < function.scope_token; ++i) {
            if (tokens[i].text == "(") { ++depth; seen = true; }
            else if (tokens[i].text == ")" && depth && --depth == 0) { close = i; break; }
        }
        for (auto i = close + 1; seen && i < function.scope_token; ++i) {
            if (tokens[i].kind == TokenKind::Comment) continue;
            if (tokens[i].text == ":") {
                std::vector<TypeId>members;for(++i;i<function.scope_token;++i){if(tokens[i].kind==TokenKind::Comment||tokens[i].text=="|")continue;auto type=named_type(tokens[i].text,model.store);if(tokens[i].kind==TokenKind::String)type=model.store.literal(model.store.string(),literal_value(tokens[i].text));else if(tokens[i].kind==TokenKind::Number)type=model.store.literal(tokens[i].text.back()=='n'?model.store.bigint():model.store.number(),tokens[i].text);if(type==model.store.unknown()){members.clear();break;}members.push_back(type);}signature.result=model.store.union_of(std::move(members));
                break;
            }
        }
        signature.valid = true;
    }
    return model;
}

} // namespace tscc
