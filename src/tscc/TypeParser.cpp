#include "TypeParser.h"

namespace tscc {
namespace {
class AnnotationParser {
public:
    AnnotationParser(const std::vector<Token>& t, std::size_t& i, std::size_t end,
                     const TypeStore& store,
                     const std::unordered_map<std::string, TypeId>* named)
        : tokens(t), cursor(i), end(end), store(store), named(named) {}

    TypeId parse() {
        auto first = postfix_primary();
        if (first == store.unknown()) return first;
        std::vector<TypeId> members{first};
        while (take("|")) {
            auto member = postfix_primary();
            if (member == store.unknown()) return member;
            members.push_back(member);
        }
        noise();if(cursor<end){const auto&next=tokens[cursor].text;if(next!=","&&next!=">"&&next!=")"&&next!="]"&&next!="}"&&next!=";")return store.unknown();}
        return store.union_of(std::move(members));
    }

private:
    TypeId postfix_primary() {
        auto type = primary();
        if (type == store.unknown()) return type;
        while (take("[")) {
            if (take("]")) { type = store.array_of(type); continue; }
            auto key=parse();
            if(key==store.unknown()||!take("]"))return store.unknown();
            type=store.indexed_access(type,key);
            if(type==store.unknown())return type;
        }
        return type;
    }
    void noise() { while (cursor < end && tokens[cursor].kind == TokenKind::Comment) ++cursor; }
    bool take(const char* text) {
        noise();
        if (cursor >= end || tokens[cursor].text != text) return false;
        ++cursor;
        return true;
    }
    TypeId atom(const Token& token) const {
        if (token.kind == TokenKind::String)
            return store.literal(store.string(), token.text.substr(1, token.text.size() - 2));
        if (token.kind == TokenKind::Number)
            return store.literal(token.text.back() == 'n' ? store.bigint() : store.number(), token.text);
        if (token.text == "true" || token.text == "false") return store.literal(store.boolean(), token.text);
        if (token.text == "number") return store.number();
        if (token.text == "string") return store.string();
        if (token.text == "boolean") return store.boolean();
        if (token.text == "bigint") return store.bigint();
        if (token.text == "symbol") return store.symbol();
        if (token.text == "null") return store.null();
        if (token.text == "undefined") return store.undefined();
        if (named) { auto found = named->find(token.text); if (found != named->end()) return found->second; }
        return store.unknown();
    }
    TypeId primary() {
        noise();
        if (cursor >= end) return store.unknown();
        if(take("keyof")){auto operand=postfix_primary();return operand==store.unknown()?operand:store.keyof_type(operand);}
        if(take("readonly")){
            auto value=postfix_primary();
            if(value==store.unknown())return value;
            const auto&elements=store.tuple_elements(value);
            if(!elements.empty())return store.tuple_of(elements,store.tuple_flags(value),true);
            auto element=store.array_element(value);
            return element==store.unknown()?store.unknown():store.array_of(element,true);
        }
        if (tokens[cursor].text == "{") return object();
        if (tokens[cursor].text == "(") return function("=>");
        if (tokens[cursor].text == "[") return tuple();
        const auto name=tokens[cursor].text;auto type=atom(tokens[cursor++]);
        if(take("<")){std::vector<TypeId>arguments;while(cursor<end&&tokens[cursor].text!=">"){auto argument=parse();if(argument==store.unknown())return argument;arguments.push_back(argument);if(!take(","))break;}if(!take(">"))return store.unknown();if((name=="Array"||name=="ReadonlyArray")&&arguments.size()==1)type=store.array_of(arguments[0],name=="ReadonlyArray");else type=store.instantiate_generic(name,arguments);}
        return type;
    }
    TypeId tuple() {
        if (!take("[")) return store.unknown();
        std::vector<TypeId> elements;
        std::vector<unsigned char> flags;
        while (cursor < end && tokens[cursor].text != "]") {
            const bool rest=take("...");
            auto element = parse();
            if (element == store.unknown()) return element;
            const bool optional=take("?");
            if(rest){
                const auto&spread=store.tuple_elements(element);
                if(!spread.empty()){
                    const auto&spread_flags=store.tuple_flags(element);
                    elements.insert(elements.end(),spread.begin(),spread.end());
                    if(spread_flags.empty())flags.insert(flags.end(),spread.size(),TupleRequired);else flags.insert(flags.end(),spread_flags.begin(),spread_flags.end());
                    if (!take(",")) break;
                    continue;
                }
                const auto array_element=store.array_element(element);if(array_element!=store.unknown())element=array_element;
            }
            elements.push_back(element);
            flags.push_back(rest?TupleRest:optional?TupleOptional:TupleRequired);
            if (!take(",")) break;
        }
        return take("]") ? store.tuple_of(std::move(elements),std::move(flags)) : store.unknown();
    }
    bool parameters(std::vector<TypeId>& types, std::size_t& required, bool& rest) {
        if (!take("(")) return false;
        noise();
        while (cursor < end && tokens[cursor].text != ")") {
            const bool is_rest = take("..."); rest = rest || is_rest;
            noise();
            if (cursor >= end || tokens[cursor].kind != TokenKind::Identifier) return false;
            ++cursor;
            const bool optional = take("?");
            if (!take(":")) return false;
            auto type = parse();
            if (type == store.unknown()) return false;
            types.push_back(type);
            if (!optional && !is_rest) ++required;
            if (!take(",")) break;
        }
        return take(")");
    }
    TypeId function(const char* separator) {
        std::vector<TypeId> types; std::size_t required = 0; bool rest = false;
        if (!parameters(types, required, rest) || !take(separator)) return store.unknown();
        auto result = parse();
        return result == store.unknown() ? result : store.function_of(std::move(types), result, required, rest);
    }
    bool index(TypeId& string_index, TypeId& number_index,TypeId&symbol_index) {
        if (!take("[")) return false;
        noise();
        if (cursor >= end || tokens[cursor].kind != TokenKind::Identifier) return false;
        ++cursor;
        if (!take(":")) return false;
        noise();
        if (cursor >= end) return false;
        const auto key = tokens[cursor++].text;
        if (!take("]") || !take(":")) return false;
        auto type = parse();
        if (type == store.unknown()) return false;
        if (key == "string") string_index = type;
        else if (key == "number") number_index = type;
        else if (key == "symbol") symbol_index = type;
        else return false;
        return true;
    }
    TypeId object() {
        if (!take("{")) return store.unknown();
        std::vector<TypeProperty> properties;
        TypeId string_index = store.unknown(), number_index = store.unknown(),symbol_index=store.unknown(), call = store.unknown();
        while (cursor < end) {
            noise(); while (take(";") || take(",")) {}
            if (take("}")) return store.object_of(std::move(properties), string_index, number_index, call,symbol_index);
            noise();
            if (cursor < end && tokens[cursor].text == "[") {
                if (!index(string_index, number_index,symbol_index)) return store.unknown();
                continue;
            }
            if (cursor < end && tokens[cursor].text == "(") {
                call = function(":"); if (call == store.unknown()) return call; continue;
            }
            const bool readonly = take("readonly");
            noise();
            if (cursor >= end || (tokens[cursor].kind != TokenKind::Identifier && tokens[cursor].kind != TokenKind::Keyword && tokens[cursor].kind != TokenKind::String && tokens[cursor].kind != TokenKind::Number)) return store.unknown();
            auto name = tokens[cursor++].text;
            if(name.size()>=2&&(name.front()=='\''||name.front()=='"'))name=name.substr(1,name.size()-2);
            const bool optional = take("?");
            noise();
            TypeId type = store.unknown();
            if (cursor < end && tokens[cursor].text == "(") type = function(":");
            else if (take(":")) type = parse();
            if (type == store.unknown()) return type;
            properties.push_back({std::move(name), type, optional, readonly});
        }
        return store.unknown();
    }
    const std::vector<Token>& tokens; std::size_t& cursor; std::size_t end;
    const TypeStore& store; const std::unordered_map<std::string, TypeId>* named;
};
}

TypeId parse_type_annotation(const std::vector<Token>& tokens, std::size_t& cursor,
                             std::size_t end, const TypeStore& store,
                             const std::unordered_map<std::string, TypeId>* named) {
    return AnnotationParser(tokens, cursor, end, store, named).parse();
}
}
