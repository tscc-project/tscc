#pragma once
#include "Binder.h"
#include "Lexer.h"
#include "Semantic.h"
#include "Syntax.h"
#include <cstddef>
#include <string>
#include <vector>

namespace tscc {

using TypeId = std::size_t;
enum class TypeKind { Unknown, Number, String, Boolean, BigInt, Function, Null, Undefined, Literal, Union };
struct Type { TypeKind kind = TypeKind::Unknown;TypeId base=0;std::string literal;std::vector<TypeId> members; };

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
    TypeKind kind(TypeId) const;
    TypeId widen(TypeId) const;
    std::string name(TypeId) const;
private:
    mutable std::vector<Type> types_;
};

struct TypeModel {
    TypeStore store;
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
                           const SemanticModel&, const BindingModel&);

} // namespace tscc
