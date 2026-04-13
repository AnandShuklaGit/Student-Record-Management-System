# Student Record Management System

A menu-driven C application with CRUD operations across all six standard file access modes.

## Overview
Demonstrates practical use of every C file I/O mode in a single unified application. Student records are stored in a pipe-delimited text file and can be created, read, appended, modified, or deleted through a terminal menu.

## Features
| Menu Option | File Mode | Operation |
|-------------|-----------|-----------|
| Create records | `w` | Overwrite file, write N new records |
| Read all records | `r` | Display all stored students |
| Append records | `a` | Add new records without disturbing existing data |
| Modify a record | `r+` | Update marks by roll number using `fseek` |
| Recreate file | `w+` | Truncate, write fresh records, read back immediately |
| Append & read | `a+` | Append new records and display the full file |
| Delete a record | temp file | Remove a student by roll number |

## Tech Stack
C, File I/O — `fprintf`, `fscanf`, `fseek`, `rewind`

## Build & Run
No external dependencies — requires only a standard C compiler (GCC or Clang).

## File Format
Records are stored as pipe-delimited lines in `students.dat`:
