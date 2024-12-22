#pragma once
#include <string>
#include <string_view>

namespace ui {
#ifdef _WIN32
    using string_type = std::wstring;
    using string_view_type = std::wstring_view;
    // 字符串字面量宏，用于跨平台字符串
    #define UI_STR(str) L##str
#else
    using string_type = std::string;
    using string_view_type = std::string_view;
    #define UI_STR(str) u8##str
#endif
} 