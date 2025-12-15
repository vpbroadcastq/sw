using System.Globalization;
using System.Diagnostics;

class Program
{
    static void Main(string[] args)
    {
        string pathTestConfigFile = "/home/ben/personal/dev/experiments/sw/csharp/test.ini";
        DateTimeOffset programStartTime = DateTimeOffset.UtcNow;
        Stopwatch sw = new Stopwatch();
        sw.Start();

        IEnumerable<ConfigFile.TimerEntry> entries = ConfigFile.ReadConfigFile(pathTestConfigFile);

        TimeSpan elapsedSavedTimer = TimeSpan.Zero;
        if (args.Length == 1)
        {
            string timerName = args[0];
            DateTimeOffset? startTimeUtcSavedTimer = ConfigFile.StartTimeIfPresent(timerName, entries);
            if (startTimeUtcSavedTimer.HasValue)
            {
                // The user wants to load a saved timer
                elapsedSavedTimer = programStartTime - startTimeUtcSavedTimer.Value;
            }
            else
            {
                ConfigFile.WriteTimerToConfigFile(pathTestConfigFile,
                    new ConfigFile.TimerEntry
                    {
                        TimerName = timerName,
                        StartTimeUtc = programStartTime
                    });
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
        }
    }
}

