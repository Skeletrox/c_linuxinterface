extern crate getopts;
use getopts::Options;
use std::{env, io::Read, io::Write};
use std::fs::{OpenOptions};
use std::io::{BufReader, BufWriter, Seek, SeekFrom};
use rand::{distributions::Alphanumeric, Rng};

/// This exercise has two modes.
/// 1. pre-exercise mode, denoted with the flag -p. Here we shall setup a file
/// with holes in it.
/// 2. exercise mode, without the flag. Here we shall copy a file with holes
/// to another.

fn print_usage(prog_name: &str, opts: Options) {
    let brief = format!("Usage: {} [-p inp_file] [inp_file out_file]",
                                prog_name);
    println!("{}", opts.usage(&brief));
}

fn gen_rand_str(length: usize) -> String {
    let rng = rand::thread_rng();

    rng.sample_iter(&Alphanumeric)
        .take(length)
        .map(char::from)
        .collect()
}

/// Creates a file and fills it with holes such that the file is as follows:
/// [string][holes][string]
fn fill_file_holes(file_name: &str) {
    // The "length" of the hole.
    const SEEK_LEN: i64 = 65536;
    // Length of the "garbage" string
    const STR_LEN: usize = 32768;

    let garbage = gen_rand_str(STR_LEN);

    // An opened file is closed automatically when it goes out of scope
    let file = match OpenOptions::new()
        .create(true)
        .write(true)
        .truncate(true)
        .open(file_name) {
            Ok(f) => { f },
            Err(e) => { panic!("File open error: {}", e.to_string()) } 
    };

    let mut file_buf = BufWriter::new(file);

    match write!(&mut file_buf, "{}", garbage.as_str()) {
        Ok(_) => {},
        Err(e) => { panic!("Writing garbage error: {}", e.to_string()) }
    };

    // seek() or seek_relative() can be used
    match file_buf.seek(SeekFrom::Current(SEEK_LEN)) {
        Ok(_) => {},
        Err(e) => { panic!("Seeking file error: {}", e.to_string()) }
    };

    match write!(&mut file_buf, "{}", garbage.as_str()) {
        Ok(_) => {},
        Err(e) => { 
            panic!("Writing garbage error 2: {}", e.to_string())
        }
    };
}

fn copy_file_holes(source_file: &str, target_file: &str) {
    let src = match OpenOptions::new()
        .read(true)
        .open(source_file) {
            Ok(f) => { f },
            Err(e) => {
                panic!("Source file open error: {}", e.to_string())
            }
    };

    let tgt = match OpenOptions::new()
        .create(true)
        .write(true)
        .truncate(true)
        .open(target_file) {
            Ok(f) => { f },
            Err(e) => {
                panic!("Target file open error: {}", e.to_string())
            }
    };

    const BUFFER_SIZE: usize = 1024;
    let mut buffer  = [0; BUFFER_SIZE];

    // Buffer reads and writes
    let mut in_buffer = BufReader::new(src);
    let mut out_buffer = BufWriter::new(tgt);

    let mut n_holes = 0;
    // Keep reading until we return 0 bytes
    while match in_buffer.read(&mut buffer) {
            Ok(n_bytes) => { n_bytes }
            Err(_) => { 0 }
    } != 0 {
        for mut x in buffer {
            // If this is a hole, increment the number of holes we have
            // seen so far
            if x == b'\0' {
                n_holes += 1;
                continue;
            }

            // If this not a hole, but we have a batch of holes to be written,
            // then seek the target to the number of holes and write this to
            // the file.
            if n_holes > 0 {
                match out_buffer.flush() {
                    Ok(_) => {},
                    Err(e) => {
                        panic!("Buffer flush error: {}", e.to_string())
                    }
                };
                match out_buffer.seek_relative(n_holes) {
                    Ok(_) => { n_holes = 0; },
                    Err(e) => {
                        panic!("Target hole write error: {}", e.to_string())
                    }
                }
            }

            match out_buffer.write(std::slice::from_mut(&mut x)) {
                Ok(_) => {},
                Err(e) => {
                    panic!("Target byte write error: {}", e.to_string())
                }
            };

        }
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();

    // This program should be invoked as:
    // prog -p [file_1]
    // prog [file_1] [file_2]
    
    let mut opts = Options::new();

    opts.optopt("p", "preexercise",
                 "Generate a file with holes", "INPUT_FILE");
    let matches = match opts.parse(&args[1..]) {
        Ok(m) => { m },
        Err(f) => { panic!("{}", f.to_string()) }
    };

    // Check if we are in pre-exercise mode
    let source_file: String;
    let target_file: String;
    if matches.opt_present("p") {
        if !matches.free.is_empty() {
            print_usage(&args[0], opts);
            return;
        }
        source_file = match matches.opt_str("p") {
            Some(x) => { x },
            None => {
                print_usage(&args[0], opts);
                return;
            }
        };
        fill_file_holes(source_file.as_str());
    } else {
        if matches.free.len() != 2 {
            print_usage(&args[0], opts);
            return;
        }
        source_file = matches.free[0].clone();
        target_file = matches.free[1].clone();
        copy_file_holes(source_file.as_str(),
                        target_file.as_str());
    }
}
