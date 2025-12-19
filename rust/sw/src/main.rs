

fn main() {
    //let program_start_sys = std::time::SystemTime::now();
    let program_start_steady = std::time::Instant::now();
    println!("Hello, world!");

    run(std::time::Duration::from_secs(0), program_start_steady);

}


fn run(elapsed_saved_timer:std::time::Duration, program_start_steady: std::time::Instant) {
    let mut dest:std::string::String;
    while (true) {
        let elapsed = std::time::Instant::now() - program_start_steady + elapsed_saved_timer;
        let elapsed_ms = elapsed.as_millis() % 1000;
        let elapsed_secs = elapsed.as_secs() % 60;
        let elapsed_mins = (elapsed.as_secs() / 60) % 60;
        let elapsed_hours = elapsed.as_secs() / 3600; // TODO:  %?
        let elapsed_days = elapsed.as_secs() / 86400; // TODO:  %?
        
        if (elapsed_days > 0) {
            dest = format!("  {} days {:02}:{:02}:{:02}.{:03}                              ",
                elapsed_days, elapsed_hours % 24, elapsed_mins, elapsed_secs, elapsed_ms);
        } else if (elapsed_hours > 0) {
            dest = format!("  {:02}:{:02}:{:02}.{:03}                              ",
                elapsed_hours, elapsed_mins, elapsed_secs, elapsed_ms);
        } else if (elapsed_mins > 0) {
            dest = format!("  {:02}:{:02}.{:03}                              ",
                elapsed_mins, elapsed_secs, elapsed_ms);
        } else {
            dest = format!("  {:02}.{:03}                              ",
                elapsed_secs, elapsed_ms);
        }
        
        print!("{}\r", dest);

        std::thread::sleep(std::time::Duration::from_millis(10));
    }
}

