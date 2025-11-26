
# sw — command-line stopwatch

sw is a command-line stopwatch utility with the ability to create named stopwatches that persist across executions (named timers are saved in a plain-text config file).

## Config file format
The config file is a plain-text file with repeating pairs of a bracketed name and a numeric timestamp (system_clock milliseconds). Example:

```
[some name]
153818613518

[some other name]
135168412151

```

- Lines starting with `[` and ending with `]` indicate a timer name.
- The following non-empty line should be a signed integer representing milliseconds since the epoch for `std::chrono::system_clock`.


## Build (Linux / macOS)
This project uses CMake. From the project root run:

```bash
cmake -S . -B build
cmake --build build -- -j$(nproc)
```

This produces the `sw` executable in `build/` (or whatever CMake generator/`CMAKE_RUNTIME_OUTPUT_DIRECTORY` is configured).

## Run
If a config file is present (defaults to `$XDG_CONFIG_HOME/sw.ini` or `~/.config/sw.ini`), the program will print that path on startup and use parsed timers.

To run:

```bash
./build/sw
```

