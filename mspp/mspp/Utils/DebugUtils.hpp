#pragma once

#include "mspp/Utils/third_party.hpp"

////////////////
// DebugPrint //
////////////////

#ifdef _DEBUG

#include <string>
#include <algorithm>

inline std::string _DebugPrint_impl_shorten___file___(std::string __file__)
{
	const std::string MSPP_DIR = "mspp/";
	std::replace(__file__.begin(), __file__.end(), '\\', '/');
	return __file__.substr(__file__.rfind(MSPP_DIR) + MSPP_DIR.length());
}

#define DEBUG_SOURCE_LOCATION _DebugPrint_impl_shorten___file___(__FILE__), __LINE__
#define DEBUG_SOURCE_LOCATION_FORMAT "{}@{}"
#define DEBUG_PREFIX DEBUG_SOURCE_LOCATION
#define DEBUG_PREFIX_FORMAT DEBUG_SOURCE_LOCATION_FORMAT ": "

#define DebugPrint(format, ...) fmt::print(DEBUG_PREFIX_FORMAT format "\n", DEBUG_PREFIX, ##__VA_ARGS__)

#else // _DEBUG

#define DebugPrint(format, ...)

#endif // _DEBUG
