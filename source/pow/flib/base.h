#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <exception>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <functional>

namespace flib {

typedef __int8 s8;
typedef __int16 s16;
typedef __int32 s32;
typedef __int64 s64;

typedef unsigned __int8 u8;
typedef unsigned __int16 u16;
typedef unsigned __int32 u32;
typedef unsigned __int64 u64;

class todo_throw_proper_exception : std::exception {};
class todo_display_proper_error : std::exception {};

} // namespace flib

namespace fsample { using namespace flib; }
