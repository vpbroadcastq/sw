using System.Globalization;
using System.Diagnostics;

class Program
{
    // TODO:  Error handling and reporting for if the config file path can't be obtained || if it can't be
    // created or written to.  This is not present right now because i am hardcoding the path for testing
    // purposes.
    static void Main(string[] args)
    {
        string pathTestConfigFile = "/home/ben/personal/dev/sw/csharp/test.ini";
        DateTimeOffset programStartTime = DateTimeOffset.UtcNow;
        Stopwatch sw = new Stopwatch();
        sw.Start();

        TimeSpan elapsedSavedTimer = TimeSpan.Zero;

        Sw.Task task = Sw.DetermineTask(args);

        List<Sw.TimerEntry> entries = Sw.ReadConfigFile(pathTestConfigFile);
        if (entries.Count > 0)
        {
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
                    Sw.WriteConfigFile(entries, pathTestConfigFile);
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
                    Sw.WriteConfigFile(entries, pathTestConfigFile);
                }
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



        /*if (args.Length == 1)
        {
            string timerName = args[0];
            DateTimeOffset? startTimeUtcSavedTimer = Sw.StartTimeIfPresent(timerName, entries);
            if (startTimeUtcSavedTimer.HasValue)
            {
                // The user wants to load a saved timer
                elapsedSavedTimer = programStartTime - startTimeUtcSavedTimer.Value;
            }
            else
            {
                // The user is creating a new named timer
                entries.Add(new Sw.TimerEntry
                {
                    TimerName = timerName,
                    StartTimeUtc = programStartTime
                });
                Sw.WriteConfigFile(entries, pathTestConfigFile);
            }
        }
        
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
        }*/
    }
}

