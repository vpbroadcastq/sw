# sw — command-line stopwatch
sw is a command-line stopwatch utility with the ability to create multiple named stopwatches that persist across executions (named timers are saved in a plain-text config file).

## Usage
### Run an ad-hoc stopwatch
```bash
sw
```
Starts a stopwatch at 0 and updates the display until you stop it (Ctrl+C).  The timer is not saved.

### Run an existing named timer or create a new named timer
```bash
sw my_timer
# or, for a name with spaces,
sw "my timer"
```
If the timer already exists in the config file, `sw` resumes it and displays the time elapsed since it was started. If it does not exist, `sw` creates it and starts it immediately.  If the name has a space it must be enclosed in double quotes.  When you exit `sw` with Ctrl+C, the start time is persisted, so the next run continues from the original start.

### List saved timers
```bash
sw --list-timers
# or,
sw -l
```

### Delete an existing timer
```bash
sw --delete-timer my_timer
# or,
sw -d my_timer
```

## Config file
The config file is a plain-text file with repeating pairs of a bracketed name and a numeric timestamp.  Example:
```
[some name]
153818613518

[some other name]
135168412151
```

- Lines starting with `[` and ending with `]` indicate a timer name.
- The line immediately following a timer name is an implementation-defined value representing the start time of the timer.  Its format and interpretation is not published, subject to change, and values are not intended to be user-editable.
- Config files are not portable across systems.
On linux and other unix-like systems the config file location defaults to `$XDG_CONFIG_HOME/sw.ini` (usually `~/.config/`)
- Any operation that causes the config file to be written to, for example, running with `-d <timer name>` or with a new timer name will cause the entire file to be normalized; invalid entries and unnecessary whitespace will be removed.

## Build
This project uses CMake and requires C++23.  Googletest is required to build the unit tests (built by default, use `-DBUILD_TESTS=OFF` to skip building the tests).  From the project root run:

```bash
cmake -S . -B build
cmake --build ./build
```
This produces the `sw` executable in `build/exe` and a unit test executable in `build/tests`.

## Other things in this repository
The `csharp` and `rust` folders contain partial "experimental" implementations of `sw` in these languages as an exercise in comparing implementations of the same thing in different languages.  They are not really meant to be used.

