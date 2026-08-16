#pragma once
#include "Binder.h"
#include "Lexer.h"
#include "Semantic.h"
#include "Syntax.h"
#include <cstddef>
#include <vector>

namespace tscc {

using TypeId = std::size_t;
enum class TypeKind { Unknown, Number, String, Boolean, BigInt };
struct Type { TypeKind kind = TypeKind::Unknown; };

class TypeStore {
public:
    TypeStore();
    TypeId unknown() const { return 0; }
    TypeId number() const { return 1; }
    TypeId string() const { return 2; }
    TypeId boolean() const { return 3; }
    TypeId bigint() const { return 4; }
    TypeKind kind(TypeId) const;
    const char* name(TypeId) const;
private:
    std::vector<Type> types_;
};

struct TypeModel {
    TypeStore store;
    std::vector<TypeId> symbol_types;
};

TypeModel build_type_model(const std::vector<Token>&, const Program&,
                           const SemanticModel&, const BindingModel&);

} // namespace tscc
