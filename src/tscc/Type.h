#pragma once
#include "Binder.h"
#include "Lexer.h"
#include "Semantic.h"
#include "Syntax.h"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace tscc {

using TypeId = std::size_t;
enum class TypeKind { Unknown, Number, String, Boolean, BigInt, Function, Null, Undefined, Literal, Union, Object };
struct TypeProperty{std::string name;TypeId type=0;bool optional=false;bool readonly=false;};
struct Type { TypeKind kind = TypeKind::Unknown;TypeId base=0;std::string literal;std::vector<TypeId> members;std::vector<TypeProperty>properties;std::vector<TypeId>parameters;TypeId result=0;std::size_t required_parameters=0;bool rest=false;TypeId string_index=0;TypeId number_index=0;TypeId call_signature=0; };

class TypeStore {
public:
    TypeStore();
    TypeId unknown() const { return 0; }
    TypeId number() const { return 1; }
    TypeId string() const { return 2; }
    TypeId boolean() const { return 3; }
    TypeId bigint() const { return 4; }
    TypeId function() const { return 5; }
    TypeId null() const { return 6; }
    TypeId undefined() const { return 7; }
    TypeId literal(TypeId,const std::string&) const;
    TypeId union_of(std::vector<TypeId>) const;
    bool assignable(TypeId actual,TypeId expected) const;
    TypeId object_of(std::vector<TypeProperty>,TypeId string_index=0,TypeId number_index=0,TypeId call_signature=0)const;
    TypeId array_of(TypeId)const;
    TypeId tuple_of(std::vector<TypeId>)const;
    TypeId array_element(TypeId)const;
    const std::vector<TypeId>& tuple_elements(TypeId)const;
    TypeId function_of(std::vector<TypeId>,TypeId,std::size_t,bool)const;
    const Type* callable(TypeId)const;
    const TypeProperty* property(TypeId,const std::string&)const;
    const std::vector<TypeProperty>& properties(TypeId)const;
    TypeId string_index(TypeId)const;
    TypeId number_index(TypeId)const;
    TypeKind kind(TypeId) const;
    TypeId import_from(const TypeStore&,TypeId) const;
    TypeId widen(TypeId) const;
    std::string name(TypeId) const;
private:
    mutable std::vector<Type> types_;
};

struct TypeModel {
    TypeStore store;
    std::unordered_map<std::string,TypeId> named_types;
    std::vector<TypeId> symbol_types;
    struct FunctionSignature {
        std::vector<TypeId> parameters;
        TypeId result = 0;
        std::size_t required_parameters = 0;
        bool rest = false;
        bool valid = false;
    };
    std::vector<FunctionSignature> function_signatures;
};

TypeModel build_type_model(const std::vector<Token>&, const Program&,
                           const SemanticModel&, const BindingModel&,
                           TypeModel* seed=nullptr);
bool add_library_bundle(TypeModel&,const std::string&);

} // namespace tscc
