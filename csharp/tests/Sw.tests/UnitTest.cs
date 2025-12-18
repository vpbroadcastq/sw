using System.Reflection;
using System.Runtime;
using Xunit;

public class UnitTest
{
    //
    // DetermineTask tests
    //
    [Fact]
    public void DetermineTaskListTimers()
    {
        string[] argsShort = new string[] { "-l" };
        Sw.Task taskShort = Sw.DetermineTask(argsShort);
        Assert.True(taskShort == Sw.Task.ListTimers);

        string[] argsLong = new string[] { "--list-timers" };
        Sw.Task taskLong = Sw.DetermineTask(argsLong);
        Assert.True(taskLong == Sw.Task.ListTimers);
    }
    [Fact]
    public void DetermineTaskRunNameless()
    {
        // No arguments should return RunNameless
        string[] argsEmpty = new string[] { };
        Sw.Task taskEmpty = Sw.DetermineTask(argsEmpty);
        Assert.True(taskEmpty == Sw.Task.RunNameless);
    }

    [Fact]
    public void DetermineTaskRunNamed()
    {
        string[] argsTimerNameQuotedSpaces = new string[] { "\"My Timer\"" };
        Sw.Task taskQuotedSpaces = Sw.DetermineTask(argsTimerNameQuotedSpaces);
        Assert.True(taskQuotedSpaces == Sw.Task.RunNamed);

        string[] argsTimerNameSpecialChars = new string[] { "xfrsge45$%&%$sd.s" };
        Sw.Task taskSpecialChars = Sw.DetermineTask(argsTimerNameSpecialChars);
        Assert.True(taskSpecialChars == Sw.Task.RunNamed);

        string[] argsTimerNameSimple = new string[] { "xhaha" };
        Sw.Task taskSimple = Sw.DetermineTask(argsTimerNameSimple);
        Assert.True(taskSimple == Sw.Task.RunNamed);
    }

    [Fact]
    public void DetermineTaskDeleteNamed()
    {
        string[] argsShort = new string[] { "-d", "myTimer" };
        Sw.Task taskShort = Sw.DetermineTask(argsShort);
        Assert.True(taskShort == Sw.Task.DeleteNamed);

        string[] argsLong = new string[] { "--delete-timer", "myTimer" };
        Sw.Task taskLong = Sw.DetermineTask(argsLong);
        Assert.True(taskLong == Sw.Task.DeleteNamed);
    }

    //
    // DecodeConfigFile tests
    //
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