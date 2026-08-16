#pragma once
#include "Diagnostic.h"
#include "Source.h"
#include "Syntax.h"
#include <string>
#include <vector>

namespace tscc {

// Applies replacements expressed in original-source offsets. Non-empty edits
// may be adjacent but never overlap; insertions may share a boundary and retain
// producer order in the emitted text. Conflicts are diagnosed instead of being
// resolved accidentally by container or traversal order.
bool apply_replacements(const SourceFile&, std::string&, const std::vector<Replacement>&,
                        Diagnostics&);

} // namespace tscc
