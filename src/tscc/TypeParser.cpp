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
        auto first = primary();
        if (first == store.unknown()) return first;
        std::vector<TypeId> members{first};
        while (take("|")) {
            auto member = primary();
            if (member == store.unknown()) return member;
            members.push_back(member);
        }
        return store.union_of(std::move(members));
    }

private:
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
        if (token.text == "null") return store.null();
        if (token.text == "undefined") return store.undefined();
        if (named) { auto found = named->find(token.text); if (found != named->end()) return found->second; }
        return store.unknown();
    }
    TypeId primary() {
        noise();
        if (cursor >= end) return store.unknown();
        if (tokens[cursor].text == "{") return object();
        if (tokens[cursor].text == "(") return function("=>");
        return atom(tokens[cursor++]);
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
    bool index(TypeId& string_index, TypeId& number_index) {
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
        else return false;
        return true;
    }
    TypeId object() {
        if (!take("{")) return store.unknown();
        std::vector<TypeProperty> properties;
        TypeId string_index = store.unknown(), number_index = store.unknown(), call = store.unknown();
        while (cursor < end) {
            noise(); while (take(";") || take(",")) {}
            if (take("}")) return store.object_of(std::move(properties), string_index, number_index, call);
            noise();
            if (cursor < end && tokens[cursor].text == "[") {
                if (!index(string_index, number_index)) return store.unknown();
                continue;
            }
            if (cursor < end && tokens[cursor].text == "(") {
                call = function(":"); if (call == store.unknown()) return call; continue;
            }
            const bool readonly = take("readonly");
            noise();
            if (cursor >= end || tokens[cursor].kind != TokenKind::Identifier) return store.unknown();
            auto name = tokens[cursor++].text;
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
