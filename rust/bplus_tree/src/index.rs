use crate::bptree::BPTree;
use std::fs::File;
use std::io::Write as _;

#[derive(Debug)]
pub struct Index {
    tree: BPTree,
}

impl Index {
    pub fn new(data: &[(i32, i32)]) -> Self {
        let mut tree = BPTree::new();

        for &(key, value) in data {
            tree.insert(key, value);
        }

        Index { tree }
    }

    pub fn key_query(&self, query_keys: &[i32]) {
        let mut f = File::create("key_query_out.txt").expect("Failed to open output file!");

        for &query in query_keys {
            let value = self.tree.get(query).unwrap_or_else(|| -1);
            //eprintln!("Query {}: {}", query, value);
            writeln!(&mut f, "{}", value).unwrap();
        }

        f.flush().unwrap();
    }

    pub fn range_query(&self, query_pairs: &[(i32, i32)]) {
        let mut f = File::create("range_query_out.txt").expect("Failed to open output file!");

        for &(left, right) in query_pairs {
            if let Some(iter) = self.tree.search_key(left) {
                let value = iter
                    .take_while(|(k, _)| *k <= right)
                    .map(|(_, v)| v)
                    .max()
                    .unwrap_or_else(|| -1);
                //eprintln!("Query {} ~ {}: {}", left, right, value);
                writeln!(&mut f, "{}", value).unwrap();
            } else {
                writeln!(&mut f, "-1").unwrap();
            }
        }

        f.flush().unwrap();
    }

    pub fn clear_index(self) {}
}
