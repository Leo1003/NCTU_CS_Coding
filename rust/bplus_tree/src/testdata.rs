use std::fs::File;
use std::io::{
    BufRead as _, BufReader, Error as IoError, ErrorKind, Result as IoResult, Write as _,
};
use std::time::Duration;

fn parse_csvline(s: &str) -> IoResult<Vec<i32>> {
    s.split(',')
        .map(|i| i.trim())
        .map(|i| i.parse::<i32>())
        .collect::<Result<Vec<i32>, _>>()
        .map_err(|e| IoError::new(ErrorKind::InvalidData, e))
}

pub fn read_input_file() -> IoResult<Vec<(i32, i32)>> {
    let f = File::open("data.txt")?;
    let reader = BufReader::new(f);

    let mut buf = Vec::new();
    for lines in reader.lines() {
        let s = lines?;
        let vals = parse_csvline(&s)?;
        buf.push((vals[0], vals[1]));
    }
    println!("Data file reading complete, {} rows loaded.", buf.len());

    Ok(buf)
}

pub fn read_key_query_file() -> IoResult<Vec<i32>> {
    let f = File::open("key_query.txt")?;
    let reader = BufReader::new(f);

    let mut buf = Vec::new();
    for lines in reader.lines() {
        let s = lines?;
        let vals = parse_csvline(&s)?;
        buf.push(vals[0]);
    }
    println!(
        "Key query file reading complete, {} queries loaded.",
        buf.len()
    );

    Ok(buf)
}

pub fn read_range_query_file() -> IoResult<Vec<(i32, i32)>> {
    let f = File::open("range_query.txt")?;
    let reader = BufReader::new(f);

    let mut buf = Vec::new();
    for lines in reader.lines() {
        let s = lines?;
        let vals = parse_csvline(&s)?;
        buf.push((vals[0], vals[1]));
    }
    println!(
        "Range query file reading complete, {} queries loaded.",
        buf.len()
    );

    Ok(buf)
}

pub fn record_time_used(
    time_to_build_index: Duration,
    time_to_query_key: Duration,
    time_to_query_range: Duration,
) -> IoResult<()> {
    let mut f = File::create("time_used.txt")?;
    writeln!(
        &mut f,
        "{},{},{}",
        time_to_build_index.as_micros(),
        time_to_query_key.as_micros(),
        time_to_query_range.as_micros()
    )?;
    Ok(())
}
