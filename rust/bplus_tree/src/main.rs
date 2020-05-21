use index::Index;
use std::error::Error as StdError;
use std::time::*;

mod bptree;
mod index;
mod testdata;

fn main() -> Result<(), Box<dyn StdError>> {
    let data = testdata::read_input_file()?;
    let query_keys = testdata::read_key_query_file()?;
    let query_pairs = testdata::read_range_query_file()?;

    eprintln!("Starting to build the tree");
    let start = Instant::now();
    // Build index when index constructor is called
    let myindex = Index::new(&data);
    eprintln!("Tree building completed");
    let build_index = Instant::now();

    // Query by key
    eprintln!("Starting to query by keys");
    myindex.key_query(&query_keys);
    let key_query = Instant::now();
    eprintln!("Query completed");

    // Query by range of key
    eprintln!("Starting to query by ranges");
    myindex.range_query(&query_pairs);
    let range_query = Instant::now();
    eprintln!("Query completed");

    // Free memory
    myindex.clear_index();

    let time_to_build_index = build_index.duration_since(start);
    let time_to_query_key = key_query.duration_since(build_index);
    let time_to_query_range = range_query.duration_since(key_query);
    testdata::record_time_used(time_to_build_index, time_to_query_key, time_to_query_range)?;

    Ok(())
}
