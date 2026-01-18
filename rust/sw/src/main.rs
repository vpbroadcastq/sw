

fn main() {
    let program_start_sys = std::time::SystemTime::now();
    let program_start_steady = std::time::Instant::now();
    println!("Hello, world!");

    // You can pass any iterator/collection without cloning inside determine_task.
    // std::env::args() yields owned Strings already (from the OS), and we avoid extra copies.
    let task: Task = determine_task(std::env::args());
    if task == Task::InvalidCommandline {
        eprintln!("Invalid command line arguments.\nError:  Run 'sw --help' for usage information.");
        return;
    }
    if task == Task::PrintHelp {
        println!("{}", HELP_TEXT);
        return;
    }

    let cf:std::option::Option<std::path::PathBuf> = config_file_path();
    let mut elapsed_saved_timer:std::time::Duration = std::time::Duration::ZERO;
    if cf.is_some() {
        let config_file_data:std::result::Result<std::string::String, std::io::Error> = read_file(cf.as_ref().expect(""));
        let mut saved_timers:std::vec::Vec<TimerEntry> = if config_file_data.is_ok() {
            decode_config_file_data(&config_file_data.expect(""))
        } else {
            std::vec::Vec::<TimerEntry>::new()
        };

        if task == Task::DeleteNamed {
            let mut a: std::env::Args = std::env::args();
            a.next();  // Skip the full command line
            let name:std::option::Option<std::string::String> = a.next();  // Skip the option indicating timer delete
            if let Some(idx) = saved_timers.iter().position(|e|{e.name == name.as_deref().expect("")}) {
                let idx:usize = idx;
                saved_timers.swap_remove(idx);
                let new_config_file_data:std::string::String = encode_config_file_data(&saved_timers);
                let _ = write_file(cf.as_ref().expect(""), &new_config_file_data);
            } else {
                eprintln!("No timer named \"{}\" found in config file.\n", name.as_ref().expect(""));
                return;
            }
        } else if task == Task::ListTimers {
            for t in &saved_timers {
                let t:&TimerEntry = t;
                println!("{}", t.name);
            }
            return;
        }

        // Either RunNamed or RunNameless
        // When did the timer start?  It's either the value in the config file, or when the program was started
        // TODO:  Why not assign program_start_time right here &! have it be an option<>?
        let mut named_timer_start: std::option::Option<std::time::SystemTime> = None;
        if task == Task::RunNamed {
            let mut a: std::env::Args = std::env::args();
            a.next();  // Skip the full command line
            let tname:std::option::Option<std::string::String> = a.next();  // Skip the option indicating timer delete
            named_timer_start = tstart_if_exists(&saved_timers, tname.as_deref().expect(""));
        }

        if task == Task::RunNamed && named_timer_start.is_none() {
            let mut a: std::env::Args = std::env::args();
            a.next();  // Skip the full command line
            let tname:std::option::Option<std::string::String> = a.next();  // Skip the option indicating timer delete
            let new_timer:TimerEntry = TimerEntry {name: tname.expect(""), elapsed: program_start_sys};
            saved_timers.push(new_timer);
            let new_filedata:std::string::String = encode_config_file_data(&saved_timers);
            let _ = write_file(cf.as_ref().expect(""), &new_filedata);
        }

        if named_timer_start.is_some() {
            elapsed_saved_timer = program_start_sys.duration_since(named_timer_start.expect("")).expect("");
        }
    } else {
        // Unable to get a config file path.  This is only an error if the task requires a config file.
        if task == Task::RunNamed || task == Task::ListTimers || task == Task::DeleteNamed {
            eprintln!("Unable to determine config file path.");
            return;
        }
    }


    run(elapsed_saved_timer, program_start_steady);

}


#[derive(PartialEq, Eq)]
enum Task {
    RunNamed,
    RunNameless,
    ListTimers,
    DeleteNamed,
    InvalidCommandline,
    PrintHelp,
}


fn determine_task<S>(mut args: impl std::iter::Iterator<Item=S>) -> Task
where
    S: AsRef<str>
{
    args.next();  // Skip the full command line
    let a0: Option<S> = args.next();

    match a0.as_ref().map(|s| s.as_ref()) {
        None => Task::RunNameless,
        Some("-l") | Some("--list-timers") => Task::ListTimers,
        Some("-d") | Some("--delete-timer") => {
            if args.next().is_some() {
                Task::DeleteNamed
            } else {
                Task::InvalidCommandline  // No name provided to delete
            }
        }
        Some("-h") | Some("-?") | Some("--help") => Task::PrintHelp,
        Some(_) => Task::RunNamed,
    }
}


fn is_valid_timer_name(name: &str) -> bool {
    if name.len() == 0 || name.starts_with('-') {
        return false;
    }
    return true;
}


fn run(elapsed_saved_timer:std::time::Duration, program_start_steady: std::time::Instant) {
    let mut dest:std::string::String;
    loop {
        let elapsed: std::time::Duration = std::time::Instant::now() - program_start_steady + elapsed_saved_timer;
        let elapsed_ms: u128 = elapsed.as_millis() % 1000;
        let elapsed_secs: u64 = elapsed.as_secs() % 60;
        let elapsed_mins: u64 = (elapsed.as_secs() / 60) % 60;
        let elapsed_hours: u64 = elapsed.as_secs() / 3600; // TODO:  %?
        let elapsed_days: u64 = elapsed.as_secs() / 86400; // TODO:  %?
        
        if elapsed_days > 0 {
            dest = format!("  {}:{:02}:{:02}:{:02}:{:03}                              ",
                elapsed_days, elapsed_hours % 24, elapsed_mins, elapsed_secs, elapsed_ms);
        } else if elapsed_hours > 0 {
            dest = format!("  {}:{:02}:{:02}:{:03}                              ",
                elapsed_hours, elapsed_mins, elapsed_secs, elapsed_ms);
        } else if elapsed_mins > 0 {
            dest = format!("  {}:{:02}:{:03}                              ",
                elapsed_mins, elapsed_secs, elapsed_ms);
        } else {
            dest = format!("  {}:{:03}                              ",
                elapsed_secs, elapsed_ms);
        }
        
        print!("{}\r", dest);

        std::thread::sleep(std::time::Duration::from_millis(10));
    }
}


fn read_file(path: &std::path::Path) -> std::result::Result<std::string::String, std::io::Error> {
    // TODO:  What if the file doesn't exist?
    let mut file: std::fs::File = std::fs::File::open(path)?;
    let mut contents= std::string::String::new();
    use std::io::Read;
    file.read_to_string(&mut contents)?;
    return Ok(contents);
}


// TODO:  This should take a str?
fn write_file(path: &std::path::Path, data: &std::string::String) -> std::result::Result<(), std::io::Error> {
    // TODO:  What if file doesn't exist?
    let mut file: std::fs::File = std::fs::File::open(path)?;
    use std::io::Write;
    // TODO:  write_all && .as_bytes() vs some sort of string writing method?
    file.write_all(&data.as_bytes())?;
    return Ok(());
}


fn config_file_path() -> std::option::Option<std::path::PathBuf> {
    let fname:&std::path::Path = std::path::Path::new("sw-r.ini");
    let conf_path:std::result::Result<std::string::String, std::env::VarError> = std::env::var("XDG_CONFIG_HOME");
    if conf_path.is_ok() {
        return Some(std::path::Path::new(&conf_path.unwrap()).join(fname));
    }

    let home_dir:std::result::Result<std::string::String, std::env::VarError> = std::env::var("HOME");
    if home_dir.is_ok() {
        return Some(std::path::Path::new(&home_dir.unwrap()).join(".config").join(fname));
    }

    return None;
}


pub struct TimerEntry {
    name:std::string::String,
    elapsed:std::time::SystemTime,
}

fn decode_config_file_data(fdata: &str) -> std::vec::Vec<TimerEntry> {
    let mut entries: std::vec::Vec<TimerEntry> = std::vec::Vec::<TimerEntry>::new();
    let mut curr_name: std::string::String = std::string::String::new();
    for ln in fdata.lines() {
        let ln: &str = ln;
        let trimmed:&str = ln.trim();
        if trimmed.is_empty() {
            continue;
        }
        if trimmed.starts_with('[') && trimmed.ends_with(']') {
            curr_name = trimmed[1..trimmed.len()-1].to_string();
        } else if is_valid_timer_name(&curr_name) {
            // Extract timestamp for the current timer
            // TODO:  to_system_time
            let secs:u64 = match trimmed.parse::<u64>() {
                Ok(v) => v,
                Err(_) => {
                    curr_name.clear();
                    continue;
                }
            };
            let t:std::time::SystemTime = std::time::UNIX_EPOCH + std::time::Duration::from_secs(secs);
            entries.push(TimerEntry{ name: std::mem::take(&mut curr_name), elapsed: t });
        }
    }

    return entries;
}


fn encode_config_file_data(timers:&std::vec::Vec<TimerEntry>) -> std::string::String {
    let mut s:std::string::String = std::string::String::new();
    for t in timers {
        let t:&TimerEntry = t;
        // TODO:  Ineffecient.  Should be able to format "into" s w/o involving 'temp'
        let temp:std::string::String = format!("[{}]\n{}\n\n", t.name, to_string(t.elapsed));
        s.push_str(&temp);
    }
    return s;
}


fn tstart_if_exists(timers:&std::vec::Vec<TimerEntry>, tname:&str) -> std::option::Option<std::time::SystemTime> {
    match timers.iter().find(|t|{let t:&TimerEntry = t; return t.name == tname;}) {
        Some(entry) => return Some(entry.elapsed),
        None => return None
    }
}


// String is assumed to be a number of seconds since UNIX_EPOCH
fn to_system_time(s: &str) -> std::result::Result<std::time::SystemTime, ()> {
    use std::str::FromStr;
    let seconds: u64 = match u64::from_str(s) {
        Ok(v) => v,
        Err(_) => return Err(()),
    };

    return Ok(std::time::SystemTime::UNIX_EPOCH + std::time::Duration::from_secs(seconds));
}


// TODO:  Pass by ref?
fn to_string(t:std::time::SystemTime) -> std::string::String {
    return format!("{}", t.duration_since(std::time::UNIX_EPOCH).unwrap().as_secs());
}


const HELP_TEXT: &'static str = "\
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

";


