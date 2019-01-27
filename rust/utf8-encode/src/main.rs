use std::io;
use std::io::Write;

fn main() {
    loop {
        let mut input = String::new();
        if io::stdin().read_line(&mut input).expect("Failed to read from stdin") == 0 {
            // Reached EOF
            break;
        }
        let bytes = input.trim_end_matches('\n').as_bytes();
        let mut i = 0;
        for hex in bytes {
            print!("{:02X}", hex);
            i += 1;
            if i % 16 == 0 {
                println!("");
            } else {
                print!(" ");
            }
        }
        println!("");
        io::stdout().flush().unwrap();
    }
}
