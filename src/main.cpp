#include "tscc/Compiler.h"
#include "tscc/Config.h"
#include "tscc/Diagnostic.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

static void help() {
    std::cout
        << "tscc - TypeScript to JavaScript compiler in C++\n\n"
        << "Usage: tscc [options] file.ts ...\n"
        << "       tscc -p tsconfig.json\n\n"
        << "Options:\n"
        << "  --help, -h                 Show help\n"
        << "  --version, -v              Show version\n"
        << "  --project, -p <tsconfig>   Compile a tsconfig.json project\n"
        << "  --outDir <dir>             Redirect emitted JavaScript\n"
        << "  --rootDir <dir>            Preserve source paths relative to this directory\n"
        << "  --module <name>            Module mode (preserve/esnext or initial commonjs transform)\n"
        << "  --jsx <mode>               JSX mode (preserve currently supported)\n"
        << "  --target <name>            Target label (currently es2022-compatible erasure)\n"
        << "  --removeComments           Remove comments\n"
        << "  --noEmit                   Parse/transpile without writing files\n"
        << "  --noEmitOnError            Commit no outputs when any input has errors\n"
        << "  --noResolve                Do not follow relative module dependencies\n"
        << "  --pretty / --pretty false  Enable/disable ANSI diagnostics\n";
}

int main(int argc, char** argv) {
    tscc::CompilerOptions options;
    std::vector<std::string> files;
    std::string project;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") { help(); return 0; }
        if (arg == "--version" || arg == "-v") { std::cout << "tscc 0.15.0\n"; return 0; }
        if ((arg == "--project" || arg == "-p") && i + 1 < argc) { project = argv[++i]; continue; }
        if (arg == "--outDir" && i + 1 < argc) { options.out_dir = argv[++i]; continue; }
        if (arg == "--rootDir" && i + 1 < argc) { options.root_dir = argv[++i]; continue; }
        if (arg == "--module" && i + 1 < argc) { options.module = argv[++i]; continue; }
        if (arg == "--jsx" && i + 1 < argc) { options.jsx = argv[++i]; continue; }
        if (arg == "--target" && i + 1 < argc) { options.target = argv[++i]; continue; }
        if (arg == "--removeComments") { options.remove_comments = true; continue; }
        if (arg == "--noEmit") { options.no_emit = true; continue; }
        if (arg == "--noEmitOnError") { options.no_emit_on_error = true; continue; }
        if (arg == "--noResolve") { options.follow_imports = false; continue; }
        if (arg == "--pretty" && i + 1 < argc && std::string(argv[i + 1]) == "false") {
            options.pretty = false; ++i; continue;
        }
        if (arg == "--pretty") { options.pretty = true; continue; }
        if (!arg.empty() && arg[0] == '-') {
            std::cerr << "tscc: error: unknown option '" << arg << "'\n";
            return 2;
        }
        files.push_back(arg);
    }

    if (!project.empty()) {
        if (!files.empty()) {
            std::cerr << "tscc: error: cannot mix --project with source files\n";
            return 2;
        }
        tscc::Diagnostics diagnostics;
        if (!tscc::load_tsconfig(project, options, files, diagnostics)) {
            diagnostics.print(options.pretty);
            return 2;
        }
    }

    if (files.empty()) { help(); return 0; }
    return tscc::compile_files(files, options);
}
