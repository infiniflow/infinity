A simplified C++ reimplementing of [BurntSushi/fst](https://github.com/BurntSushi/fst) library. The FST file format should be binary compatible with the original one. 

This library is for efficiently storing and searching ordered sets or maps where the keys are byte strings. A key design goal of this crate is to support storing and searching *very large* sets or maps (i.e., billions). This means that much effort has gone in to making sure that all operations are memory efficient.

Sets and maps are represented by a finite state machine, which acts as a form of compression on common prefixes and suffixes in the keys. Additionally, finite state machines can be efficiently queried with automata (like regular expressions or Levenshtein distance for fuzzy queries) or lexicographic ranges.
