#pragma once

namespace tscc {

enum class PrimitiveType { Unsupported, Number, String, Boolean, BigInt };

const char* type_name(PrimitiveType);

} // namespace tscc
