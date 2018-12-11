use std::io;

fn main() {
    eprint!("Enter string to encrypt/decrypt: ");
    
    let mut input = String::new();
    io::stdin().read_line(&mut input)
        .expect("Failed to read line");

    let result = rot13(&input);
    eprint!("The output is: ");
    println!("{}", result);
}

fn rot13(s: &str) -> String {
    let mut res = String::with_capacity(s.len());
    for c in s.chars() {
        let o = if c.is_ascii_lowercase() {
            char::from(((u32::from(c) - u32::from('a') + 13) % 26 + u32::from('a')) as u8)
        } else if c.is_ascii_uppercase() {
            char::from(((u32::from(c) - u32::from('A') + 13) % 26 + u32::from('A')) as u8)
        } else {
            c
        };
        res.push(o);
    }
    res
}
