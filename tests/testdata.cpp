#include "testdata.h"
#include <swlib/sw.h>
#include <array>
#include <string_view>
#include <chrono>

namespace testdata {

//
// All entries are invalid
//
std::string_view all_invalid{R"(
] BackwardsBrackets [
1765721157118

NoBrackets
1765721157118

[ MissingRightBracket
1765721157118

MissingLeftBracket]
1765721157118

[MissingTimestamp]


)"};


//
// All entries are valid
//
std::string_view all_valid{R"([whatever]
1765721157118

[name with spaces]
1765721157118


[Name~with " special' ;chars!@#$%^&]
1765721157118

)"};

std::array<timer_entry,3> all_valid_entries{{
    {.timer_name="whatever", .start_time=std::chrono::system_clock::time_point(std::chrono::milliseconds(1765721157118))},
    {.timer_name="name with spaces", .start_time=std::chrono::system_clock::time_point(std::chrono::milliseconds(1765721157118))},
    {.timer_name="Name~with \" special' ;chars!@#$%^&", .start_time=std::chrono::system_clock::time_point(std::chrono::milliseconds(1765721157118))}
}};

std::string_view all_valid_reencoded{R"([whatever]
1765721157118

[name with spaces]
1765721157118

[Name~with " special' ;chars!@#$%^&]
1765721157118

)"};


//
// Mixed valid and invalid entries
//
std::string_view mixed_valid_invalid{R"([whatever]
1765721157118

[]
1765721157118

[name with spaces]
1765721157118

MissingLeftBracket]
1765721157118

1765721157118

[Name~with " special' ;chars!@#$%^&]
1765721157118

[   name with leading&trailing spaces  ]
1765721157118

)"};

std::array<timer_entry,4> mixed_valid_invalid_entries{{
    {.timer_name="whatever", .start_time=std::chrono::system_clock::time_point(std::chrono::milliseconds(1765721157118))},
    {.timer_name="name with spaces", .start_time=std::chrono::system_clock::time_point(std::chrono::milliseconds(1765721157118))},
    {.timer_name="Name~with \" special' ;chars!@#$%^&", .start_time=std::chrono::system_clock::time_point(std::chrono::milliseconds(1765721157118))},
    {.timer_name="   name with leading&trailing spaces  ", .start_time=std::chrono::system_clock::time_point(std::chrono::milliseconds(1765721157118))}
}};

std::string_view mixed_valid_invalid_reencoded{R"([whatever]
1765721157118

[name with spaces]
1765721157118

[Name~with " special' ;chars!@#$%^&]
1765721157118

[   name with leading&trailing spaces  ]
1765721157118

)"};

} // namespace testdata
