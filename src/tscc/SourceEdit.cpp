#include "SourceEdit.h"
#include <algorithm>

namespace tscc {
namespace {
struct OrderedReplacement {
    Replacement edit;
    std::size_t order = 0;
};

bool conflicts(const Replacement& a, const Replacement& b) {
    const bool a_insert = a.begin == a.end;
    const bool b_insert = b.begin == b.end;
    if (a_insert && b_insert) return false;
    if (a_insert) return a.begin > b.begin && a.begin < b.end;
    if (b_insert) return b.begin > a.begin && b.begin < a.end;
    return a.begin < b.end && b.begin < a.end;
}
}

bool apply_replacements(const SourceFile& source, std::string& out,
                        const std::vector<Replacement>& replacements,
                        Diagnostics& diagnostics) {
    std::vector<OrderedReplacement> edits;
    edits.reserve(replacements.size());
    for (std::size_t i = 0; i < replacements.size(); ++i) {
        const auto& edit = replacements[i];
        if (edit.begin > edit.end || edit.end > source.text.size()) {
            diagnostics.error(source.path, 1, 1, "invalid source edit range");
            return false;
        }
        edits.push_back({edit, i});
    }

    for (std::size_t i = 0; i < edits.size(); ++i) {
        for (std::size_t j = i + 1; j < edits.size(); ++j) {
            if (!conflicts(edits[i].edit, edits[j].edit)) continue;
            const auto offset = std::max(edits[i].edit.begin, edits[j].edit.begin);
            const auto [line, column] = source.line_col(offset);
            diagnostics.error(source.path, line, column,
                              "conflicting compiler source edits", source.line_text(line));
            return false;
        }
    }

    std::sort(edits.begin(), edits.end(), [](const auto& a, const auto& b) {
        if (a.edit.begin != b.edit.begin) return a.edit.begin > b.edit.begin;
        if (a.edit.end != b.edit.end) return a.edit.end > b.edit.end;
        // Apply later same-position insertions first so repeated string insertion
        // leaves the producer sequence in source order.
        return a.order > b.order;
    });
    for (const auto& ordered : edits) {
        const auto& edit = ordered.edit;
        out.replace(edit.begin, edit.end - edit.begin, edit.text);
    }
    return true;
}

} // namespace tscc
