using System;
using System.Globalization;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

public class Sw
{
    public enum Task
    {
        RunNameless,  // Run a timer without saving to config
        RunNamed,  // Possibly create a new named timer, possibly load existing from config
        ListTimers,  // List all the named timers in the config file and exit
        DeleteNamed,  // Delete a named timer from the config file and exit
    }

    // TODO:  This returns RunNameless in cases where the command line is invalid.  Warn the user instead?
    public static Task DetermineTask(IReadOnlyList<string> programArgs)
    {
        int nargs = programArgs.Count();
        if (nargs == 0) {
            return Task.RunNameless;
        }

        if (nargs == 1)
        {
            if (programArgs[0] == "-l" || programArgs[0] == "--list-timers") {
                return Task.ListTimers;
            } else {
                // The argument is the name of the new or existing timer
                return Task.RunNamed;
            }
        }

        if (nargs == 2) {
            if (programArgs[0] == "-d" || programArgs[0] == "--delete-timer") {
                return Task.DeleteNamed;
            }
        }

        return Task.RunNameless;
    }

    public struct TimerEntry
    {
        public string TimerName;
        // Note that the type is called "Offset" but this represents an "absolute" timestamp corresponding
        // to the time at which the timer was started.
        public DateTimeOffset StartTimeUtc;
    }

    //
    // Returns the full path to the config file (including the file itself), ex
    // /home/ben/.config/sw.ini
    //
    public static string? GetConfigFilePath()
    {
        string? configDir = null;
        if (OperatingSystem.IsWindows())
        {
            configDir = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);
        }
        else
        {
            string? xdgConfigHome = Environment.GetEnvironmentVariable("XDG_CONFIG_HOME");
            if (!string.IsNullOrWhiteSpace(xdgConfigHome))
            {
                configDir = xdgConfigHome;
            }
            else
            {
                string home = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
                if (home.Length != 0)
                {
                    configDir = Path.Combine(home, ".config");
                }
            }
        }

        if (string.IsNullOrWhiteSpace(configDir))
        {
            return null;
        }
        return Path.Combine(configDir, "sw.ini");
    }

    // TODO:  Seperate method to read the file into a string.  This facilitates unit testing.
    public static List<TimerEntry> ReadConfigFile(string path)
    {
        if (!Path.Exists(path))
        {
            return new List<TimerEntry>();
        }
        List<TimerEntry> entries = new List<TimerEntry>();

        string fileContents = File.ReadAllText(path);
        ReadOnlySpan<char> remainingFileContent = fileContents.AsSpan();
        ReadOnlySpan<char> currTimerName = ReadOnlySpan<char>.Empty;
        while (!remainingFileContent.IsEmpty)
        {
            int newLineIndex = remainingFileContent.IndexOf('\n');
            if (newLineIndex == -1)
            {
                // On the last line of the file
                newLineIndex = remainingFileContent.Length;
            }
            ReadOnlySpan<char> currLine = remainingFileContent[..newLineIndex];
            // Update remainingFileContent before processing currentLine.  This is done here so that 'continue'
            // statements below don't skip this step.
            remainingFileContent = newLineIndex == remainingFileContent.Length
                ? ReadOnlySpan<char>.Empty
                : remainingFileContent[(newLineIndex + 1)..];

            // Process currLine
            ReadOnlySpan<char> currLineTrimmed = currLine.Trim();
            if (currLineTrimmed.Length == 0)
            {
                continue;
            }
            if (currLineTrimmed.Length > 2 && currLineTrimmed.StartsWith("[") && currLineTrimmed.EndsWith("]"))
            {
                currTimerName = currLineTrimmed.Slice(1, currLineTrimmed.Length - 2).ToString();
                continue;
            }

            if (!currTimerName.IsEmpty)
            {
                if (DateTimeOffset.TryParseExact(currLineTrimmed, "O", CultureInfo.InvariantCulture, DateTimeStyles.RoundtripKind, out DateTimeOffset startedUtc))
                {
                    entries.Add(new TimerEntry
                    {
                        TimerName = currTimerName.ToString(),
                        StartTimeUtc = startedUtc
                    });
                    currTimerName = ReadOnlySpan<char>.Empty;
                    continue;
                }
            }
        }

        return entries;
    }

    //
    // Returns the start time for the given timer name, or null if not found.
    //
    public static DateTimeOffset? StartTimeIfPresent(string timerName, IEnumerable<TimerEntry> entries)
    {
        foreach (TimerEntry entry in entries)
        {
            if (entry.TimerName.Equals(timerName, StringComparison.Ordinal))
            {
                return entry.StartTimeUtc;
            }
        }
        return null;
    }


    //
    // Writes the given timer entries to the config file at the given path.  Overwrites the existing file.
    //
    public static bool WriteConfigFile(IEnumerable<TimerEntry> entries, string path)
    {
        try
        {
            using (StreamWriter sw = new StreamWriter(path, false))
            {
                foreach (TimerEntry entry in entries)
                {
                    sw.WriteLine($"\n[{entry.TimerName}]");
                    sw.WriteLine($"{entry.StartTimeUtc.ToString("O", CultureInfo.InvariantCulture)}\n");
                }
            }
            return true;
        }
        catch
        {
            return false;
        }
    }
}



