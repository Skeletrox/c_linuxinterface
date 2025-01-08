extern crate getopts;
use getopts::Options;
use std::{env, io::Write};
use std::fs::File;
use std::fs::OpenOptions;


fn print_usage(prog_name: &str, opts: Options) {
    let usage = format!("Usage: {} [-a] FILE", prog_name);
    println!("{}", opts.usage(&usage));
}
fn main() -> std::io::Result<()> {
    let args: Vec<String> = env::args().collect();
    // We expect the program to be called as:
    // prog [-a] file_name
    let mut opts = Options::new();
    opts.optflag("a", "append",
            "Append to file instead of writing new");

    let matches = match opts.parse(&args[1..]) {
        Ok(m) => { m },
        Err(f) => { panic!("{}", f.to_string()) }
    };
    let append_mode: bool = matches.opt_present("a");
    
    let n_expected_args = if append_mode { 3 } else { 2 };

    if args.len() != n_expected_args {
        print_usage(args[0].as_str(), opts);
        return Ok(())
    }
    
    let file_name = args[args.len() - 1].clone();

    // Create a buffer size and accept user input
    let buffer_size = 128;
    let mut input = String::with_capacity(buffer_size);

    let mut file: File;
    if append_mode {
        file = OpenOptions::new().create(true).append(true).open(
            file_name.as_str()).unwrap();
    } else {
        file = OpenOptions::new().create(true).write(true).truncate(true).open(
            file_name.as_str()).unwrap();
    }
    
    
    loop {
        let n_bytes = match std::io::stdin().read_line(
            &mut input) {
                Ok(n_bytes) => { n_bytes },
                Err(f) => { 
                    println!("{}", f.to_string());
                    break;
                }
        };
        if n_bytes == 0 {
            break;
        }
        print!("{}", input);
        // print! requires explicit buffer flushing
        std::io::stdout().flush().unwrap();

        write!(&mut file, "{}", input.as_str())?;
        input.clear();
    }
    Ok(())
}