#include "Binder.h"
#include <algorithm>
#include <utility>
#include <unordered_set>

namespace tscc {
namespace {
constexpr std::size_t missing = static_cast<std::size_t>(-1);

std::size_t containing_scope(const BindingModel& binding, std::size_t token) {
    std::size_t best = 0;
    for (std::size_t i = 1; i < binding.scopes.size(); ++i) {
        const auto& scope = binding.scopes[i];
        if (token > scope.begin_token && token < scope.end_token &&
            (best == 0 || scope.end_token - scope.begin_token <
                              binding.scopes[best].end_token - binding.scopes[best].begin_token))
            best = i;
    }
    return best;
}

std::size_t resolve(const BindingModel& binding, std::size_t scope, const std::string& name) {
    for (;;) {
        for (std::size_t i = 0; i < binding.symbols.size(); ++i)
            if (binding.symbols[i].scope == scope && binding.symbols[i].name == name) return i;
        if (scope == 0 || binding.scopes[scope].parent == missing) return missing;
        scope = binding.scopes[scope].parent;
    }
}

}

std::size_t BindingModel::symbol_for_reference(std::size_t token) const {
    for (const auto& reference : references)
        if (reference.token == token) return reference.symbol;
    return missing;
}

bool BindingModel::is_declaration_token(std::size_t token) const {
    for (const auto& symbol : symbols)
        if (symbol.declaration_token == token) return true;
    return false;
}

bool BindingModel::is_import_token(std::size_t token) const {
    for (const auto& range : import_ranges)
        if (token >= range.first && token <= range.second) return true;
    return false;
}

BindingModel bind_semantic_model(const std::vector<Token>& tokens, const Program& program,
                                 const SemanticModel& semantic) {
    BindingModel binding;
    binding.scopes.push_back({0, tokens.size(), missing, true});

    std::vector<std::size_t> brace_nodes;
    for (std::size_t i = 0; i < semantic.nodes.size(); ++i)
        if (semantic.nodes[i].kind == SemanticNodeKind::BraceRegion ||
            semantic.nodes[i].kind == SemanticNodeKind::LexicalRegion) brace_nodes.push_back(i);
    std::sort(brace_nodes.begin(), brace_nodes.end(), [&](auto a, auto b) {
        if (semantic.nodes[a].begin_token != semantic.nodes[b].begin_token)
            return semantic.nodes[a].begin_token < semantic.nodes[b].begin_token;
        return semantic.nodes[a].end_token > semantic.nodes[b].end_token;
    });
    for (auto node_index : brace_nodes) {
        const auto& node = semantic.nodes[node_index];
        std::size_t parent = 0;
        for (std::size_t i = 1; i < binding.scopes.size(); ++i)
            if (node.begin_token > binding.scopes[i].begin_token &&
                node.end_token <= binding.scopes[i].end_token &&
                (parent == 0 || binding.scopes[i].end_token - binding.scopes[i].begin_token <
                                    binding.scopes[parent].end_token - binding.scopes[parent].begin_token))
                parent = i;
        binding.scopes.push_back({node.begin_token, node.end_token, parent, false});
    }

    for (const auto& function : semantic.nodes) {
        if (function.kind != SemanticNodeKind::FunctionDeclaration &&
            function.kind != SemanticNodeKind::ArrowFunction) continue;
        std::size_t body_scope = missing;
        for (std::size_t i = 1; i < binding.scopes.size(); ++i)
            if (binding.scopes[i].begin_token == function.scope_token) body_scope = i;
        if (body_scope != missing) binding.scopes[body_scope].function_scope = true;
        if (function.name_token != missing && function.name_token < tokens.size())
            binding.symbols.push_back({tokens[function.name_token].text, SymbolKind::Function,
                                       function.name_token, containing_scope(binding, function.begin_token),
                                       static_cast<std::size_t>(&function - semantic.nodes.data())});
    }

    for (std::size_t node_index = 0; node_index < semantic.nodes.size(); ++node_index) {
        const auto& node = semantic.nodes[node_index];
        if (node.kind != SemanticNodeKind::ClassDeclaration || node.name_token >= tokens.size()) continue;
        binding.symbols.push_back({tokens[node.name_token].text, SymbolKind::Class,
                                   node.name_token, containing_scope(binding,node.begin_token),node_index});
    }

    std::unordered_set<std::size_t> declaration_tokens;
    for (std::size_t node_index = 0; node_index < semantic.nodes.size(); ++node_index) {
        const auto& node = semantic.nodes[node_index];
        if (node.kind != SemanticNodeKind::VariableDeclaration &&
            node.kind != SemanticNodeKind::BindingDeclaration &&
            node.kind != SemanticNodeKind::ParameterDeclaration &&
            node.kind != SemanticNodeKind::CatchDeclaration) continue;
        if (node.name_token == missing || node.name_token >= tokens.size()) continue;
        auto scope = containing_scope(binding, node.name_token);
        SymbolKind kind = (node.kind == SemanticNodeKind::ParameterDeclaration ||
                           node.kind == SemanticNodeKind::CatchDeclaration)
                              ? SymbolKind::Parameter : SymbolKind::Variable;
        VariableKind variable_kind = VariableKind::None;
        if (node.kind == SemanticNodeKind::CatchDeclaration) {
            for (std::size_t i = 1; i < binding.scopes.size(); ++i)
                if (binding.scopes[i].begin_token == node.scope_token &&
                    binding.scopes[i].end_token == node.scope_end_token) { scope = i; break; }
        } else if (kind == SymbolKind::Parameter) {
            for (std::size_t i = 1; i < binding.scopes.size(); ++i)
                if (binding.scopes[i].begin_token == node.scope_token) { scope = i; break; }
        } else if (node.variable_index < program.variables.size()) {
            std::size_t keyword = node.name_token;
            while (keyword > 0 && tokens[keyword].text != "var" && tokens[keyword].text != "let" &&
                   tokens[keyword].text != "const" && tokens[keyword].text != ";" &&
                   tokens[keyword].text != "{") --keyword;
            if (tokens[keyword].text == "var") variable_kind = VariableKind::Var;
            else if (tokens[keyword].text == "let") variable_kind = VariableKind::Let;
            else if (tokens[keyword].text == "const") variable_kind = VariableKind::Const;
            if (tokens[keyword].text == "var")
                while (scope != 0 && !binding.scopes[scope].function_scope)
                    scope = binding.scopes[scope].parent;
        }
        binding.symbols.push_back({tokens[node.name_token].text, kind, node.name_token,
                                   scope, node_index, variable_kind});
        declaration_tokens.insert(node.name_token);
    }
    for (const auto& symbol : binding.symbols) declaration_tokens.insert(symbol.declaration_token);

    // Static value imports are root-scope declarations. Keep the supported slice
    // deliberately syntactic here: default, namespace and named bindings. Type-only
    // imports do not introduce runtime symbols.
    for (std::size_t i = 0; i + 1 < tokens.size(); ++i) {
        if (tokens[i].text != "import" || tokens[i+1].text == "(") continue;
        std::size_t module = i + 1;
        while (module < tokens.size() && tokens[module].kind != TokenKind::String &&
               tokens[module].text != ";" && tokens[module].kind != TokenKind::End) ++module;
        if (module >= tokens.size() || tokens[module].kind != TokenKind::String) continue;
        std::size_t end = module, after = module + 1;
        while (after < tokens.size() && tokens[after].kind == TokenKind::Comment) ++after;
        if (after < tokens.size() && (tokens[after].text == "with" ||
                                      tokens[after].text == "assert")) {
            ++after;
            while (after < tokens.size() && tokens[after].kind == TokenKind::Comment) ++after;
            if (after < tokens.size() && tokens[after].text == "{") {
                int depth = 0;
                for (; after < tokens.size(); ++after) {
                    if (tokens[after].text == "{") ++depth;
                    else if (tokens[after].text == "}" && --depth == 0) { end = after; break; }
                }
            }
        }
        after = end + 1;
        while (after < tokens.size() && tokens[after].kind == TokenKind::Comment) ++after;
        if (after < tokens.size() && tokens[after].text == ";") end = after;
        binding.import_ranges.push_back({i, end});
        if (tokens[i+1].kind == TokenKind::String) { i = end; continue; }
        std::size_t from = i + 1;
        while (from < module && tokens[from].text != "from") ++from;
        if (from == module) { i = end; continue; }
        auto add_import = [&](std::size_t token) {
            if (token >= from || tokens[token].kind != TokenKind::Identifier) return;
            binding.symbols.push_back({tokens[token].text, SymbolKind::Import, token, 0});
            declaration_tokens.insert(token);
        };
        std::size_t p = i + 1;
        const bool clause_type_only = p < from && tokens[p].text == "type";
        if (clause_type_only) ++p;
        if (p < from && tokens[p].kind == TokenKind::Identifier) {
            if (!clause_type_only) add_import(p);
            ++p;
            if (p < from && tokens[p].text == ",") ++p;
        }
        if (p + 2 < from && tokens[p].text == "*" && tokens[p+1].text == "as") {
            if (!clause_type_only) add_import(p + 2);
        } else if (p < from && tokens[p].text == "{") {
            ++p;
            while (p < from && tokens[p].text != "}") {
                if (tokens[p].text == ",") { ++p; continue; }
                if (tokens[p].text == "type") {
                    ++p;
                    while (p < from && tokens[p].text != "," && tokens[p].text != "}") ++p;
                    continue;
                }
                if (tokens[p].kind != TokenKind::Identifier && tokens[p].text != "default") {
                    ++p; continue;
                }
                const auto imported = p++;
                if (p + 1 < from && tokens[p].text == "as") {
                    if (!clause_type_only) add_import(p + 1);
                    p += 2;
                } else if (!clause_type_only) add_import(imported);
                while (p < from && tokens[p].text != "," && tokens[p].text != "}") ++p;
            }
        }
        i = end;
    }

    auto in_type = [&](std::size_t token) {
        for (const auto& variable : program.variables)
            if (token >= variable.type_begin_token && token < variable.type_end_token) return true;
        return false;
    };
    for (std::size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].kind != TokenKind::Identifier || declaration_tokens.count(i) || in_type(i)) continue;
        bool in_import = false;
        for (const auto& range : binding.import_ranges)
            if (i >= range.first && i <= range.second) { in_import = true; break; }
        if (in_import) continue;
        if (i > 0 && (tokens[i-1].text == "." || tokens[i-1].text == "?." ||
                      tokens[i-1].text == "#")) continue;
        if (i + 1 < tokens.size() && tokens[i+1].text == ":") continue;
        const auto scope = containing_scope(binding, i);
        binding.references.push_back({i, scope, resolve(binding, scope, tokens[i].text)});
    }
    return binding;
}

} // namespace tscc
