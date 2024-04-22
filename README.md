# Custom File System

## Overview
Our custom user-level C application facilitates read/write operations on files within a custom file system while preserving their contents and recording all operations in a tree-like data structure representing the system's state. This README provides an overview of our implementation and its key features.

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
    - Snapshots are stored in unique directories named using the SHA256 hash of the file's absolute path to ensure uniqueness.

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
