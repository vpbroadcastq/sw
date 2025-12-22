

fn main() {
    //let program_start_sys = std::time::SystemTime::now();
    let program_start_steady = std::time::Instant::now();
    println!("Hello, world!");

    // You can pass any iterator/collection without cloning inside determine_task.
    // std::env::args() yields owned Strings already (from the OS), and we avoid extra copies.
    let _task: Task = determine_task(std::env::args());

    run(std::time::Duration::from_secs(0), program_start_steady);

}


enum Task {
    RunNamed,
    RunNameless,
    ListTimers,
    DeleteNamed,
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
                Task::RunNameless  // Error case: no name provided
            }
        }
        Some(_) => Task::RunNamed,
    }
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

