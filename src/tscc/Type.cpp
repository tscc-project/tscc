#include "Type.h"
#include "TypeParser.h"
#include "TypeParser.cpp"
#include <algorithm>
#include <functional>
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
    if(first<declaration.type_end_token){auto at=first;auto parsed=parse_type_annotation(tokens,at,declaration.type_end_token,store,named);while(at<declaration.type_end_token&&tokens[at].kind==TokenKind::Comment)++at;if(parsed!=store.unknown()&&at==declaration.type_end_token)return parsed;}
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
    auto begin=node.name_token+1;
    while(begin<node.end_token&&tokens[begin].kind==TokenKind::Comment)++begin;
    if(begin<node.end_token&&tokens[begin].text=="?")++begin;
    while(begin<node.end_token&&tokens[begin].kind==TokenKind::Comment)++begin;
    if(begin>=node.end_token||tokens[begin].text!=":")return store.unknown();
    ++begin;return parse_type_annotation(tokens,begin,node.end_token,store,named);
}
}

TypeStore::TypeStore()
    : types_{{TypeKind::Unknown,0,"",{},{},{},0,0,false}, {TypeKind::Number,0,"",{},{},{},0,0,false},
             {TypeKind::String,0,"",{},{},{},0,0,false}, {TypeKind::Boolean,0,"",{},{},{},0,0,false},
             {TypeKind::BigInt,0,"",{},{},{},0,0,false}, {TypeKind::Function,0,"",{},{},{},0,0,false},
             {TypeKind::Null,0,"",{},{},{},0,0,false}, {TypeKind::Undefined,0,"",{},{},{},0,0,false}} {}

bool add_library_bundle(TypeModel&model,const std::string&name){
    auto method=[&](std::vector<TypeId>p,TypeId result){auto required=p.size();return model.store.function_of(std::move(p),result,required,false);};
    if(name=="es2022"){
        model.named_types["Date"]=model.store.object_of({{"getTime",method({},model.store.number()),false,true},{"toISOString",method({},model.store.string()),false,true}});
        model.named_types["RegExp"]=model.store.object_of({{"test",method({model.store.string()},model.store.boolean()),false,true}});
        model.named_types["Error"]=model.store.object_of({{"message",model.store.string(),false,false},{"name",model.store.string(),false,false}});
        return true;
    }
    if(name=="node"){
        model.named_types["Buffer"]=model.store.object_of({{"length",model.store.number(),false,true},{"toString",method({},model.store.string()),false,true}});
        model.named_types["ProcessEnv"]=model.store.object_of({},model.store.union_of({model.store.string(),model.store.undefined()}));
        return true;
    }
    return false;
}

TypeKind TypeStore::kind(TypeId id) const {
    return id < types_.size() ? types_[id].kind : TypeKind::Unknown;
}
TypeId TypeStore::import_from(const TypeStore&source,TypeId id)const{
    std::unordered_map<TypeId,TypeId>memo;
    std::function<TypeId(TypeId)>copy=[&](TypeId current)->TypeId{
        if(current<=source.undefined())return current;
        if(auto found=memo.find(current);found!=memo.end())return found->second;
        if(current>=source.types_.size())return unknown();
        const auto&type=source.types_[current];TypeId result=unknown();
        if(type.kind==TypeKind::Literal)result=literal(copy(type.base),type.literal);
        else if(type.kind==TypeKind::Union){std::vector<TypeId>members;for(auto member:type.members)members.push_back(copy(member));result=union_of(std::move(members));}
        else if(type.kind==TypeKind::Function){std::vector<TypeId>parameters;for(auto parameter:type.parameters)parameters.push_back(copy(parameter));result=function_of(std::move(parameters),copy(type.result),type.required_parameters,type.rest);}
        else if(type.kind==TypeKind::Object){std::vector<TypeProperty>properties;for(const auto&property:type.properties)properties.push_back({property.name,copy(property.type),property.optional,property.readonly});if(type.literal=="array")result=array_of(copy(type.result));else if(type.literal=="tuple"){std::vector<TypeId>elements;for(auto element:type.parameters)elements.push_back(copy(element));result=tuple_of(std::move(elements));}else result=object_of(std::move(properties),copy(type.string_index),copy(type.number_index),copy(type.call_signature));}
        memo[current]=result;return result;
    };
    return copy(id);
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
        case TypeKind::Object:{if(types_[id].literal=="array")return name(types_[id].result)+"[]";if(types_[id].literal=="tuple"){std::string out="[";for(std::size_t i=0;i<types_[id].parameters.size();++i){if(i)out+=", ";out+=name(types_[id].parameters[i]);}return out+"]";}return "object";}
        case TypeKind::TypeParameter:return types_[id].literal;
        default: return "unknown";
    }
}

TypeId TypeStore::literal(TypeId base,const std::string&value)const{for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Literal&&types_[i].base==base&&types_[i].literal==value)return i;types_.push_back({TypeKind::Literal,base,value,{},{},{},0,0,false});return types_.size()-1;}
TypeId TypeStore::union_of(std::vector<TypeId> members)const{std::vector<TypeId>flat;for(auto id:members){if(id==unknown())return id;if(kind(id)==TypeKind::Union)flat.insert(flat.end(),types_[id].members.begin(),types_[id].members.end());else flat.push_back(id);}std::sort(flat.begin(),flat.end());flat.erase(std::unique(flat.begin(),flat.end()),flat.end());std::vector<TypeId>bases;for(auto id:flat)if(kind(id)!=TypeKind::Literal)bases.push_back(id);flat.erase(std::remove_if(flat.begin(),flat.end(),[&](auto x){return kind(x)==TypeKind::Literal&&std::find(bases.begin(),bases.end(),types_[x].base)!=bases.end();}),flat.end());if(flat.empty())return unknown();if(flat.size()==1)return flat[0];for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Union&&types_[i].members==flat)return i;types_.push_back({TypeKind::Union,0,"",flat,{},{},0,0,false});return types_.size()-1;}
bool TypeStore::assignable(TypeId actual,TypeId expected)const{if(actual==unknown()||expected==unknown()||actual==expected)return true;if(kind(expected)==TypeKind::Union){for(auto member:types_[expected].members)if(assignable(actual,member))return true;return false;}if(kind(actual)==TypeKind::Union){for(auto member:types_[actual].members)if(!assignable(member,expected))return false;return true;}if(kind(actual)==TypeKind::Literal)return types_[actual].base==expected;if(kind(actual)==TypeKind::Function&&kind(expected)==TypeKind::Function){const auto*a=callable(actual),*e=callable(expected);if(!a||!e)return actual==function()||expected==function();if(a->required_parameters>e->required_parameters||(!a->rest&&a->parameters.size()<e->parameters.size()))return false;for(std::size_t i=0;i<e->parameters.size();++i){const auto ai=i<a->parameters.size()?i:a->parameters.size()-1;if(a->parameters.empty()||!assignable(e->parameters[i],a->parameters[ai]))return false;}return assignable(a->result,e->result);}if(kind(expected)==TypeKind::Object&&types_[expected].call_signature!=unknown()&&kind(actual)==TypeKind::Function)return assignable(actual,types_[expected].call_signature);if(kind(actual)==TypeKind::Object&&kind(expected)==TypeKind::Object){if(types_[expected].call_signature!=unknown()&&(types_[actual].call_signature==unknown()||!assignable(types_[actual].call_signature,types_[expected].call_signature)))return false;for(const auto&wanted:types_[expected].properties){const auto*got=property(actual,wanted.name);if(!got){if(wanted.optional)continue;return false;}if(!assignable(got->type,wanted.type))return false;}if(types_[expected].string_index!=unknown()){for(const auto&got:types_[actual].properties)if(!assignable(got.type,types_[expected].string_index))return false;if(types_[actual].string_index!=unknown()&&!assignable(types_[actual].string_index,types_[expected].string_index))return false;}if(types_[expected].number_index!=unknown()&&types_[actual].number_index!=unknown()&&!assignable(types_[actual].number_index,types_[expected].number_index))return false;return true;}return false;}
TypeId TypeStore::object_of(std::vector<TypeProperty>properties,TypeId string_index,TypeId number_index,TypeId call_signature)const{std::sort(properties.begin(),properties.end(),[](const auto&a,const auto&b){return a.name<b.name;});for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Object&&types_[i].properties.size()==properties.size()&&types_[i].string_index==string_index&&types_[i].number_index==number_index&&types_[i].call_signature==call_signature){bool same=true;for(std::size_t p=0;p<properties.size();++p){const auto&a=types_[i].properties[p];const auto&b=properties[p];same= same&&a.name==b.name&&a.type==b.type&&a.optional==b.optional&&a.readonly==b.readonly;}if(same)return i;}Type type;type.kind=TypeKind::Object;type.properties=std::move(properties);type.string_index=string_index;type.number_index=number_index;type.call_signature=call_signature;types_.push_back(std::move(type));return types_.size()-1;}
TypeId TypeStore::array_of(TypeId element)const{for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Object&&types_[i].literal=="array"&&types_[i].result==element)return i;Type type;type.kind=TypeKind::Object;type.literal="array";type.result=element;type.number_index=element;type.properties={{"length",number(),false,true}};types_.push_back(std::move(type));return types_.size()-1;}
TypeId TypeStore::tuple_of(std::vector<TypeId>elements)const{for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Object&&types_[i].literal=="tuple"&&types_[i].parameters==elements)return i;Type type;type.kind=TypeKind::Object;type.literal="tuple";type.parameters=elements;type.number_index=union_of(elements);type.properties.push_back({"length",literal(number(),std::to_string(elements.size())),false,true});for(std::size_t i=0;i<elements.size();++i)type.properties.push_back({std::to_string(i),elements[i],false,false});types_.push_back(std::move(type));return types_.size()-1;}
TypeId TypeStore::array_element(TypeId id)const{return kind(id)==TypeKind::Object&&types_[id].literal=="array"?types_[id].result:unknown();}
const std::vector<TypeId>&TypeStore::tuple_elements(TypeId id)const{static const std::vector<TypeId>empty;return kind(id)==TypeKind::Object&&types_[id].literal=="tuple"?types_[id].parameters:empty;}
const TypeProperty*TypeStore::property(TypeId id,const std::string&name)const{if(kind(id)!=TypeKind::Object)return nullptr;for(const auto&property:types_[id].properties)if(property.name==name)return &property;if(types_[id].string_index!=unknown()){static thread_local TypeProperty indexed;indexed={name,types_[id].string_index,false,false};return &indexed;}return nullptr;}
const std::vector<TypeProperty>&TypeStore::properties(TypeId id)const{static const std::vector<TypeProperty>empty;return kind(id)==TypeKind::Object?types_[id].properties:empty;}
TypeId TypeStore::string_index(TypeId id)const{return kind(id)==TypeKind::Object?types_[id].string_index:unknown();}
TypeId TypeStore::number_index(TypeId id)const{return kind(id)==TypeKind::Object?types_[id].number_index:unknown();}
TypeId TypeStore::function_of(std::vector<TypeId>parameters,TypeId result,std::size_t required,bool rest)const{for(TypeId i=8;i<types_.size();++i)if(types_[i].kind==TypeKind::Function&&types_[i].parameters==parameters&&types_[i].result==result&&types_[i].required_parameters==required&&types_[i].rest==rest)return i;Type type;type.kind=TypeKind::Function;type.parameters=std::move(parameters);type.result=result;type.required_parameters=required;type.rest=rest;types_.push_back(std::move(type));return types_.size()-1;}
const Type*TypeStore::callable(TypeId id)const{if(kind(id)==TypeKind::Object&&types_[id].call_signature!=unknown())id=types_[id].call_signature;return kind(id)==TypeKind::Function&&id!=function()?&types_[id]:nullptr;}
TypeId TypeStore::type_parameter(const std::string&name,TypeId constraint,TypeId fallback)const{Type type;type.kind=TypeKind::TypeParameter;type.literal=name;type.base=constraint;type.result=fallback;types_.push_back(std::move(type));return types_.size()-1;}
TypeId TypeStore::parameter_constraint(TypeId id)const{return kind(id)==TypeKind::TypeParameter?types_[id].base:unknown();}
TypeId TypeStore::parameter_default(TypeId id)const{return kind(id)==TypeKind::TypeParameter?types_[id].result:unknown();}
TypeId TypeStore::substitute(TypeId id,const std::unordered_map<TypeId,TypeId>&map,std::size_t depth)const{if(depth>64)return unknown();if(auto found=map.find(id);found!=map.end())return found->second;if(id>=types_.size())return unknown();const auto&type=types_[id];if(type.kind==TypeKind::Union){std::vector<TypeId>v;for(auto x:type.members)v.push_back(substitute(x,map,depth+1));return union_of(std::move(v));}if(type.kind==TypeKind::Function){std::vector<TypeId>v;for(auto x:type.parameters)v.push_back(substitute(x,map,depth+1));return function_of(std::move(v),substitute(type.result,map,depth+1),type.required_parameters,type.rest);}if(type.kind==TypeKind::Object){if(type.literal=="array")return array_of(substitute(type.result,map,depth+1));if(type.literal=="tuple"){std::vector<TypeId>v;for(auto x:type.parameters)v.push_back(substitute(x,map,depth+1));return tuple_of(std::move(v));}std::vector<TypeProperty>p;for(const auto&x:type.properties)p.push_back({x.name,substitute(x.type,map,depth+1),x.optional,x.readonly});return object_of(std::move(p),substitute(type.string_index,map,depth+1),substitute(type.number_index,map,depth+1),substitute(type.call_signature,map,depth+1));}return id;}
void TypeStore::define_generic(const std::string&name,std::vector<TypeId>parameters,std::vector<TypeId>constraints,std::vector<TypeId>defaults,TypeId body)const{generics_[name]={std::move(parameters),std::move(constraints),std::move(defaults),body};}
TypeId TypeStore::instantiate_generic(const std::string&name,const std::vector<TypeId>&arguments,bool*valid)const{auto found=generics_.find(name);if(found==generics_.end()){if(valid)*valid=false;return unknown();}const auto&definition=found->second;if(arguments.size()>definition.parameters.size()){if(valid)*valid=false;return unknown();}std::unordered_map<TypeId,TypeId>map;for(std::size_t i=0;i<definition.parameters.size();++i){auto selected=i<arguments.size()?arguments[i]:(i<definition.defaults.size()?definition.defaults[i]:unknown());if(selected==unknown()){if(valid)*valid=false;return unknown();}auto constraint=i<definition.constraints.size()?substitute(definition.constraints[i],map):unknown();if(constraint!=unknown()&&!assignable(selected,constraint)){if(valid)*valid=false;return unknown();}map[definition.parameters[i]]=selected;}if(valid)*valid=true;return substitute(definition.body,map);}
void TypeStore::infer_candidates(TypeId pattern,TypeId actual,std::unordered_map<TypeId,std::vector<TypeId>>&out,std::size_t depth)const{if(depth>64||pattern==unknown()||actual==unknown())return;if(kind(pattern)==TypeKind::TypeParameter){out[pattern].push_back(widen(actual));return;}if(pattern>=types_.size()||actual>=types_.size())return;const auto&p=types_[pattern];if(p.kind==TypeKind::Object&&kind(actual)==TypeKind::Object){if(p.literal=="array"){auto element=array_element(actual);if(element==unknown())element=number_index(actual);if(element!=unknown())infer_candidates(p.result,element,out,depth+1);return;}if(p.literal=="tuple"){const auto&elements=tuple_elements(actual);for(std::size_t i=0;i<p.parameters.size()&&i<elements.size();++i)infer_candidates(p.parameters[i],elements[i],out,depth+1);return;}for(const auto&property:p.properties)if(const auto*value=this->property(actual,property.name))infer_candidates(property.type,value->type,out,depth+1);return;}if(p.kind==TypeKind::Function&&kind(actual)==TypeKind::Function){const auto&a=types_[actual];for(std::size_t i=0;i<p.parameters.size()&&i<a.parameters.size();++i)infer_candidates(p.parameters[i],a.parameters[i],out,depth+1);infer_candidates(p.result,a.result,out,depth+1);}}

TypeModel build_type_model(const std::vector<Token>& tokens, const Program& program,
                           const SemanticModel& semantic, const BindingModel& binding,
                           TypeModel* seed) {
    TypeModel model=seed?std::move(*seed):TypeModel{};
    for(const auto&node:program.root.children){if(node.kind!=SyntaxKind::InterfaceDeclaration&&node.kind!=SyntaxKind::TypeAliasDeclaration)continue;auto name=node.begin_token+1;while(name<=node.end_token&&tokens[name].kind==TokenKind::Comment)++name;if(name>node.end_token||tokens[name].kind!=TokenKind::Identifier)continue;auto open=name+1;while(open<=node.end_token&&tokens[open].kind==TokenKind::Comment)++open;if(open>node.end_token||tokens[open].text!="<")continue;int depth=1;auto close=open+1;for(;close<=node.end_token&&depth;++close){if(tokens[close].text=="<")++depth;else if(tokens[close].text==">")--depth;}if(depth)continue;--close;auto local=model.named_types;std::vector<TypeId>parameters,constraints,defaults;std::size_t part=open+1;while(part<close){auto comma=part;int nested=0;for(;comma<close;++comma){if(tokens[comma].text=="<"||tokens[comma].text=="("||tokens[comma].text=="["||tokens[comma].text=="{")++nested;else if((tokens[comma].text==">"||tokens[comma].text==")"||tokens[comma].text=="]"||tokens[comma].text=="}")&&nested)--nested;else if(tokens[comma].text==","&&!nested)break;}while(part<comma&&tokens[part].kind==TokenKind::Comment)++part;if(part>=comma||tokens[part].kind!=TokenKind::Identifier)break;auto parameter_name=tokens[part].text;auto marker=part+1,eq=comma;while(marker<comma&&tokens[marker].text!="extends"&&tokens[marker].text!="=")++marker;if(marker<comma&&tokens[marker].text=="extends"){eq=marker+1;while(eq<comma&&tokens[eq].text!="=")++eq;}TypeId constraint=model.store.unknown(),fallback=model.store.unknown();if(marker<comma&&tokens[marker].text=="extends"){auto at=marker+1;constraint=parse_type_annotation(tokens,at,eq,model.store,&local);}if(eq<comma&&tokens[eq].text=="="){auto at=eq+1;fallback=parse_type_annotation(tokens,at,comma,model.store,&local);}auto parameter=model.store.type_parameter(parameter_name,constraint,fallback);parameters.push_back(parameter);constraints.push_back(constraint);defaults.push_back(fallback);local[parameter_name]=parameter;part=comma+1;}auto body=close+1;if(node.kind==SyntaxKind::InterfaceDeclaration){while(body<=node.end_token&&tokens[body].text!="{")++body;}else{while(body<=node.end_token&&tokens[body].text!="=")++body;if(body<=node.end_token)++body;}if(body>node.end_token||parameters.empty())continue;auto at=body;auto shape=parse_type_annotation(tokens,at,node.end_token+1,model.store,&local);if(shape==model.store.unknown())continue;model.store.define_generic(tokens[name].text,parameters,constraints,defaults,shape);model.named_types[tokens[name].text]=shape;}
    for(const auto&node:program.root.children){if(node.kind!=SyntaxKind::InterfaceDeclaration&&node.kind!=SyntaxKind::TypeAliasDeclaration)continue;auto name=node.begin_token+1;while(name<tokens.size()&&tokens[name].kind==TokenKind::Comment)++name;if(name>=tokens.size()||tokens[name].kind!=TokenKind::Identifier)continue;std::size_t begin=name+1;if(node.kind==SyntaxKind::InterfaceDeclaration){while(begin<=node.end_token&&tokens[begin].text!="{")++begin;}else{while(begin<=node.end_token&&tokens[begin].text!="=")++begin;if(begin<=node.end_token)++begin;}if(begin>node.end_token)continue;auto at=begin;auto shape=parse_annotation(tokens,at,node.end_token+1,model.store,&model.named_types);while(at<=node.end_token&&(tokens[at].kind==TokenKind::Comment||tokens[at].text==";"))++at;if(shape==model.store.unknown()||at<=node.end_token)continue;if(node.kind==SyntaxKind::InterfaceDeclaration){std::vector<TypeProperty>inherited;bool extends=false,valid=true;for(auto p=name+1;p<begin;++p){if(tokens[p].kind==TokenKind::Comment||tokens[p].text==",")continue;if(tokens[p].text=="extends"){extends=true;continue;}if(extends&&tokens[p].kind==TokenKind::Identifier){auto base=model.named_types.find(tokens[p].text);if(base==model.named_types.end()||model.store.kind(base->second)!=TypeKind::Object){valid=false;break;}for(const auto&property:model.store.properties(base->second)){auto existing=std::find_if(inherited.begin(),inherited.end(),[&](const auto&item){return item.name==property.name;});if(existing==inherited.end())inherited.push_back(property);else if(existing->type!=property.type||existing->optional!=property.optional||existing->readonly!=property.readonly){valid=false;break;}}}}if(!valid)continue;for(const auto&property:model.store.properties(shape)){auto existing=std::find_if(inherited.begin(),inherited.end(),[&](const auto&item){return item.name==property.name;});if(existing==inherited.end())inherited.push_back(property);else *existing=property;}if(!inherited.empty())shape=model.store.object_of(std::move(inherited));}auto found=model.named_types.find(tokens[name].text);if(found!=model.named_types.end()&&node.kind==SyntaxKind::InterfaceDeclaration&&model.store.kind(found->second)==TypeKind::Object&&model.store.kind(shape)==TypeKind::Object){auto merged=model.store.properties(found->second);for(const auto&property:model.store.properties(shape)){auto existing=std::find_if(merged.begin(),merged.end(),[&](const auto&p){return p.name==property.name;});if(existing==merged.end())merged.push_back(property);else if(existing->type==property.type&&existing->optional==property.optional&&existing->readonly==property.readonly){}else{merged.clear();break;}}if(!merged.empty())shape=model.store.object_of(std::move(merged));else continue;}model.named_types[tokens[name].text]=shape;}
    model.symbol_types.assign(binding.symbols.size(), model.store.unknown());
    model.function_signatures.assign(binding.symbols.size(),{});
    model.overload_sets.assign(binding.symbols.size(),{});
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
        auto& signature = model.function_signatures[symbol];
        auto local_named=model.named_types;
        std::size_t generic_open=function.begin_token;
        while(generic_open<function.scope_token&&tokens[generic_open].text!="<"&&tokens[generic_open].text!="(")++generic_open;
        if(generic_open<function.scope_token&&tokens[generic_open].text=="<"){
            int depth=1;std::size_t generic_close=generic_open+1;for(;generic_close<function.scope_token&&depth;++generic_close){if(tokens[generic_close].text=="<")++depth;else if(tokens[generic_close].text==">")--depth;}if(depth==0){std::size_t part=generic_open+1,end=generic_close-1;while(part<end){std::size_t comma=part,nested=0;for(;comma<end;++comma){if(tokens[comma].text=="<"||tokens[comma].text=="("||tokens[comma].text=="["||tokens[comma].text=="{")++nested;else if((tokens[comma].text==">"||tokens[comma].text==")"||tokens[comma].text=="]"||tokens[comma].text=="}")&&nested)--nested;else if(tokens[comma].text==","&&!nested)break;}while(part<comma&&tokens[part].kind==TokenKind::Comment)++part;if(part<comma&&(tokens[part].text=="const"||tokens[part].text=="in"||tokens[part].text=="out"))++part;if(part<comma&&tokens[part].kind==TokenKind::Identifier){const auto name=tokens[part].text;std::size_t ext=part+1,eq=comma;while(ext<comma&&tokens[ext].text!="extends"&&tokens[ext].text!="=")++ext;if(ext<comma&&tokens[ext].text=="extends"){eq=ext+1;while(eq<comma&&tokens[eq].text!="=")++eq;}TypeId constraint=model.store.unknown(),fallback=model.store.unknown();if(ext<comma&&tokens[ext].text=="extends"){auto at=ext+1;constraint=parse_type_annotation(tokens,at,eq,model.store,&local_named);}if(eq<comma&&tokens[eq].text=="="){auto at=eq+1;fallback=parse_type_annotation(tokens,at,comma,model.store,&local_named);}auto parameter=model.store.type_parameter(name,constraint,fallback);local_named[name]=parameter;signature.type_parameters.push_back({name,parameter,constraint,fallback});}part=comma+1;}}}
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
        for (std::size_t parameter_index=0;parameter_index<parameters.size();++parameter_index) {
            const auto parameter=parameters[parameter_index];
            const auto parameter_type=node_annotation(tokens,semantic.nodes[binding.symbols[parameter].semantic_node],model.store,&local_named);
            signature.parameters.push_back(parameter_type);
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
                ++i;auto at=i;signature.result=parse_type_annotation(tokens,at,function.scope_token,model.store,&local_named);
                break;
            }
        }
        signature.valid = true;
        model.symbol_types[symbol]=model.store.function_of(signature.parameters,signature.result,signature.required_parameters,signature.rest);
    }
    for(std::size_t symbol=0;symbol<binding.symbols.size();++symbol){
        if(binding.symbols[symbol].kind!=SymbolKind::Function)continue;
        std::vector<std::size_t>group;
        for(std::size_t candidate=0;candidate<binding.symbols.size();++candidate)
            if(binding.symbols[candidate].kind==SymbolKind::Function&&
               binding.symbols[candidate].scope==binding.symbols[symbol].scope&&
               binding.symbols[candidate].name==binding.symbols[symbol].name)
                group.push_back(candidate);
        std::sort(group.begin(),group.end(),[&](auto a,auto b){return binding.symbols[a].declaration_token<binding.symbols[b].declaration_token;});
        bool has_declarations=false;
        for(auto member:group){const auto node=binding.symbols[member].semantic_node;if(node<semantic.nodes.size()&&semantic.nodes[node].scope_token<tokens.size()&&tokens[semantic.nodes[node].scope_token].text=="(")has_declarations=true;}
        std::vector<TypeModel::FunctionSignature>surface;
        for(auto member:group){
            const auto node=binding.symbols[member].semantic_node;
            const bool declaration=node<semantic.nodes.size()&&semantic.nodes[node].scope_token<tokens.size()&&tokens[semantic.nodes[node].scope_token].text=="(";
            if((!has_declarations||declaration)&&member<model.function_signatures.size()&&model.function_signatures[member].valid)
                surface.push_back(model.function_signatures[member]);
        }
        model.overload_sets[symbol]=std::move(surface);
    }
    return model;
}

} // namespace tscc
