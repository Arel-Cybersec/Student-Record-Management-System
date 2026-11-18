# Student Record Management System

![Language](https://img.shields.io/badge/language-C%2B%2B-00599C)
![Platform](https://img.shields.io/badge/platform-Linux-FCC624)
![License](https://img.shields.io/badge/license-MIT-green)

A menu-driven C++ terminal application that manages student records using a **singly linked list**. Every record is a node connected to the next by a pointer, and every feature, from inserting and deleting to sorting and reversing, works directly on those pointers.

Built as the Group 1 project for the **Data Structures and Algorithms** course.

---

## Contents

- [Features](#features)
- [Preview](#preview)
- [Getting started](#getting-started)
- [Usage](#usage)
- [How it works](#how-it-works)
- [Time complexity](#time-complexity)
- [Input validation](#input-validation)
- [Project structure](#project-structure)
- [Team](#team)
- [License](#license)

---

## Features

- **Three ways to insert:** at the end, at the beginning, or at any position
- **Search** a student by roll number
- **Update** a student's name and CGPA
- **Delete** a student, with a confirmation prompt
- **Sort** by roll number, or by CGPA from highest to lowest
- **Reverse** the list in place, showing it before and after
- **Statistics:** total students, average, highest and lowest CGPA, pass and fail counts
- **List visualizer** that draws the nodes and pointers in the terminal
- **Automatic saving** to `students.txt` after every action, and loading at start-up
- **Colour-coded terminal interface** using ANSI escape codes, with no extra libraries
- **Safe input handling:** invalid entries never crash the program or corrupt the list

---

## Preview

### Main menu

The header shows the record count and where `head` and `tail` point, updated after every action.

```
 ╔══════════════════════════════════════════════════╗
 ║  STUDENT RECORD MANAGEMENT SYSTEM                ║
 ║  Singly Linked List | DSA Project - Group 1      ║
 ╠══════════════════════════════════════════════════╣
 ║  Records: 5    Head: 101  Tail: 105              ║
 ╠══════════════════════════════════════════════════╣
 ║  [1]  Add student                                ║
 ║  [2]  Display all students                       ║
 ║  [3]  Search by roll no                          ║
 ║  [4]  Update student                             ║
 ║  [5]  Delete student                             ║
 ║  [6]  Sort by roll no                            ║
 ║  [7]  Sort by CGPA (high to low)                 ║
 ║  [8]  Reverse list                               ║
 ║  [9]  Statistics                                 ║
 ║  [10] Visualize linked list                      ║
 ║  [0]  Save & Exit                                ║
 ╚══════════════════════════════════════════════════╝
 > Choice:
```

### Displaying records

In the terminal, CGPA is coloured green for 8 and above, yellow for 6 and above, and red below 6.

```
 ┌──────────┬───────────────────────────┬────────┐
 │ Roll     │ Name                      │ CGPA   │
 ├──────────┼───────────────────────────┼────────┤
 │ 101      │ Aarav Sharma              │   8.90 │
 │ 102      │ Diya Kulkarni             │   7.40 │
 │ 103      │ Kabir Joshi               │   9.25 │
 │ 104      │ Meera Nair                │   6.30 │
 │ 105      │ Rohan Deshmukh            │   4.60 │
 └──────────┴───────────────────────────┴────────┘
```

### Reversing the list

```
 Before:
 head
  |
  v
 [101 | *] -> [102 | *] -> [103 | *] -> [104 | *] ->
 [105 | *] -> NULL
 tail -> [105]

 After:
 head
  |
  v
 [105 | *] -> [104 | *] -> [103 | *] -> [102 | *] ->
 [101 | *] -> NULL
 tail -> [101]
 [+] List reversed.
```

### Statistics

```
 Total students : 5
 Average CGPA   : 7.29
 Highest CGPA   : 9.25 (Kabir Joshi)
 Lowest CGPA    : 4.60 (Rohan Deshmukh)
 Passed (>= 5.00): 4   Failed: 1
```

---

## Getting started

### Requirements

- Linux (developed on Kali Linux)
- g++ with C++11 or later
- A terminal that supports UTF-8 and ANSI colours (any modern Linux terminal does)

### Build and run

```bash
git clone https://github.com/Arel-Cybersec/Student-Record-Management-System.git
cd Student-Record-Management-System
g++ -Wall student.cpp -o student
./student
```

The repository includes a `students.txt` file with five sample records, so the program starts with data to try out. Delete it to start with an empty list.

---

## Usage

| Option | Action |
|:---:|---|
| `1` | Add a student, then choose where to insert: end, beginning or a position |
| `2` | Display all students in a table |
| `3` | Search for a student by roll number |
| `4` | Update a student's name and CGPA. Press Enter to keep the current name |
| `5` | Delete a student by roll number, after typing `y` to confirm |
| `6` | Sort by roll number, ascending |
| `7` | Sort by CGPA, highest first |
| `8` | Reverse the list and show it before and after |
| `9` | Show class statistics |
| `10` | Draw the list as nodes and pointers |
| `0` | Save, free all memory and exit |

### Data file

Records are stored in `students.txt`, one student per line, in the order roll number, CGPA and name:

```
101 8.9 Aarav Sharma
102 7.4 Diya Kulkarni
103 9.25 Kabir Joshi
```

The name comes last so that it can contain spaces. The file is rewritten after every action, so closing the terminal by accident does not lose data.

---

## How it works

### The node

```cpp
struct Student {
    int roll;
    string name;
    float cgpa;
    Student* next;      // pointer to the next node
};

Student* head = nullptr;    // first node
Student* tail = nullptr;    // last node, makes insertEnd O(1)
```

```
 head                                      tail
  |                                         |
  v                                         v
 [101 | Aarav | 8.90 | *] -> [102 | ... | *] -> [103 | ... | *] -> NULL
```

Every node is created with `new` when a student is added and released with `delete` when the student is removed. On exit, `freeAll()` deletes every remaining node, so no memory is leaked.

### Insert at a position

The program walks to the node **before** the target position, then links the new node in two steps. The order matters: setting `p->next` first would lose the rest of the list.

```cpp
Student* s = new Student{roll, name, cgpa, p->next};  // 1. new node points to p's next
p->next = s;                                         // 2. p points to the new node
```

### Delete

Two pointers walk together. `cur` checks each node while `prev` stays one step behind, because a singly linked node cannot see the node before it.

```cpp
if (prev == nullptr) head = cur->next;   // deleting the first node
else prev->next = cur->next;             // deleting a middle or last node
if (cur == tail) tail = prev;            // deleted the last node
delete cur;
```

### Reverse

Three pointers flip every `next` link in a single pass, without creating or copying any node.

```cpp
while (cur != nullptr) {
    Student* next = cur->next;  // remember the rest of the list
    cur->next = prev;           // flip the pointer
    prev = cur;
    cur = next;
}
head = prev;
```

### Sort

Sorting swaps the **data** inside nodes and leaves the links untouched, so `head` and `tail` always stay valid.

---

## Time complexity

| Operation | Time | Reason |
|---|:---:|---|
| Insert at end | O(1) | The `tail` pointer gives direct access to the last node |
| Insert at beginning | O(1) | Only `head` and one link change |
| Insert at position | O(n) | Walks to the node before the position |
| Search, update | O(n) | Checks nodes one by one from `head` |
| Delete | O(n) | Must find the node and its predecessor |
| Reverse | O(n) | Flips each pointer once, using O(1) extra memory |
| Sort | O(n²) | Compares every pair of nodes |
| Display, statistics, save, load | O(n) | One pass over the list or the file |

Space complexity is O(n): one node per student.

---

## Input validation

| Input | Result |
|---|---|
| Letters where a number is expected | `Invalid number, try again.` and the prompt repeats |
| Unknown menu option | `Invalid choice.` |
| Roll number that already exists | Rejected, and the message names the roll number |
| Empty name | `Name cannot be empty.` |
| CGPA outside 0 to 10 | `CGPA must be between 0 and 10.` and the record is left unchanged |
| Roll number not in the list | `Not found.` |
| Delete not confirmed with `y` | `Cancelled.` |

---

## Project structure

```
Student-Record-Management-System/
├── student.cpp     # the complete program
├── students.txt    # sample records, loaded at start-up
├── README.md
└── LICENSE
```

Inside `student.cpp`:

| Part | Functions |
|---|---|
| Linked list operations | `insertEnd`, `insertFront`, `insertAt`, `search`, `deleteStudent`, `reverseList`, `sortList`, `countStudents`, `freeAll` |
| Display | `display`, `printHeader`, `printRow`, `printFooter`, `visualize`, `showStats`, `showMenu` |
| File handling | `save`, `load` |
| Input | `readNum`, `readLine` |
| Menu actions | `addStudent`, `searchStudent`, `updateStudent`, `removeStudent`, `main` |

---

## Team

**Group 1, Division D1**

| Name | GitHub |
|---|---|
| Aditya Santosh Adhikar | [@adityaadhikar](https://github.com/adityaadhikar) |
| Nkanyiso Antonio Nkiwane | |
| Arsh Patel | [@Arel-Cybersec](https://github.com/Arel-Cybersec) |
| Avishkar Bhausaheb Kadarkar | |

**Course:** Data Structures and Algorithms
**Subject in-charge:** Prof. Sonali Patil

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
