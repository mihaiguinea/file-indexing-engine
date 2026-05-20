# File Indexing System (Data Structures Project)

A high-performance C implementation of a file indexing and retrieval engine. The system efficiently handles file management and keyword-based search queries by combining three fundamental data structures: a **Doubly Linked List**, a **Trie (Prefix Tree)** with alphabetically sorted sibling lists, and a **Max-Heap**.

## Architecture & Data Structures

### 1. Doubly Linked List (File Storage)
The primary storage for files. It preserves the exact chronological order in which files are added to the system. Each node in this list contains:
* A unique string identifier (`id`)
* An integer relevance `score`
* A singly linked list of unique associated `keywords`
* Pointers to the previous (`prev`) and next (`next`) elements to support efficient removal in $O(1)$ time once the node pointer is located.

### 2. Multi-Way Trie (Keyword Indexing)
A retrieval tree used to index keywords character by character. To optimize memory layout and avoid large fixed-size pointer arrays, children are represented using the **Left-Child Right-Sibling (LCRS)** binary tree representation. 
* Sibling chains are strictly maintained in ascending alphabetical order to guarantee deterministic lexicographical operations.
* Terminal nodes (representing the end of a valid keyword) hold a singly linked list of references (`FileRef`) pointing back to the specific `File` structures in the main list.

### 3. Max-Heap (Priority Queue)
A binary heap structure utilized exclusively for processing top-K queries. The extraction priority is determined dynamically:
* Primary key: The file's relevance `score` (higher scores have priority).
* Secondary key (tie-breaker): Lexicographical ordering of the file `id` (alphabetically smaller IDs have priority).

---

## Core Operations

### File and Keyword Management
* **ADD `<id> <score> <t> <kw1> <kw2> ...`** Validates if the file identifier is unique. If it does not exist, a new file node is appended to the tail of the Doubly Linked List. Each unique keyword provided is dynamically appended to the file's list and mapped into the Trie structure. Duplicate keywords within the same command are automatically ignored.
  
* **DEL `<id>`** Locates the specified file. It traverses all keywords registered under that file, removes their corresponding file references from the terminal nodes of the Trie, and recursively cleans up any empty, redundant trie branches. Finally, the file node is safely unlinked from the Doubly Linked List and its allocated memory is freed.
  
* **ADDKW `<id> <keyword>`** Injects a new keyword into an existing file's entry. It adds the word to the file's internal keyword list and binds a fresh reference to the file inside the Trie, provided the association does not already exist.
  
* **DELKW `<id> <keyword>`** Removes a specific keyword association. It unlinks the file reference from the corresponding terminal node in the Trie (triggering branch cleanups if necessary) and extracts the string from the file's personal keyword collection.

### Search and Output Queries
* **FIND `<keyword>`** Executes an exact string matching search down the Trie. If a terminal node is successfully reached, all indexed file references are extracted into an array, sorted strictly by their ID using `qsort`, and written to the output file along with the match count.
  
* **TOPK `<keyword> <k>`** Retrieves files associated with a precise keyword and dynamically pushes them into a Max-Heap. It extracts up to `k` elements sequentially using the priority constraints, outputting the most relevant file IDs in optimized $O(k \log n)$ time.
  
* **PRINT** Performs a Depth-First Search (DFS) traversal over the Trie structure. Because sibling subtrees are inherently sorted alphabetically during insertion, this operation naturally outputs every successfully indexed keyword and its corresponding file count/IDs in alphabetical order without requiring an explicit external sort step.
  
* **PREFIX `<prefix>`** Traverses downwards along the path matching the specified prefix string. From that point, it recursively visits the entire underlying sub-tree to collect all distinct files associated with any word starting with that prefix. Duplicate file records are automatically filtered out during collection, and final results are sorted lexicographically by ID.
