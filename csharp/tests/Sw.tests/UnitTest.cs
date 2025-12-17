using System.Reflection;
using System.Runtime;
using Xunit;

public class UnitTest
{
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
}
