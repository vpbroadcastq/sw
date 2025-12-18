using System.Globalization;

internal static class TestData
{
    // All these example files use the same timestamp
    public static DateTimeOffset GetTimestamp()
    {
        DateTimeOffset.TryParseExact("2025-12-17T14:39:06.7866990+00:00",
            "O", CultureInfo.InvariantCulture, DateTimeStyles.RoundtripKind,
            out DateTimeOffset startedUtc);
        return startedUtc;
    }
    //
    // All entries are invalid
    //
    public static readonly string AllInvalid = """
        ] BackwardsBrackets [
        2025-12-17T14:39:06.7866990+00:00

        NoBrackets
        2025-12-17T14:39:06.7866990+00:00

        [ MissingRightBracket
        2025-12-17T14:39:06.7866990+00:00

        MissingLeftBracket]
        2025-12-17T14:39:06.7866990+00:00

        [MissingTimestamp]


        """;


    //
    // All entries are valid
    //
    public static readonly string AllValid = """
        [whatever]
        2025-12-17T14:39:06.7866990+00:00

        [name with spaces]
        2025-12-17T14:39:06.7866990+00:00


        [Name~with " special' ;chars!@#$%^&]
        2025-12-17T14:39:06.7866990+00:00

        """;

    public static readonly List<Sw.TimerEntry> AllValidEntries = new List<Sw.TimerEntry> {
        new Sw.TimerEntry { TimerName = "whatever", StartTimeUtc = GetTimestamp() },
        new Sw.TimerEntry { TimerName = "name with spaces", StartTimeUtc = GetTimestamp() },
        new Sw.TimerEntry { TimerName = "Name~with \" special' ;chars!@#$%^&", StartTimeUtc = GetTimestamp() }
    };

    public static readonly string AllValidReencoded = """
        [whatever]
        2025-12-17T14:39:06.7866990+00:00

        [name with spaces]
        2025-12-17T14:39:06.7866990+00:00

        [Name~with " special' ;chars!@#$%^&]
        2025-12-17T14:39:06.7866990+00:00


        """;


    //
    // Mixed valid and invalid entries
    //
    public static readonly string MixedValidInvalid = """
        [whatever]
        2025-12-17T14:39:06.7866990+00:00

        []
        2025-12-17T14:39:06.7866990+00:00

        [name with spaces]
        2025-12-17T14:39:06.7866990+00:00

        MissingLeftBracket]
        2025-12-17T14:39:06.7866990+00:00

        2025-12-17T14:39:06.7866990+00:00

        [Name~with " special' ;chars!@#$%^&]
        2025-12-17T14:39:06.7866990+00:00

        [   name with leading&trailing spaces  ]
        2025-12-17T14:39:06.7866990+00:00

        """;

    public static readonly List<Sw.TimerEntry> MixedValidInvalidEntries = new List<Sw.TimerEntry> {
        new Sw.TimerEntry { TimerName = "whatever", StartTimeUtc = GetTimestamp() },
        new Sw.TimerEntry { TimerName = "name with spaces", StartTimeUtc = GetTimestamp() },
        new Sw.TimerEntry { TimerName = "Name~with \" special' ;chars!@#$%^&", StartTimeUtc = GetTimestamp() },
        new Sw.TimerEntry { TimerName = "   name with leading&trailing spaces  ", StartTimeUtc = GetTimestamp() }
    };

    public static readonly string MixedValidInvalidReencoded = """
        [whatever]
        2025-12-17T14:39:06.7866990+00:00

        [name with spaces]
        2025-12-17T14:39:06.7866990+00:00

        [Name~with " special' ;chars!@#$%^&]
        2025-12-17T14:39:06.7866990+00:00

        [   name with leading&trailing spaces  ]
        2025-12-17T14:39:06.7866990+00:00


        """;
}


