use std::cell::RefCell;
use std::cmp::PartialEq;
use std::rc::{Rc, Weak};

const DEGREE: usize = 3;

#[derive(Debug, Clone)]
pub struct BPTree {
    root: Option<Node>,
}

impl BPTree {
    pub fn new() -> Self {
        BPTree { root: None }
    }

    pub fn insert(&mut self, key: i32, value: i32) {
        if let Some(root) = &self.root {
            let leaf = root.search_to_leaf(key);
            if key > root.max_index() {
                self.update_maximum(key)
            }
            leaf.borrow_mut().insert(key, value);
            if leaf.borrow().count() > DEGREE {
                // Exceed!!! Spliting the node
                let leafnode = Node::Leaf(leaf);
                let newnode = Node::split_from_node(&leafnode);

                if let Some(newroot) = self.insert_index(&leafnode, newnode) {
                    // A new root has been created, update the tree's root.
                    self.root = Some(newroot);
                }
            }
        } else {
            // This tree haven't have any node yet!!!
            // Let create one!
            let mut newnode = LeafNode::default();
            newnode.insert(key, value);
            self.root = Some(newnode.into());
        }
    }

    /// Return `Some(...)` if a new root node is created,
    /// else return `None`
    fn insert_index(&mut self, node: &Node, newnode: Node) -> Option<Node> {
        if let Some(parent) = newnode.parent() {
            // Inserting into parent
            parent.borrow_mut().insert(node, newnode);
            if parent.borrow().count() > DEGREE {
                let parentnode = Node::Index(parent);
                let newparent = Node::split_from_node(&parentnode);
                self.insert_index(&parentnode, newparent)
            } else {
                None
            }
        } else {
            // Here we reach the root node
            let newroot = Node::new_root_node(node.clone(), newnode);
            Some(newroot)
        }
    }

    pub fn get(&self, key: i32) -> Option<i32> {
        self.search_to_leaf(key)
            .and_then(|leaf| leaf.borrow().get(key))
    }

    pub fn search_key(&self, key: i32) -> Option<Iter<'_>> {
        self.search_to_leaf(key).map(|leaf| {
            let idx = leaf
                .borrow()
                .kvs
                .iter()
                .enumerate()
                .find_map(|(i, (k, _))| if *k >= key { Some(i) } else { None })
                .unwrap();
            Iter {
                _tree: self,
                leaf,
                idx,
            }
        })
    }

    fn search_to_leaf(&self, key: i32) -> Option<Rc<RefCell<LeafNode>>> {
        if let Some(root) = &self.root {
            Some(root.search_to_leaf(key))
        } else {
            None
        }
    }

    fn update_maximum(&mut self, key: i32) {
        if let Some(root) = &self.root {
            let mut cur = root.clone();
            while let Node::Index(inode) = cur {
                let mut node_ref = inode.borrow_mut();
                let (key_ref, child_ref) = node_ref.kvs.last_mut().unwrap();
                if key > *key_ref {
                    *key_ref = key;
                    cur = child_ref.clone();
                } else {
                    break;
                }
            }
        }
    }
}

pub struct Iter<'t> {
    _tree: &'t BPTree,
    leaf: Rc<RefCell<LeafNode>>,
    idx: usize,
}

impl std::iter::Iterator for Iter<'_> {
    type Item = (i32, i32);
    fn next(&mut self) -> Option<Self::Item> {
        while self.idx >= self.leaf.borrow().kvs.len() {
            let next_node = if let Some(next) = &self.leaf.borrow().next_node {
                next.clone()
            } else {
                return None;
            };
            self.leaf = next_node;
            self.idx = 0;
        }
        let result = Some(self.leaf.borrow().kvs[self.idx]);
        self.idx += 1;
        result
    }
}

#[derive(Debug, Clone)]
struct IndexNode {
    kvs: Vec<(i32, Node)>,
    parent: Option<Weak<RefCell<IndexNode>>>,
}

#[derive(Debug, Default, Clone)]
struct LeafNode {
    next_node: Option<Rc<RefCell<LeafNode>>>,
    kvs: Vec<(i32, i32)>,
    parent: Option<Weak<RefCell<IndexNode>>>,
}

#[derive(Debug, Clone)]
enum Node {
    Index(Rc<RefCell<IndexNode>>),
    Leaf(Rc<RefCell<LeafNode>>),
}

impl PartialEq for Node {
    fn eq(&self, other: &Self) -> bool {
        use Node::*;
        match self {
            Index(inode) => {
                if let Index(onode) = other {
                    Rc::ptr_eq(inode, onode)
                } else {
                    false
                }
            }
            Leaf(lnode) => {
                if let Leaf(onode) = other {
                    Rc::ptr_eq(lnode, onode)
                } else {
                    false
                }
            }
        }
    }
}

impl From<IndexNode> for Node {
    fn from(node: IndexNode) -> Node {
        Node::Index(Rc::new(RefCell::new(node)))
    }
}
impl From<LeafNode> for Node {
    fn from(node: LeafNode) -> Node {
        Node::Leaf(Rc::new(RefCell::new(node)))
    }
}

impl Node {
    pub fn new_root_node(left: Node, right: Node) -> Node {
        use Node::*;
        let newroot = Rc::new(RefCell::new(IndexNode::new_root(left, right)));
        for (_, n) in &newroot.borrow().kvs {
            n.set_parent(&newroot);
        }
        Index(newroot)
    }

    pub fn split_from_node(node: &Node) -> Node {
        use Node::*;
        match node {
            Leaf(leaf) => {
                let mut newleaf = LeafNode::split_from_leaf(&mut leaf.borrow_mut());
                newleaf.next_node = leaf.borrow().next_node.clone();
                let newnode = Rc::new(RefCell::new(newleaf));
                leaf.borrow_mut().next_node = Some(newnode.clone());
                Node::Leaf(newnode)
            }
            Index(inode) => {
                let newindex = IndexNode::split_from_index(&mut inode.borrow_mut());
                let newnode = Rc::new(RefCell::new(newindex));
                for (_, n) in &newnode.borrow().kvs {
                    n.set_parent(&newnode);
                }
                Node::Index(newnode)
            }
        }
    }

    #[inline]
    pub fn max_index(&self) -> i32 {
        use Node::*;
        match self {
            Index(inode) => inode.borrow().kvs.last().unwrap().0,
            Leaf(lnode) => lnode.borrow().kvs.last().unwrap().0,
        }
    }

    #[inline]
    pub fn parent(&self) -> Option<Rc<RefCell<IndexNode>>> {
        use Node::*;
        match self {
            Index(inode) => inode.borrow().parent(),
            Leaf(lnode) => lnode.borrow().parent(),
        }
    }

    #[inline]
    fn set_parent(&self, node: &Rc<RefCell<IndexNode>>) {
        use Node::*;
        match self {
            Index(inode) => inode.borrow_mut().parent = Some(Rc::downgrade(node)),
            Leaf(lnode) => lnode.borrow_mut().parent = Some(Rc::downgrade(node)),
        }
    }

    #[inline]
    fn search_to_leaf(&self, key: i32) -> Rc<RefCell<LeafNode>> {
        use Node::*;
        match self {
            Index(inode) => inode.borrow().search_to_leaf(key),
            Leaf(lnode) => lnode.clone(),
        }
    }

    #[inline]
    pub fn count(&self) -> usize {
        use Node::*;
        match self {
            Index(inode) => inode.borrow().count(),
            Leaf(lnode) => lnode.borrow().count(),
        }
    }
}

impl IndexNode {
    fn new_root(left: Node, right: Node) -> Self {
        let mut kvs = Vec::with_capacity(DEGREE + 1);
        kvs.push((left.max_index(), left));
        kvs.push((right.max_index(), right));
        Self { kvs, parent: None }
    }

    fn split_from_index(node: &mut IndexNode) -> Self {
        let mut kvs = Vec::with_capacity(DEGREE + 1);
        let idx = node.kvs.len() / 2;
        kvs.extend_from_slice(&node.kvs[idx..]);
        node.kvs.truncate(idx);

        IndexNode {
            kvs,
            parent: node.parent.clone(),
        }
    }

    fn search_to_leaf(&self, key: i32) -> Rc<RefCell<LeafNode>> {
        self.search_down(key).search_to_leaf(key)
    }

    fn search_index(&self, key: i32) -> usize {
        match self.kvs.binary_search_by_key(&key, |&(k, _)| k) {
            Ok(i) => i,
            Err(i) => i,
        }
    }

    pub fn parent(&self) -> Option<Rc<RefCell<IndexNode>>> {
        self.parent.as_ref().map(|node| node.upgrade().unwrap())
    }

    fn insert(&mut self, child: &Node, newnode: Node) {
        let (ins, _) = self
            .kvs
            .iter()
            .enumerate()
            .find(|(_, (_, n))| n == child)
            .expect("Cannot find the child in node");
        self.kvs[ins].0 = child.max_index();
        self.kvs.insert(ins + 1, (newnode.max_index(), newnode));
    }

    pub fn search_down(&self, key: i32) -> &Node {
        let idx = self.search_index(key);
        if idx < self.kvs.len() {
            &self.kvs[idx].1
        } else {
            &self.kvs.last().unwrap().1
        }
    }

    pub fn count(&self) -> usize {
        self.kvs.len()
    }
}

impl LeafNode {
    /// This constructor doesn't modify the next_node part
    /// Caller should do it by itself
    fn split_from_leaf(node: &mut LeafNode) -> Self {
        let mut kvs = Vec::with_capacity(DEGREE + 1);
        let idx = node.kvs.len() / 2;
        kvs.extend_from_slice(&node.kvs[idx..]);
        node.kvs.truncate(idx);

        LeafNode {
            kvs,
            parent: node.parent.clone(),
            next_node: None,
        }
    }

    pub fn get(&self, key: i32) -> Option<i32> {
        for &(i, val) in &self.kvs {
            if key == i {
                return Some(val);
            }
        }
        None
    }

    pub fn parent(&self) -> Option<Rc<RefCell<IndexNode>>> {
        self.parent.as_ref().map(|node| node.upgrade().unwrap())
    }

    fn insert_search(&self, key: i32) -> usize {
        match self.kvs.binary_search_by_key(&key, |&(k, _)| k) {
            Ok(i) => i,
            Err(i) => i,
        }
    }

    pub fn insert(&mut self, key: i32, value: i32) {
        let ins = self.insert_search(key);
        self.kvs.insert(ins, (key, value));
    }

    pub fn count(&self) -> usize {
        self.kvs.len()
    }
}
