#pragma once
#include "Type.h"

namespace tscc {
TypeId parse_type_annotation(const std::vector<Token>&, std::size_t&, std::size_t,
                             const TypeStore&,
                             const std::unordered_map<std::string, TypeId>* = nullptr);
}
