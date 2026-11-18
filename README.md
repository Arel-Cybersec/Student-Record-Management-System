# Student Record Management Using Linked Lists

A menu-driven C++ terminal application that stores student records in a singly linked list. Group 1 project for Data Structures and Algorithms.

## Features

- Add a student at the end, at the beginning or at a given position
- Display all students in a table
- Search by roll number and update a student's details
- Delete a student, with confirmation
- Sort by roll number or by CGPA (highest first)
- Reverse the list in place
- Class statistics: average, highest and lowest CGPA, pass and fail counts
- Draw the list as nodes and pointers in the terminal
- Records are saved to `students.txt` after every action and loaded at start-up

## Build and run

Requires g++ on Linux.

```bash
g++ -Wall student.cpp -o student
./student
```

## Data file

`students.txt` holds one student per line, in the order roll number, CGPA and name:

```
101 8.9 Aarav Sharma
102 7.4 Diya Kulkarni
```

The included file contains sample records for testing.

## How it works

Each student is a node holding a roll number, name, CGPA and a pointer to the next node. The program keeps a `head` pointer to the first node and a `tail` pointer to the last, so inserting at the end is O(1).

| Operation | Time |
|---|---|
| Insert at end or beginning | O(1) |
| Insert at a position | O(n) |
| Search, update, delete | O(n) |
| Reverse | O(n) |
| Sort | O(n²) |
