using Xunit;

public class UnitTest
{
    //
    // DetermineTask tests
    //
    [Fact]
    public void DetermineTaskRunNameless()
    {
        // No arguments should return RunNameless
        string[] argsEmpty = new string[] { };
        Sw.Task taskEmpty = Sw.DetermineTask(argsEmpty);
        Assert.Equal(Sw.Task.RunNameless, taskEmpty);
    }

    [Fact]
    public void DetermineTaskListTimers()
    {
        string[] argsShort = new string[] { "-l" };
        Sw.Task taskShort = Sw.DetermineTask(argsShort);
        Assert.Equal(Sw.Task.ListTimers, taskShort);

        string[] argsLong = new string[] { "--list-timers" };
        Sw.Task taskLong = Sw.DetermineTask(argsLong);
        Assert.Equal(Sw.Task.ListTimers, taskLong);
    }

    [Fact]
    public void DetermineTaskPrintHelp()
    {
        string[] argsH = new string[] { "-h" };
        Sw.Task taskH = Sw.DetermineTask(argsH);
        Assert.Equal(Sw.Task.PrintHelp, taskH);

        string[] argsQuestion = new string[] { "-?" };
        Sw.Task taskQuestion = Sw.DetermineTask(argsQuestion);
        Assert.Equal(Sw.Task.PrintHelp, taskQuestion);

        string[] argsHelp = new string[] { "--help" };
        Sw.Task taskHelp = Sw.DetermineTask(argsHelp);
        Assert.Equal(Sw.Task.PrintHelp, taskHelp);
    }

    [Fact]
    public void DetermineTaskRunNamed()
    {
        string[] argsTimerNameQuotedSpaces = new string[] { "\"My Timer\"" };
        Sw.Task taskQuotedSpaces = Sw.DetermineTask(argsTimerNameQuotedSpaces);
        Assert.Equal(Sw.Task.RunNamed, taskQuotedSpaces);

        string[] argsTimerNameSpecialChars = new string[] { "xfrsge45$%&%$sd.s" };
        Sw.Task taskSpecialChars = Sw.DetermineTask(argsTimerNameSpecialChars);
        Assert.Equal(Sw.Task.RunNamed, taskSpecialChars);

        string[] argsTimerNameSimple = new string[] { "xhaha" };
        Sw.Task taskSimple = Sw.DetermineTask(argsTimerNameSimple);
        Assert.Equal(Sw.Task.RunNamed, taskSimple);

        string[] argsTimerNumber = new string[] { "123" };
        Sw.Task taskNumber = Sw.DetermineTask(argsTimerNumber);
        Assert.Equal(Sw.Task.RunNamed, taskNumber);

        string[] argsTimerUnderscore = new string[] { "my_timer_name" };
        Sw.Task taskUnderscore = Sw.DetermineTask(argsTimerUnderscore);
        Assert.Equal(Sw.Task.RunNamed, taskUnderscore);
    }

    [Fact]
    public void DetermineTaskInvalidCommandline()
    {
        // Unknown flags
        string[] argsUnknownShort = new string[] { "-x" };
        Sw.Task taskUnknownShort = Sw.DetermineTask(argsUnknownShort);
        Assert.Equal(Sw.Task.InvalidCommandline, taskUnknownShort);

        string[] argsUnknownLong = new string[] { "--unknown-flag" };
        Sw.Task taskUnknownLong = Sw.DetermineTask(argsUnknownLong);
        Assert.Equal(Sw.Task.InvalidCommandline, taskUnknownLong);

        // Missing timer name to delete
        string[] argsMissingNameShort = new string[] { "-d" };
        Sw.Task taskMissingNameShort = Sw.DetermineTask(argsMissingNameShort);
        Assert.Equal(Sw.Task.InvalidCommandline, taskMissingNameShort);

        string[] argsMissingNameLong = new string[] { "--delete-timer" };
        Sw.Task taskMissingNameLong = Sw.DetermineTask(argsMissingNameLong);
        Assert.Equal(Sw.Task.InvalidCommandline, taskMissingNameLong);

        // Multiple arguments (invalid scenarios)
        string[] argsTwoArgs = new string[] { "arg1", "arg2" };
        Sw.Task taskTwoArgs = Sw.DetermineTask(argsTwoArgs);
        Assert.Equal(Sw.Task.InvalidCommandline, taskTwoArgs);

        string[] argsThreeArgs = new string[] { "arg1", "arg2", "arg3" };
        Sw.Task taskThreeArgs = Sw.DetermineTask(argsThreeArgs);
        Assert.Equal(Sw.Task.InvalidCommandline, taskThreeArgs);
    }

    [Fact]
    public void DetermineTaskDeleteNamed()
    {
        string[] argsShort = new string[] { "-d", "myTimer" };
        Sw.Task taskShort = Sw.DetermineTask(argsShort);
        Assert.Equal(Sw.Task.DeleteNamed, taskShort);

        string[] argsLong = new string[] { "--delete-timer", "myTimer" };
        Sw.Task taskLong = Sw.DetermineTask(argsLong);
        Assert.Equal(Sw.Task.DeleteNamed, taskLong);
    }

    //
    // IsValidTimerName tests
    //
    [Fact]
    public void IsValidTimerNameValidExamples()
    {
        foreach (string name in TestData.ValidTimerNames)
        {
            Assert.True(Sw.IsValidTimerName(name), $"Expected valid timer name: \"{name}\"");
        }
    }

    [Fact]
    public void IsValidTimerNameInvalidExamples()
    {
        foreach (string name in TestData.InvalidTimerNames)
        {
            Assert.False(Sw.IsValidTimerName(name), $"Expected invalid timer name: \"{name}\"");
        }
    }

    //
    // DecodeConfigFile tests
    //
    [Fact]
    public void DecodeConfigFileNoData()
    {
        string emptyData = "";
        IEnumerable<Sw.TimerEntry> entries = Sw.DecodeConfigFile(emptyData);
        Assert.Empty(entries);
    }

    [Fact]
    public void DecodeConfigFileAllInvalid()
    {
        IEnumerable<Sw.TimerEntry> entries = Sw.DecodeConfigFile(TestData.AllInvalid);
        Assert.Empty(entries);
    }

    [Fact]
    public void DecodeConfigFileAllValid()
    {
        IEnumerable<Sw.TimerEntry> entries = Sw.DecodeConfigFile(TestData.AllValid);
        Assert.Equal(TestData.AllValidEntries.Count, entries.Count());
        for (int i = 0; i < TestData.AllValidEntries.Count; i++)
        {
            Assert.Equal(TestData.AllValidEntries[i].TimerName, entries.ElementAt(i).TimerName);
            Assert.Equal(TestData.AllValidEntries[i].StartTimeUtc, entries.ElementAt(i).StartTimeUtc);
        }
    }

    [Fact]
    public void DecodeConfigFileMixedValidInvalid()
    {
        IEnumerable<Sw.TimerEntry> entries = Sw.DecodeConfigFile(TestData.MixedValidInvalid);
        Assert.Equal(TestData.MixedValidInvalidEntries.Count, entries.Count());
        for (int i = 0; i < TestData.MixedValidInvalidEntries.Count; i++)
        {
            Assert.Equal(TestData.MixedValidInvalidEntries[i].TimerName, entries.ElementAt(i).TimerName);
            Assert.Equal(TestData.MixedValidInvalidEntries[i].StartTimeUtc, entries.ElementAt(i).StartTimeUtc);
        }
    }

    //
    // EncodeConfigFile tests
    //
    [Fact]
    public void EncodeConfigFileEmptyList()
    {
        string encoded = Sw.EncodeConfigFile(new List<Sw.TimerEntry>());
        Assert.Equal("", encoded);
    }

    [Fact]
    public void EncodeConfigFileAllValid()
    {
        string encoded = Sw.EncodeConfigFile(TestData.AllValidEntries);
        Assert.Equal(TestData.AllValidReencoded, encoded);
    }

    [Fact]
    public void EncodeConfigFileMixedValidInvalid()
    {
        string encoded = Sw.EncodeConfigFile(TestData.MixedValidInvalidEntries);
        Assert.Equal(TestData.MixedValidInvalidReencoded, encoded);
    }

}