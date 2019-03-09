extern crate rand;
use std::io;
use std::cmp::Ordering;
use rand::prelude::*;

fn main() {
    println!("Guess number");

    let ans = rand::thread_rng().gen_range(1, 101);

    loop {
        println!("Enter your number: ");
        let mut input = String::new();
        io::stdin().read_line(&mut input)
            .expect("Failed to get input!");

        let input: u32 = match input.trim().parse() {
            Ok(num) => num,
            Err(_) => {
                println!("Invalid input!");
                continue;
            }
        };
            

        match input.cmp(&ans) {
            Ordering::Less => println!("Too small."),
            Ordering::Greater => println!("Too big."),
            Ordering::Equal => {
                println!("You guess it!");
                break;
            }
        }
    }
}
