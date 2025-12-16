#include <swlib/sw.h>
#include <array>
#include <string_view>

namespace testdata {

extern std::string_view all_invalid;

// All entries are valid
// The _reencoded file differs from the input because the input has some extra newlines
extern std::string_view all_valid;
extern std::array<sw::timer_entry,3> all_valid_entries;
extern std::string_view all_valid_reencoded;


extern std::string_view mixed_valid_invalid;
extern std::array<sw::timer_entry,4> mixed_valid_invalid_entries;
extern std::string_view mixed_valid_invalid_reencoded;

} // namespace testdata
