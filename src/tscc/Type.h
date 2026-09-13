#pragma once
#include "Binder.h"
#include "Lexer.h"
#include "Semantic.h"
#include "Syntax.h"
#include <cstddef>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace tscc {

using TypeId = std::size_t;
enum class TypeKind { Unknown, Number, String, Boolean, BigInt, Function, Null, Undefined, Symbol, Never, Literal, Union, Intersection, Object, TypeParameter, Keyof, IndexedAccess, Mapped, Conditional, TemplateLiteral };
enum TupleElementFlag : unsigned char { TupleRequired = 0, TupleOptional = 1, TupleRest = 2 };
struct TypeProperty{std::string name;TypeId type=0;bool optional=false;bool readonly=false;};
struct Type { TypeKind kind = TypeKind::Unknown;TypeId base=0;std::string literal;std::vector<TypeId> members;std::vector<TypeProperty>properties;std::vector<TypeId>parameters;TypeId result=0;std::size_t required_parameters=0;bool rest=false;TypeId string_index=0;TypeId number_index=0;TypeId call_signature=0;TypeId symbol_index=0;std::vector<unsigned char> element_flags;bool readonly_collection=false; };

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
    TypeId symbol() const { return 8; }
    TypeId never() const { return 9; }
    TypeId literal(TypeId,const std::string&) const;
    TypeId union_of(std::vector<TypeId>) const;
    TypeId intersection_of(std::vector<TypeId>)const;
    TypeId non_nullable(TypeId) const;
    TypeId narrow_primitive(TypeId,TypeKind,bool=false)const;
    TypeId narrow_literal(TypeId,TypeId,bool=false)const;
    TypeId narrow_discriminant(TypeId,const std::string&,TypeId,bool=false)const;
    TypeId narrow_property(TypeId,const std::string&,bool=false)const;
    bool assignable(TypeId actual,TypeId expected) const;
    TypeId object_of(std::vector<TypeProperty>,TypeId string_index=0,TypeId number_index=0,TypeId call_signature=0,TypeId symbol_index=0)const;
    TypeId array_of(TypeId,bool readonly=false)const;
    TypeId tuple_of(std::vector<TypeId>,std::vector<unsigned char> flags={},bool readonly=false)const;
    TypeId array_element(TypeId)const;
    const std::vector<TypeId>& tuple_elements(TypeId)const;
    const std::vector<unsigned char>& tuple_flags(TypeId)const;
    bool readonly_collection(TypeId)const;
    TypeId indexed_access(TypeId,TypeId)const;
    TypeId keyof_type(TypeId)const;
    TypeId mapped_of(TypeId key_parameter,TypeId keys,TypeId value,int optional_mode=0,int readonly_mode=0)const;
    TypeId conditional_of(TypeId check,TypeId constraint,TypeId when_true,TypeId when_false)const;
    TypeId template_literal(const std::string&,const std::unordered_map<std::string,TypeId>* =nullptr)const;
    TypeId function_of(std::vector<TypeId>,TypeId,std::size_t,bool)const;
    TypeId type_parameter(const std::string&,TypeId=0,TypeId=0)const;
    TypeId substitute(TypeId,const std::unordered_map<TypeId,TypeId>&,std::size_t=0)const;
    TypeId parameter_constraint(TypeId)const;
    TypeId parameter_default(TypeId)const;
    void define_generic(const std::string&,std::vector<TypeId>,std::vector<TypeId>,std::vector<TypeId>,TypeId)const;
    TypeId instantiate_generic(const std::string&,const std::vector<TypeId>&,bool* =nullptr)const;
    void infer_candidates(TypeId,TypeId,std::unordered_map<TypeId,std::vector<TypeId>>&,std::size_t=0)const;
    const Type* callable(TypeId)const;
    const TypeProperty* property(TypeId,const std::string&)const;
    const std::vector<TypeProperty>& properties(TypeId)const;
    TypeId string_index(TypeId)const;
    TypeId number_index(TypeId)const;
    TypeId symbol_index(TypeId)const;
    TypeKind kind(TypeId) const;
    TypeId import_from(const TypeStore&,TypeId) const;
    TypeId widen(TypeId) const;
    std::string name(TypeId) const;
private:
    struct GenericDefinition{std::vector<TypeId>parameters,constraints,defaults;TypeId body=0;};
    mutable std::vector<Type> types_;
    mutable std::unordered_map<std::string,GenericDefinition>generics_;
};

struct TypeModel {
    TypeStore store;
    std::unordered_map<std::string,TypeId> named_types;
    std::vector<TypeId> symbol_types;
    struct FunctionSignature {
        struct TypeParameter { std::string name; TypeId type=0,constraint=0,default_type=0; };
        std::vector<TypeId> parameters;
        std::vector<TypeParameter> type_parameters;
        TypeId result = 0;
        std::size_t required_parameters = 0;
        bool rest = false;
        bool valid = false;
    };
    std::vector<FunctionSignature> function_signatures;
    // Indexed by bound symbol. Every symbol in one declaration group points at
    // the same ordered public overload surface. When overload declarations are
    // present, the implementation signature is deliberately excluded.
    std::vector<std::vector<FunctionSignature>> overload_sets;
    enum class Accessibility { Public, Protected, Private };
    struct ClassMember {
        ClassMember() = default;
        ClassMember(std::string member_name, TypeId member_type,
                    std::size_t begin, std::size_t end,
                    Accessibility member_accessibility, bool member_static,
                    bool member_readonly, bool member_abstract,
                    bool member_override, bool member_method,
                    bool member_declaration_only = false,
                    std::vector<FunctionSignature> member_overloads = {})
            : name(std::move(member_name)), type(member_type), begin_token(begin),
              end_token(end), accessibility(member_accessibility),
              is_static(member_static), readonly(member_readonly),
              abstract_member(member_abstract), override_member(member_override),
              method(member_method), declaration_only(member_declaration_only),
              overloads(std::move(member_overloads)) {}
        std::string name;
        TypeId type = 0;
        std::size_t begin_token = 0;
        std::size_t end_token = 0;
        Accessibility accessibility = Accessibility::Public;
        bool is_static = false;
        bool readonly = false;
        bool abstract_member = false;
        bool override_member = false;
        bool method = false;
        bool declaration_only = false;
        std::vector<FunctionSignature> overloads;
    };
    struct ClassInfo {
        std::string name;
        std::size_t symbol = static_cast<std::size_t>(-1);
        std::size_t declaration_token = 0;
        std::size_t body_begin_token = 0;
        std::size_t body_end_token = 0;
        TypeId instance_type = 0;
        TypeId static_type = 0;
        FunctionSignature constructor;
        std::vector<FunctionSignature> constructor_overloads;
        std::vector<ClassMember> members;
        std::vector<FunctionSignature::TypeParameter> type_parameters;
        std::string base_name;
        TypeId declared_base_type = 0;
        std::vector<std::string> implements_names;
        std::vector<TypeId> implements_types;
        std::size_t base_class = static_cast<std::size_t>(-1);
        bool abstract_class = false;
    };
    struct SemanticIssue { std::size_t token = 0; std::string message; };
    std::vector<ClassInfo> classes;
    std::vector<std::size_t> symbol_classes;
    std::vector<SemanticIssue> class_issues;
};

TypeModel build_type_model(const std::vector<Token>&, const Program&,
                           const SemanticModel&, const BindingModel&,
                           TypeModel* seed=nullptr);
bool add_library_bundle(TypeModel&,const std::string&);

} // namespace tscc
