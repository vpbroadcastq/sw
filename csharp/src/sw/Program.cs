using System.Globalization;
using System.Diagnostics;

class Program
{
    // TODO:  Error handling for bad command lines.  For example, if you run with "dotnet run timer 2" it
    // runs in nameless mode but the user probably wanted a new timer named "timer 2" but forgot the quotes.
    static void Main(string[] args)
    {
        DateTimeOffset programStartTime = DateTimeOffset.UtcNow;
        Stopwatch sw = new Stopwatch();
        sw.Start();

        Sw.Task task = Sw.DetermineTask(args);

        TimeSpan elapsedSavedTimer = TimeSpan.Zero;
        string? configFilePath = Sw.GetConfigFilePath();
        if (configFilePath != null)
        {
            List<Sw.TimerEntry> entries = new List<Sw.TimerEntry>();
            {
                string configFileData = Sw.ReadFile(configFilePath);
                if (!string.IsNullOrEmpty(configFileData))
                {
                    entries = Sw.DecodeConfigFile(configFileData);
                }
            }
            
            if (task == Sw.Task.ListTimers)
            {
                foreach(Sw.TimerEntry entry in entries) 
                {
                    Console.WriteLine($"[{entry.TimerName}]\n{entry.StartTimeUtc.ToString("u", CultureInfo.InvariantCulture)}");
                }
                return;
            }
            else if (task == Sw.Task.DeleteNamed)
            {
                // args[0] is the '-d' or the '--delete-timer' flag
                int idx = entries.FindIndex(e => e.TimerName == args[1]);
                if (idx >= 0)
                {
                    entries.RemoveAt(idx);
                    string newConfigFileData = Sw.EncodeConfigFile(entries);
                    Sw.WriteFile(configFilePath, newConfigFileData);
                }
                else
                {
                    Console.WriteLine($"No timer named '{args[1]}' found.");
                }
                return;
            }
            else if (task == Sw.Task.RunNamed)
            {
                int idx = entries.FindIndex(e => e.TimerName == args[0]);
                if (idx >= 0)
                {
                    // Resuming an existing named timer
                    elapsedSavedTimer = programStartTime - entries[idx].StartTimeUtc;
                }
                else
                {
                    // Adding a new named timer
                    entries.Add(new Sw.TimerEntry
                    {
                        TimerName = args[0],
                        StartTimeUtc = programStartTime
                    });
                    string newConfigFileData = Sw.EncodeConfigFile(entries);
                    Sw.WriteFile(configFilePath, newConfigFileData);
                }
            }
        }
        else
        {
            // Unable to get a config file path.  This is only an error if the task requires
            // a config file.
            if (task == Sw.Task.ListTimers || task == Sw.Task.DeleteNamed || task == Sw.Task.RunNamed)
            {
                Console.WriteLine("Could not determine config file path.");
                return;
            }
        }

        if (task == Sw.Task.RunNamed || task == Sw.Task.RunNameless)
        {
            while(true)
            {
                TimeSpan currentElapsed = sw.Elapsed + elapsedSavedTimer;
                string s = string.Format("{0}:{1:00}:{2:00}:{3:00}:{4:000}",
                    currentElapsed.Days,
                    currentElapsed.Hours,
                    currentElapsed.Minutes,
                    currentElapsed.Seconds,
                    currentElapsed.Milliseconds);
                Console.Write($"  {s}         \r");

                Thread.Sleep(10);
            }
        }
    }
}

