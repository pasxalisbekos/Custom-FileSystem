# Custom File System

## Overview
Our custom user-level C application facilitates read/write operations on files within a custom file system while preserving their contents and recording all operations in a tree-like data structure representing the system's state.
The current implementation was tested in UNIX based systems operating Ubuntu 22.04. Additional packages required:
- **pthreads**: sudo apt-get install libpthread-stubs0-dev
- **cJSON library**: sudo apt install libjson-c-dev


Please make sure you create a directory named "snapshots" on the /home directory where all snapshot directories and log files will be stored. Make sure to run the provided code with sudo permissions because it requires write acfess to files located at the "/home" (read-only) directory. 


Usage: A user can build any C application using our customized read/write with only applying modifications on their Makefile to link the library to their code.
In order to use the library affectively:
- **Initiallize handler**: In order to initiallize the signal handler in the main() function call "handler_init()"
- **Completion of execution**: To generate the JSON log file after completion of execution call the "end()" function





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


**Summary**: Our custom filesystem preserves data through two files: `absolute_paths.txt`, which logs all operations performed on any file, and `directory_tree.json`, which represents the file system's directory structure. Despite some inconsistencies, our system successfully preserves a correct mapping of all executions performed on the files.

### Crash Consistency
We implemented a set of experiments to test crash consistency in cases where a process performing an operation to a file crashes. Our implementation provides a signal handler that manages various signals, including SIGKILL, SIGSEGV, SIGABRT, SIGILL, SIGFPE, SIGTERM, and SIGINT. When such a signal is received, the program offers the rollback option to the user, allowing them to revert to the previous version of the latest file that was operated on. While we did not cover all edge cases due to time constraints, manual and automatic introduction of bugs verified the rollback option's functionality.

