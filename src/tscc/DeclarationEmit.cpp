#include "DeclarationEmit.h"
namespace tscc { namespace {
std::string slice(const CompilationUnit&u,std::size_t first,std::size_t last){if(first>=u.tokens.size()||last>=u.tokens.size()||first>last)return {};return u.source.text.substr(u.tokens[first].begin,u.tokens[last].end-u.tokens[first].begin);}
std::string prefix(const CompilationUnit&u,const SyntaxNode&node){if(node.begin_token>0&&u.tokens[node.begin_token-1].text=="export")return "export ";return {};}
}
std::string emit_declarations(const CompilationUnit&u){std::string out;for(const auto&node:u.program.root.children){if(node.kind==SyntaxKind::InterfaceDeclaration||node.kind==SyntaxKind::TypeAliasDeclaration){out+=prefix(u,node)+slice(u,node.begin_token,node.end_token)+"\n";continue;}if(node.kind==SyntaxKind::EnumDeclaration){out+=prefix(u,node)+"declare "+slice(u,node.begin_token,node.end_token)+"\n";continue;}if(node.kind==SyntaxKind::FunctionDeclaration){auto open=node.begin_token;while(open<=node.end_token&&u.tokens[open].text!="{")++open;auto end=open<=node.end_token?open-1:node.end_token;out+=prefix(u,node)+"declare "+slice(u,node.begin_token,end)+";\n";}}
 for(const auto&v:u.program.variables){if(v.type_begin_token>=v.type_end_token)continue;std::size_t kind=v.name_token;while(kind>0&&u.tokens[kind].text!="const"&&u.tokens[kind].text!="let"&&u.tokens[kind].text!="var"&&u.tokens[kind].text!=";")--kind;if(u.tokens[kind].text==";")continue;bool exported=kind>0&&u.tokens[kind-1].text=="export";out+=(exported?"export ":"")+std::string("declare ")+u.tokens[kind].text+" "+u.tokens[v.name_token].text+": "+slice(u,v.type_begin_token,v.type_end_token-1)+";\n";}
 return out;}
}
