#include "tscc/Binder.h"
#include "tscc/Diagnostic.h"
#include "tscc/Lexer.h"
#include "tscc/Parser.h"
#include "tscc/Semantic.h"
#include "tscc/Source.h"
#include "tscc/Type.h"
#include <cstdlib>
#include <iostream>

using namespace tscc;
static void fail(const char* message) { std::cerr << message << '\n'; std::exit(1); }

int main() {
    SourceFile source;
    source.path = "types.ts";
    source.text = "interface Point {readonly x:number} interface Point {label?:string} type Box={value:Point};type Mapper=(value:string)=>number;const mapper:Mapper=(value)=>1;const point:Point={x:1};const box:Box={value:{x:1}};const n:number=1;const s:string='x';let b:boolean=true;const complex:number|string=1;function add(left:number,right:number):number{return left+right;}";
    source.line_starts = {0};
    Diagnostics diagnostics;
    Lexer lexer(source, diagnostics);
    const auto tokens = lexer.lex();
    Program program;
    Parser parser(source, tokens, diagnostics);
    if (!parser.parse(program)) fail("type-model fixture did not parse");
    const auto semantic = build_semantic_model(tokens, program);
    const auto binding = bind_semantic_model(tokens, program, semantic);
    const auto types = build_type_model(tokens, program, semantic, binding);
    const auto point=types.store.object_of({{"x",types.store.number(),false,false},{"label",types.store.string(),true,true}});if(types.store.kind(point)!=TypeKind::Object||!types.store.property(point,"x")||!types.store.property(point,"label")->readonly)fail("canonical object shape missing");if(types.store.object_of({{"label",types.store.string(),true,true},{"x",types.store.number(),false,false}})!=point)fail("object shape was not canonical");
    const auto named_point=types.named_types.find("Point"),box=types.named_types.find("Box");if(named_point==types.named_types.end()||!types.store.property(named_point->second,"x")||!types.store.property(named_point->second,"label"))fail("merged interface shape missing");if(box==types.named_types.end()||!types.store.property(box->second,"value"))fail("object alias shape missing");
    const auto mapper=types.named_types.find("Mapper");if(mapper==types.named_types.end()||!types.store.callable(mapper->second)||types.store.callable(mapper->second)->parameters[0]!=types.store.string()||types.store.callable(mapper->second)->result!=types.store.number())fail("callable alias missing");if(types.store.function_of({types.store.string()},types.store.number(),1,false)!=mapper->second)fail("callable identity was not canonical");
    const auto mutable_array=types.store.array_of(types.store.number());
    const auto readonly_array=types.store.array_of(types.store.number(),true);
    if(mutable_array==readonly_array||!types.store.readonly_collection(readonly_array))fail("readonly array identity missing");
    if(!types.store.assignable(mutable_array,readonly_array)||types.store.assignable(readonly_array,mutable_array))fail("readonly collection variance is wrong");
    const auto tuple=types.store.tuple_of({types.store.string(),types.store.number(),types.store.boolean()},{TupleRequired,TupleOptional,TupleRest},true);
    if(types.store.tuple_flags(tuple).size()!=3||!(types.store.tuple_flags(tuple)[1]&TupleOptional)||!(types.store.tuple_flags(tuple)[2]&TupleRest))fail("tuple element metadata missing");
    const auto indexed=types.store.object_of({{"name",types.store.string(),false,false},{"7",types.store.boolean(),false,false}},types.store.unknown(),types.store.unknown(),types.store.unknown(),types.store.number());
    if(types.store.indexed_access(indexed,types.store.literal(types.store.string(),"name"))!=types.store.string())fail("literal indexed access missing");
    if(types.store.symbol_index(indexed)!=types.store.number())fail("symbol index signature missing");
    const auto keys=types.store.keyof_type(indexed);
    if(!types.store.assignable(types.store.literal(types.store.string(),"name"),keys)||!types.store.assignable(types.store.literal(types.store.number(),"7"),keys)||!types.store.assignable(types.store.symbol(),keys))fail("keyof key domains missing");
    const auto intersection=types.store.intersection_of({types.store.object_of({{"a",types.store.number(),false,false}}),types.store.object_of({{"b",types.store.string(),false,false}})});
    if(!types.store.property(intersection,"a")||!types.store.property(intersection,"b"))fail("canonical intersection merge missing");
    if(types.store.intersection_of({types.store.string(),types.store.number()})!=types.store.never())fail("disjoint intersection did not reduce to never");
    const auto conditional=types.store.conditional_of(types.store.string(),types.store.string(),types.store.number(),types.store.boolean());
    if(conditional!=types.store.number())fail("concrete conditional type did not reduce");
    std::unordered_map<std::string,TypeId>template_names{{"Suffix",types.store.union_of({types.store.literal(types.store.string(),"A"),types.store.literal(types.store.string(),"B")})}};
    const auto template_type=types.store.template_literal("`get${Suffix}`",&template_names);
    if(!types.store.assignable(types.store.literal(types.store.string(),"getA"),template_type)||types.store.assignable(types.store.literal(types.store.string(),"getC"),template_type))fail("template literal expansion missing");
    if (types.symbol_types.size() != binding.symbols.size()) fail("type facts lost symbol identity");
    for (std::size_t i = 0; i < binding.symbols.size(); ++i) {
        const auto& name = binding.symbols[i].name;
        const auto kind = types.store.kind(types.symbol_types[i]);
        if (name == "n" && kind != TypeKind::Number) fail("number fact missing");
        if (name == "s" && kind != TypeKind::String) fail("string fact missing");
        if (name == "b" && kind != TypeKind::Boolean) fail("boolean fact missing");
        if (name == "complex" && kind != TypeKind::Union) fail("union type fact missing");
        if (name == "mapper" && types.symbol_types[i] != mapper->second) fail("callable variable fact missing");
        if (name == "add") {
            if (kind != TypeKind::Function) fail("function type fact missing");
            const auto& signature = types.function_signatures[i];
            if (!signature.valid || signature.parameters.size() != 2 ||
                signature.parameters[0] != types.store.number() ||
                signature.parameters[1] != types.store.number() ||
                signature.result != types.store.number())
                fail("function signature fact missing");
        }
    }
    std::cout << "tscc durable type-model smoke test passed\n";
}
