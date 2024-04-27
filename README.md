# Custom File System

## Overview
Our custom user-level C application facilitates read/write operations on files within a custom file system while preserving their contents and recording all operations in a tree-like data structure representing the system's state.

## Features

### 1. Directory Tree
- **Purpose**: Design a file system that implements versioning and rollback capabilities by journaling file updates.
- **Implementation**: 
    - Upon the first operation, the system reconstructs the previously stored tree, if any, before adding or modifying a node.
    - A spinlock mechanism ensures consistency across concurrent reads/writes to the JSON file storing the directory tree.

### 2. Directory Node Components
- **Basic Structure**:
    - **Children List**: Contains sub-directories.
    - **Files List**: Stores files within the directory.

### 3. File Snapshot Preservation
- **Objective**: Preserve snapshots of file contents before each operation.
- **Implementation**:
    - For every file node, a list of snapshots includes paths to copies of its original contents.
    - Snapshot filenames follow the format "filename_timestamp.txt", where timestamp is the UNIX timestamp of the operation time.
    - Snapshots are stored in unique directories names that are drafted using the SHA256 hash of the file's absolute path to ensure uniqueness.

### 4. Space Optimization
- **Functionality**:
    - A periodic process, named cleanup_daemon, removes all but the most recent snapshot for each file.
    - Ensures space optimization by retaining only the latest snapshot for reference.

## Usage
- **File Operations**: Perform read/write operations on files within the custom file system.
- **Snapshot Preservation**: Automatically capture snapshots of file contents before each operation.
- **Space Optimization**: Periodically remove old snapshots to optimize storage space.


## Dependencies
- **cJSON**: Dependency for parsing and manipulating JSON data in C.
- **Compiler**: Required C compiler to compile the source code.
- **pthreads**: Required library for mutex and spinlocks usage


## Implementation Testing

### Sequential Execution
On sequential execution, we conducted testing using the `demo.c` test suite with the following configurations:
1. **Writes/Reads using our custom library**: `sudo ./demo 1`
2. **Writes/Reads using the default sys calls**: `sudo ./demo 0`

When executed with a random delay between each operation (ranging from 1 to 10 seconds), we observed:
- Consistent snapshot creation for every operation, preserving the content of the file before each execution.
- Consistency between the JSON tree representation and the operation log files, with every operation logged and accounted for in both files.

Even when removing the delay between operations, the above findings remained consistent. The only difference was in snapshot preservation, where snapshots were stored in a "latest operation" fashion based on the timestamp (in seconds' depth). This means operations performed within the same second could overwrite each other's snapshot files.

**Conclusion**: Regardless of the delay between operations, our system successfully maintains a consistent view of executed operations across different files and preserves snapshots based on the WRITE-AHEAD logic.

### Concurrent Execution
For concurrent execution, we utilized the `test.py` test suite, which reads operations from the file `operations.txt` and launches N threads (N = number of operations) to perform those operations on files. We conducted the following tests:
1. 30 concurrent WRITE operations (both WRITE and APPEND) on the same file (`example.txt`) with no delays between threads.
2. 145 concurrent operations in all 8 files in a random order.
3. 145 concurrent operations in all 8 files in a random order with a random delay between each thread execution to ensure correct snapshot creation.

#### Findings:
a) For 30 concurrent WRITE operations on the same file (`example.txt`), all operations were correctly preserved in the `absolute_paths.txt`, logging every operation performed. However, some inconsistencies were observed between the final directory JSON file and the log.txt. This discrepancy may be due to high "write" congestion on the JSON file, resulting in some operations not being completely stored. Nevertheless, users can reconstruct the complete tree using only the `absolute_paths.txt`, which logs every execution performed on the files.

b) Similar findings were observed for 145 concurrent operations in all 8 files. All operations were correctly preserved in the `absolute_paths.txt`, but inconsistencies between the directory JSON file and the log.txt were still present due to write congestion.

c) When introducing a random delay between thread executions, the JSON tree and the operations log became consistent. This suggests that the issue encountered may be related to the locking mechanism implemented to preserve consistency in the directory_tree.json file.

**Conclusion**: Our custom filesystem preserves data through two files: `absolute_paths.txt`, which logs all operations performed on any file, and `directory_tree.json`, which represents the file system's directory structure. Despite some inconsistencies, our system successfully preserves a correct mapping of all executions performed on the files.

### Crash Consistency
We implemented a set of experiments to test crash consistency in cases where a process performing an operation to a file crashes. Our implementation provides a signal handler that manages various signals, including SIGKILL, SIGSEGV, SIGABRT, SIGILL, SIGFPE, SIGTERM, and SIGINT. When such a signal is received, the program offers the rollback option to the user, allowing them to revert to the previous version of the latest file that was operated on. While we did not cover all edge cases due to time constraints, manual and automatic introduction of bugs verified the rollback option's functionality.

