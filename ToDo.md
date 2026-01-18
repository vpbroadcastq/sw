# ToDo's and possible upcoming features

## ToDo
- Usage informaton should report the path to the config file
- The time field in TimerEntry shouldn't be "elapsed" it should be start_time or something
- What does --list-timers print if there are no timers?  Nothing?  Should there be a message?


## Feature ideas
- Run multiple saved timers at once by passsing in more than one name
- Saved timers report the date and time they were started when you do -l
- Saved timers report the date/time they were started when you run one of them
- Spacebar to lap


## Investigate
- Do the C# and C++ impls store the config file in the same place?  C++ creates an sw/ folder but C# does not?
- C#: Always shows full format d:hh:mm:ss:mmm regardless of duration
- The C# version has a potential bug on Windows where the AppData directory should exist, but may fail in edge cases?
- C# is missing error exit codes (the C++ impl returns 1 and prints to stderr on error)

