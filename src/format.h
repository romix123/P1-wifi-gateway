// Fix error: expected '>' before numeric constant
// esp8266 defines B1 as 1 (binary.h:31)
// The fmt/format library uses B1 as template argument name (fmt/core.h:1118)
// The B1 argument would be replaced with the value 1 by the pre-processor
// To fix this issue: undefine B1 -> include fmt/format -> define B1.

#ifdef B1
#pragma push_macro("B1")
#undef B1

#define REDEFINE_B1
#endif

#include <fmt/format.h>

#ifdef REDEFINE_B1
#pragma pop_macro("B1")
#endif