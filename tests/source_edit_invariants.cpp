#include "tscc/Diagnostic.h"
#include "tscc/Source.h"
#include "tscc/SourceEdit.h"
#include <cstdlib>
#include <iostream>

using namespace tscc;

static void fail(const char* message) { std::cerr << message << '\n'; std::exit(1); }

static SourceFile fixture() {
    SourceFile source;
    source.path = "edits.ts";
    source.text = "abcdef\n";
    source.line_starts = {0, 7};
    return source;
}

int main() {
    const auto source = fixture();
    {
        Diagnostics diagnostics;
        std::string out = source.text;
        const std::vector<Replacement> edits = {{1, 3, "B"}, {3, 5, "D"}};
        if (!apply_replacements(source, out, edits, diagnostics) || out != "aBDf\n")
            fail("adjacent replacements were not deterministic");
    }
    {
        Diagnostics diagnostics;
        std::string out = source.text;
        const std::vector<Replacement> edits = {{3, 3, "first"}, {3, 3, "second"}};
        if (!apply_replacements(source, out, edits, diagnostics) ||
            out != "abcfirstseconddef\n")
            fail("same-position insertion order was not preserved");
    }
    {
        Diagnostics diagnostics;
        std::string out = source.text;
        const std::vector<Replacement> edits = {{1, 4, "x"}, {3, 5, "y"}};
        if (apply_replacements(source, out, edits, diagnostics) || !diagnostics.has_errors())
            fail("overlapping replacements were not rejected");
    }
    {
        Diagnostics diagnostics;
        std::string out = source.text;
        const std::vector<Replacement> edits = {{1, 5, "x"}, {3, 3, "inside"}};
        if (apply_replacements(source, out, edits, diagnostics) || !diagnostics.has_errors())
            fail("insertion inside a replacement was not rejected");
    }
    std::cout << "tscc source-edit invariant test passed\n";
}
