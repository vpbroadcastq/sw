using System.Diagnostics;

class Program
{
    static void Main(string[] args)
    {
        DateTimeOffset programStartTime = DateTimeOffset.UtcNow;
        Stopwatch sw = new Stopwatch();
        sw.Start();

        Sw.Task task = Sw.DetermineTask(args);
        if (task == Sw.Task.InvalidCommandline)
        {
            Console.WriteLine("Error:  Invalid command line arguments.\nError:  Run 'sw --help' for usage information.\n");
            return;
        }
        else if (task == Sw.Task.PrintHelp)
        {
            Console.WriteLine(GetUsageText());
            return;
        }

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
                    Console.WriteLine($"{entry.TimerName}");
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

    static string GetUsageText()
    {
        return """
            sw - Simple Stopwatch

            Usage:
            sw                           = Run a nameless timer.  The timer is lost when the program exits.
            sw <timer-name>              = Run a named stopwatch timer saved from a previous run.  If the named timer does not 
                                            exist, it will be created.  The named timer's start time is saved in a config file 
                                            and will persist between runs of the program.  Timer names may not begin with a -
                                            character.
            sw -l,--list                 = List all named timers stored in the config file.
            sw -d <timer-name>,--delete <timer-name>
                                         = Delete the named timer from the config file if it exists.
            sw -?, -h, --help            = Print this help message.

            """;
    }
}

