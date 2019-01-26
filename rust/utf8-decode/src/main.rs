use std::io;
use std::io::Write;

fn main() {
    loop {
        let mut input = String::new();
        if io::stdin().read_line(&mut input).expect("Failed to read from stdin") == 0 {
            // Reached EOF
            break;
        }
        input = input.trim_end_matches('\n').to_string();
        let bytes = input.into_bytes();
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
