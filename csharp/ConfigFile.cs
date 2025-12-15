using System;
using System.Globalization;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

class ConfigFile
{
    public struct TimerEntry
    {
        public string TimerName;
        // Note that the type is called "Offset" but this represents an "absolute" timestamp corresponding
        // to the time at which the timer was started.
        public DateTimeOffset StartTimeUtc;
    }

    //
    // Returns the path to the config file.
    //
    // TODO:  Returnd string needs to be  nullable, ie, string?
    public static string GetConfigFilePath()
    {
        string configDir;
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
                configDir = Path.Combine(home, ".config");
            }
        }

        return Path.Combine(configDir, "sw.ini");
    }

    //
    // Reads the config file at the given path, returning the list of timer entries found.  Bogus entries are ignored.
    //
    // TODO:  Better error handling
    // TODO:  It's wasteful to pull the file into an array of strings
    public static IEnumerable<TimerEntry> ReadConfigFile(string path)
    {
        if (!Path.Exists(path))
        {
            return new List<TimerEntry>();
        }
        List<TimerEntry> entries = new List<TimerEntry>();

        string[] fileContents = File.ReadAllLines(path);
        string currentTimerName = string.Empty;
        foreach (string line in fileContents)
        {
            ReadOnlySpan<char> trimmedLine = line.AsSpan().Trim();
            if (trimmedLine.Length == 0)
            {
                continue;
            }

            if (trimmedLine.StartsWith("[") && trimmedLine.EndsWith("]") && trimmedLine.Length > 2)
            {
                currentTimerName = trimmedLine.Slice(1, trimmedLine.Length - 2).ToString();
                continue;
            }

            if (currentTimerName.Length != 0)
            {
                if (DateTimeOffset.TryParseExact(trimmedLine, "O", CultureInfo.InvariantCulture, DateTimeStyles.RoundtripKind, out DateTimeOffset startedUtc))
                {
                    entries.Add(new TimerEntry
                    {
                        TimerName = currentTimerName,
                        StartTimeUtc = startedUtc
                    });
                    currentTimerName = string.Empty;
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
    // Appends the given timer entry to the config file at the given path.
    //
    // TODO:  What if the file does not exist
    // TODO:  Check for existing entries w/ the same name?
    public static bool WriteTimerToConfigFile(string path, TimerEntry entry)
    {
        try
        {
            using (StreamWriter sw = File.AppendText(path))
            {
                sw.WriteLine($"\n[{entry.TimerName}]");
                sw.WriteLine($"{entry.StartTimeUtc.ToString("O", CultureInfo.InvariantCulture)}\n");
            }
            return true;
        }
        catch
        {
            return false;
        }
    }
}



