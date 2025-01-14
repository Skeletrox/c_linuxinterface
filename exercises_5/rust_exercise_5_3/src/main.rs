use std::{env, io::Write, thread, time, process};
use std::fs::OpenOptions;
use std::io::{BufWriter, Seek, SeekFrom};
use rand::Rng;


fn get_random_char() -> char {
    let mut rng = rand::thread_rng();
    rng.gen_range('a'..='z')
}

// Sleep for a random time to enforce race conditions
fn sleep_random_time() {
    let mut rng = rand::thread_rng();
    let val = rng.gen_range(2..10);
    thread::sleep(time::Duration::from_secs(val));
}


fn print_usage(prog_name: &str) {
    println!("Usage: {} FILE n_bytes [x]", prog_name);
}
fn main() {
    let args: Vec<String> = env::args().collect();
    let pid = process::id();

    let n_args = args.len();
    let prog_name = args[0].as_str();

    if n_args != 3 && n_args != 4 {
        print_usage(prog_name);
    }

    let file_name = args[1].as_str();
    let n_bytes = match str::parse::<usize>(args[2].as_str()) {
        Ok(n) => { n },
        Err(e) => {
            print_usage(prog_name);
            panic!("{}", e.to_string()); 
        }
    };

    let nonatomic_append_mode: bool;
    if n_args == 4 {
        // Validate that the last entry is 'x'
        if !args[3].eq_ignore_ascii_case("x") {
            print_usage(prog_name);
            return;
        }
        nonatomic_append_mode = true;
    } else {
        nonatomic_append_mode = false;
    }

    let random_char = get_random_char();
    println!("random char is {} for PID {}", random_char, pid);

    let file = match OpenOptions::new()
        .create(true)
        .write(true)
        .append(!nonatomic_append_mode)
        .open(file_name) {
            Ok(f) => { f }
            Err(e) =>  { panic!("File open error: {}", e.to_string()) }
        };

    let mut file_buf = BufWriter::new(file);

    let pos = match file_buf.seek(SeekFrom::End(0)) {
        Ok(p) => { p },
        Err(e) => { panic!("Seek error: {}", e.to_string()) }
    };
    // Even though the seek position is 0 for all the processes (assuming that
    // none of them have written yet) - opening the file in append mode
    // guarantees atomically seeking to the end of the file and writing it.
    // In case of non-atomic appends, we end up with the issue of overwriting.
    println!("seek pos is {} for PID {}", pos, pid);

    sleep_random_time();

    let mut writable_string = String::with_capacity(n_bytes);
    for _ in 0..n_bytes {
        writable_string.push(random_char);
    }

    match write!(&mut file_buf, "{}", writable_string.as_str()) {
        Ok(_) => {},
        Err(e) => { panic!("Write error: {}", e.to_string()) }
    };
}
