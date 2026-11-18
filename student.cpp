// Student Record Management using a Singly Linked List
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
using namespace std;

struct Student {
    int roll;
    string name;
    float cgpa;
    Student* next;      // pointer to the next node
};

Student* head = nullptr;    // first node
Student* tail = nullptr;    // last node, makes insertEnd O(1)
const string FILE_NAME = "students.txt";
const float PASS_CGPA = 5.0;    // change to your university's passing CGPA

// ---------- terminal styling (ANSI escape codes) ----------
const string RST = "\033[0m", BOLD = "\033[1m", DIM = "\033[2m";
const string RED = "\033[31m", GRN = "\033[32m", YEL = "\033[33m";
const string MAG = "\033[35m", CYN = "\033[36m";
const int W = 50;   // inner width of the menu box

string rep(string s, int n) {
    string r;
    while (n-- > 0) r += s;
    return r;
}

void clearScreen() { cout << "\033[2J\033[H"; }
void ok(string m)  { cout << GRN << " [+] " << m << RST << "\n"; }
void err(string m) { cout << RED << " [!] " << m << RST << "\n"; }

void pauseScreen() {
    cout << DIM << "\n Press Enter to continue..." << RST;
    string s;
    getline(cin, s);
}

void boxLine(string l, string r) { cout << CYN << " " << l << rep("═", W) << r << RST << "\n"; }
void boxRow(string text, string color = "") {
    cout << CYN << " ║" << RST << color << left << setw(W) << ("  " + text)
         << RST << CYN << "║" << RST << "\n";
}

// ---------- input helpers ----------
// reads a whole line and accepts it only if it is exactly one number, so "12abc" or "1.5" for an int is rejected
template <typename T>
T readNum(string prompt) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) exit(0);           // input closed
        istringstream in(line);
        T x;
        if (in >> x && (in >> ws).eof()) return x;
        err("Invalid number, try again.");
    }
}

bool validCgpa(float c) { return c >= 0 && c <= 10; }

string readLine(string prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    return s;
}

// ---------- linked list operations ----------
Student* search(int roll) {
    for (Student* p = head; p != nullptr; p = p->next)
        if (p->roll == roll) return p;
    return nullptr;
}

// O(1) thanks to the tail pointer
void insertEnd(int roll, string name, float cgpa) {
    Student* s = new Student{roll, name, cgpa, nullptr};
    if (head == nullptr) head = tail = s;
    else {
        tail->next = s;
        tail = s;
    }
}

// O(1)
void insertFront(int roll, string name, float cgpa) {
    Student* s = new Student{roll, name, cgpa, head};
    head = s;
    if (tail == nullptr) tail = s;
}

// pos is 1-based: 1 = front, beyond the end = end. O(n)
void insertAt(int pos, int roll, string name, float cgpa) {
    if (pos <= 1 || head == nullptr) { insertFront(roll, name, cgpa); return; }
    Student* p = head;
    for (int i = 1; i < pos - 1 && p->next != nullptr; i++) p = p->next;
    Student* s = new Student{roll, name, cgpa, p->next};
    p->next = s;
    if (s->next == nullptr) tail = s;
}

bool deleteStudent(int roll) {
    Student* cur = head;
    Student* prev = nullptr;
    while (cur != nullptr && cur->roll != roll) {
        prev = cur;
        cur = cur->next;
    }
    if (cur == nullptr) return false;              // not found
    if (prev == nullptr) head = cur->next;         // deleting head
    else prev->next = cur->next;                   // middle or last
    if (cur == tail) tail = prev;                  // deleted the last node
    delete cur;
    return true;
}

// flips every next pointer. O(n), no extra memory
void reverseList() {
    Student* prev = nullptr;
    Student* cur = head;
    tail = head;                    // old head becomes the new tail
    while (cur != nullptr) {
        Student* next = cur->next;  // remember the rest of the list
        cur->next = prev;           // flip the pointer
        prev = cur;
        cur = next;
    }
    head = prev;                    // old tail becomes the new head
}

int countStudents() {
    int n = 0;
    for (Student* p = head; p != nullptr; p = p->next) n++;
    return n;
}

// swaps data, not nodes; O(n^2)
void sortList(bool byCgpa) {
    for (Student* i = head; i != nullptr; i = i->next)
        for (Student* j = i->next; j != nullptr; j = j->next) {
            bool needSwap = byCgpa ? j->cgpa > i->cgpa : j->roll < i->roll;
            if (needSwap) {
                swap(i->roll, j->roll);
                swap(i->name, j->name);
                swap(i->cgpa, j->cgpa);
            }
        }
}

void freeAll() {
    while (head != nullptr) {
        Student* t = head;
        head = head->next;
        delete t;
    }
    tail = nullptr;
}

// ---------- display ----------
void tableLine(string l, string m, string r) {
    cout << " " << l << rep("─", 10) << m << rep("─", 27) << m << rep("─", 8) << r << "\n";
}

void printHeader() {
    tableLine("┌", "┬", "┐");
    cout << " │ " << BOLD << left << setw(8) << "Roll" << RST
         << " │ " << BOLD << setw(25) << "Name" << RST
         << " │ " << BOLD << setw(6) << "CGPA" << RST << " │\n";
    tableLine("├", "┼", "┤");
}

void printRow(Student* p) {
    string color = p->cgpa >= 8 ? GRN : p->cgpa >= 6 ? YEL : RED;
    cout << " │ " << left << setw(8) << p->roll
         << " │ " << setw(25) << p->name.substr(0, 25)
         << " │ " << color << right << fixed << setprecision(2) << setw(6) << p->cgpa << RST << " │\n";
}

void printFooter() { tableLine("└", "┴", "┘"); }

void display() {
    if (head == nullptr) { err("No records."); return; }
    printHeader();
    for (Student* p = head; p != nullptr; p = p->next) printRow(p);
    printFooter();
}

// draws the nodes and pointers: head -> [101 | *] -> [102 | *] -> NULL
void visualize() {
    cout << " " << MAG << "head" << RST << "\n  |\n  v\n ";
    int i = 0;
    for (Student* p = head; p != nullptr; p = p->next) {
        cout << CYN << "[" << p->roll << " | *]" << RST << " -> ";
        if (++i % 4 == 0 && p->next != nullptr) cout << "\n ";
    }
    cout << RED << "NULL" << RST << "\n";
    if (tail != nullptr) cout << " " << MAG << "tail" << RST << " -> [" << tail->roll << "]\n";
}

// one traversal computes everything. O(n)
void showStats() {
    if (head == nullptr) { err("No records."); return; }
    Student* top = head;
    Student* low = head;
    float sum = 0;
    int n = 0, pass = 0;
    for (Student* p = head; p != nullptr; p = p->next) {
        sum += p->cgpa;
        n++;
        if (p->cgpa >= PASS_CGPA) pass++;
        if (p->cgpa > top->cgpa) top = p;
        if (p->cgpa < low->cgpa) low = p;
    }
    cout << fixed << setprecision(2)
         << " Total students : " << n << "\n"
         << " Average CGPA   : " << sum / n << "\n"
         << " Highest CGPA   : " << GRN << top->cgpa << RST << " (" << top->name << ")\n"
         << " Lowest CGPA    : " << RED << low->cgpa << RST << " (" << low->name << ")\n"
         << " Passed (>= " << PASS_CGPA << "): " << GRN << pass << RST
         << "   Failed: " << RED << n - pass << RST << "\n";
}

void showMenu() {
    string ends = head == nullptr ? "-" : to_string(head->roll) + "  Tail: " + to_string(tail->roll);
    boxLine("╔", "╗");
    boxRow("STUDENT RECORD MANAGEMENT SYSTEM", BOLD + YEL);
    boxRow("Singly Linked List | DSA Project - Group 1", DIM);
    boxLine("╠", "╣");
    boxRow("Records: " + to_string(countStudents()) + "    Head: " + ends, GRN);
    boxLine("╠", "╣");
    boxRow("[1]  Add student");
    boxRow("[2]  Display all students");
    boxRow("[3]  Search by roll no");
    boxRow("[4]  Update student");
    boxRow("[5]  Delete student");
    boxRow("[6]  Sort by roll no");
    boxRow("[7]  Sort by CGPA (high to low)");
    boxRow("[8]  Reverse list");
    boxRow("[9]  Statistics");
    boxRow("[10] Visualize linked list");
    boxRow("[0]  Save & Exit", RED);
    boxLine("╚", "╝");
}

// ---------- file handling ----------
// line format: roll cgpa name   (name last so it can contain spaces)
// writes a temporary file first and then swaps it in, so a failed write never destroys the old file
bool save() {
    string tmp = FILE_NAME + ".tmp";
    ofstream out(tmp);
    for (Student* p = head; p != nullptr; p = p->next)
        out << p->roll << " " << p->cgpa << " " << p->name << "\n";
    out.close();
    bool saved = out.good();
    if (saved && rename(tmp.c_str(), FILE_NAME.c_str()) != 0) {
        remove(FILE_NAME.c_str());                  // Windows cannot rename over an existing file
        saved = rename(tmp.c_str(), FILE_NAME.c_str()) == 0;
    }
    if (!saved) {
        remove(tmp.c_str());
        err("Could not save to " + FILE_NAME + ". Changes are only in memory.");
    }
    return saved;
}

// skips invalid lines instead of stopping, and keeps a copy of the original file if any were skipped
// returns the number of skipped lines
int load() {
    ifstream in(FILE_NAME);
    string line;
    int lineNo = 0, skipped = 0;
    while (getline(in, line)) {
        lineNo++;
        if (!line.empty() && line.back() == '\r') line.pop_back();     // file saved on Windows
        if (line.find_first_not_of(" \t") == string::npos) continue;   // blank line
        istringstream ls(line);
        int roll;
        float cgpa;
        string name;
        bool valid = ls >> roll >> cgpa && getline(ls >> ws, name)
                     && roll > 0 && validCgpa(cgpa) && search(roll) == nullptr;
        if (valid) insertEnd(roll, name, cgpa);
        else {
            err("Line " + to_string(lineNo) + " of " + FILE_NAME + " is invalid and was skipped.");
            skipped++;
        }
    }
    if (skipped > 0) {
        in.clear();
        in.seekg(0);
        ofstream backup(FILE_NAME + ".bak");
        backup << in.rdbuf();
        err("The original file was copied to " + FILE_NAME + ".bak before any changes.");
    }
    return skipped;
}

// ---------- menu actions ----------
void addStudent() {
    int roll = readNum<int>(" Roll no      : ");
    if (roll <= 0) { err("Roll number must be a positive number."); return; }
    if (search(roll) != nullptr) { err("Roll " + to_string(roll) + " already exists."); return; }
    string name = readLine(" Name         : ");
    if (name.empty()) { err("Name cannot be empty."); return; }
    float cgpa = readNum<float>(" CGPA (0-10)  : ");
    if (!validCgpa(cgpa)) { err("CGPA must be between 0 and 10."); return; }

    cout << "\n Insert where?  [1] End   [2] Beginning   [3] Position\n";
    switch (readNum<int>(" Choice: ")) {
        case 1: insertEnd(roll, name, cgpa); break;
        case 2: insertFront(roll, name, cgpa); break;
        case 3: {
            int pos = readNum<int>(" Position (1 to " + to_string(countStudents() + 1) + "): ");
            if (pos < 1) { err("Position must be 1 or more."); return; }
            insertAt(pos, roll, name, cgpa);
            break;
        }
        default: err("Invalid choice, student not added."); return;
    }
    ok("Student added.");
}

void searchStudent() {
    Student* s = search(readNum<int>(" Roll no: "));
    if (s == nullptr) { err("Not found."); return; }
    printHeader();
    printRow(s);
    printFooter();
}

void updateStudent() {
    Student* s = search(readNum<int>(" Roll no to update: "));
    if (s == nullptr) { err("Not found."); return; }
    string name = readLine(" New name (Enter to keep '" + s->name + "'): ");
    float cgpa = readNum<float>(" New CGPA (0-10): ");
    if (!validCgpa(cgpa)) { err("CGPA must be between 0 and 10. Nothing changed."); return; }
    if (!name.empty()) s->name = name;     // change only after all input is valid
    s->cgpa = cgpa;
    ok("Updated.");
}

void removeStudent() {
    int roll = readNum<int>(" Roll no to delete: ");
    Student* s = search(roll);
    if (s == nullptr) { err("Not found."); return; }
    string confirm = readLine(" Delete " + s->name + "? (y/n): ");
    if (confirm != "y" && confirm != "Y") { err("Cancelled."); return; }
    deleteStudent(roll);
    ok("Deleted.");
}

int main() {
    if (load() > 0) pauseScreen();      // let the user read the warnings before the screen clears
    while (true) {
        clearScreen();
        showMenu();
        int choice = readNum<int>(" " + YEL + "> " + RST + "Choice: ");
        cout << "\n";
        switch (choice) {
            case 1: addStudent(); break;
            case 2: display(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: removeStudent(); break;
            case 6: sortList(false); ok("Sorted by roll no."); display(); break;
            case 7: sortList(true); ok("Sorted by CGPA."); display(); break;
            case 8:
                cout << " Before:\n"; visualize();
                reverseList();
                cout << "\n After:\n"; visualize();
                ok("List reversed.");
                break;
            case 9: showStats(); break;
            case 10: visualize(); break;
            case 0:
                if (!save()) break;         // stay open so the data in memory is not lost
                freeAll();
                ok("Saved. Goodbye!");
                return 0;
            default: err("Invalid choice.");
        }
        save();
        pauseScreen();
    }
}
