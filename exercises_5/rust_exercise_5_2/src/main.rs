use std::io::Seek;
use rand::prelude::*;
use std::io::Write;
use std::fs::OpenOptions;
use std::{thread, time, process};


// Sleep for a random time to enforce race conditions
fn sleep_random_time() {
    let mut rng = rand::thread_rng();
    let val = rng.gen_range(2..10);
    thread::sleep(time::Duration::from_secs(val));
}

/// Run this program as:
/// ./rust_exercise_5_2 & ./rust_exercise_5_2 [& ...]
///
/// Each of these threads will not overwrite either as write on a file in
/// append mode is always at the last point of file seeking, regardless of if
/// there was a seek done before hand.
fn main() {
    // a poor file name
    const FILE_NAME: &str = "poor_file.fl";
    // open file multiple times in append mode
    let mut file = match OpenOptions::new()
        .create(true)
        .write(true)
        .append(true)
        .open(FILE_NAME) {
            Ok(f) => { f },
            Err(e) => {
                panic!("File open error: {}", e.to_string());
            }
    };

    // Remember PID for logging
    let pid = process::id();
    sleep_random_time();
    {
        println!("PID {} writing for the first time", pid);
        match file.seek(std::io::SeekFrom::Start(0)) {
            Ok(_) => {},
            Err(e) => { panic!("Seek error: {}", e.to_string()) }
        };

        match write!(
            &mut file,
            "Here is the first string, pretty long string. Wow!") {
                Ok(_) => {},
                Err(e) => {
                    panic!("Write error: {}", e.to_string())
                }
        };
    }
    sleep_random_time();
    {
        println!("PID {} writing for the second time", pid);
        match file.seek(std::io::SeekFrom::Start(0)) {
                Ok(_) => {},
                Err(e) => {
                    panic!("Seek error: {}", e.to_string())
                }
        };

        match write!(
            &mut file,
            "Second string. Not too long as first. Still wow!") {
                Ok(_) => {},
                Err(e) => {
                    panic!("Write error: {}", e.to_string())
                }
        };
    }
    sleep_random_time();
    {
        println!("PID {} writing for the third time", pid);
        match file.seek(std::io::SeekFrom::Start(0)) {
            Ok(_) => {},
            Err(e) => { panic!("Seek error: {}", e.to_string()) }
        };

        match write!(
            &mut file,
            "Third string. Still wow? I don't know!") {
                Ok(_) => {},
                Err(e) => {
                    panic!("Write error: {}", e.to_string())
                }
        };
    }
    sleep_random_time();
    {
        println!("PID {} writing for the fourth time", pid);
        match file.seek(std::io::SeekFrom::Start(0)) {
            Ok(_) => {},
            Err(e) => { panic!("Seek error: {}", e.to_string()) }
        };

        match write!(
            &mut file,
            "Fourth string. Not really wow.") {
                Ok(_) => {},
                Err(e) => {
                    panic!("Write error: {}", e.to_string())
                }
        };
    }
    sleep_random_time();
    {
        println!("PID {} writing for the fifth time", pid);
        match file.seek(std::io::SeekFrom::Start(0)) {
            Ok(_) => {},
            Err(e) => { panic!("Seek error: {}", e.to_string()) }
        };

        match write!(
            &mut file,
            "Fifth. Not wow.") {
                Ok(_) => {},
                Err(e) => {
                    panic!("Write error: {}", e.to_string())
                }
        };
    }
}
