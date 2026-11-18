#!/usr/bin/env bash
# Runs the program with scripted input and checks its output and saved file.
# Usage (from the repository folder): bash tests/run_tests.sh

cd "$(dirname "$0")/.." || exit 1
BASE=$(mktemp -d)
trap 'rm -rf "$BASE"' EXIT
BIN="$BASE/student"
g++ -Wall -Wextra student.cpp -o "$BIN" || { echo "Build failed"; exit 1; }
pass=0
fail=0

# run <students.txt contents or empty for no file> <keyboard input>
# runs the program in a fresh folder; sets OUT (output without colours) and DIR (that folder)
run() {
    DIR=$(mktemp -d -p "$BASE")
    [ -n "$1" ] && printf '%b' "$1" > "$DIR/students.txt"
    OUT=$(cd "$DIR" && printf '%b' "$2" | "$BIN" | sed 's/\x1b\[[0-9;]*[A-Za-z]//g')
}
check() {   # check <test name> <condition result 0/1> <description>
    if [ "$2" -eq 0 ]; then pass=$((pass + 1)); else fail=$((fail + 1)); echo "FAIL: $1 - $3"; fi
}
has()      { grep -qF -- "$1" <<< "$OUT"; echo $?; }
lacks()    { grep -qF -- "$1" <<< "$OUT" && echo 1 || echo 0; }
file_has() { grep -qF -- "$1" "$DIR/students.txt"; echo $?; }

# 1. A bad line in the middle of the file must not lose the records after it
run '101 8.9 Aarav Sharma\nbad line here\n102 7.4 Diya Kulkarni\n' '\n2\n\n0\n'
check "bad line" "$(has 'Line 2 of students.txt is invalid')" "warning for line 2"
check "bad line" "$(has 'Records: 2')" "both valid records loaded"
check "bad line" "$(has 'Diya Kulkarni')" "record after the bad line loaded"
check "bad line" "$(file_has '102 7.4 Diya Kulkarni')" "record after the bad line still saved"
check "bad line" "$(grep -qF 'bad line here' "$DIR/students.txt.bak"; echo $?)" "original kept in students.txt.bak"

# 2. Duplicate roll numbers and out-of-range CGPA in the file are skipped
run '101 8.9 Aarav Sharma\n101 7 Copy Of Aarav\n102 11 Too High\n103 6 Kabir Joshi\n' '\n0\n'
check "file validation" "$(has 'Records: 2')" "only the two valid records loaded"
check "file validation" "$(has 'Line 2 of students.txt is invalid')" "duplicate roll skipped"
check "file validation" "$(has 'Line 3 of students.txt is invalid')" "CGPA 11 skipped"

# 3. A file saved on Windows (CRLF line endings) loads cleanly and is saved without them
run '101 8.9 Aarav Sharma\r\n102 7.4 Diya Kulkarni\r\n' '0\n'
check "CRLF file" "$(has 'Records: 2')" "both records loaded"
check "CRLF file" "$(lacks 'is invalid')" "no lines skipped"
check "CRLF file" "$(grep -q $'\r' "$DIR/students.txt" && echo 1 || echo 0)" "no carriage returns after saving"

# 4. A missing file starts an empty list
run '' '0\n'
check "missing file" "$(has 'Records: 0')" "starts empty"

# 5. Input with extra characters is rejected
run '101 8.9 Aarav Sharma\n' '12abc\n1.5\n0\n'
check "number input" "$( [ "$(grep -cF 'Invalid number, try again.' <<< "$OUT")" -ge 2 ]; echo $?)" "\"12abc\" and \"1.5\" both rejected"

# 6. Roll numbers must be positive
run '' '1\n0\n\n1\n-5\n\n0\n'
check "roll number" "$( [ "$(grep -cF 'Roll number must be a positive number.' <<< "$OUT")" -eq 2 ]; echo $?)" "0 and -5 both rejected"
check "roll number" "$(has 'Records: 0')" "nothing was added"

# 7. Adding a student saves it, and the temporary file is cleaned up
run '' '1\n201\nTara Menon\n8\n1\n\n0\n'
check "save" "$(file_has '201 8 Tara Menon')" "new student saved"
check "save" "$( [ ! -e "$DIR/students.txt.tmp" ]; echo $?)" "no temporary file left behind"

# 8. A rejected update changes nothing
run '104 6.3 Meera Nair\n' '4\n104\nWrong Name\n11\n\n0\n'
check "update" "$(has 'Nothing changed.')" "invalid CGPA rejected"
check "update" "$(file_has '104 6.3 Meera Nair')" "record unchanged"

# 9. Delete and reverse still work
run '101 8.9 Aarav Sharma\n102 7.4 Diya Kulkarni\n103 9.25 Kabir Joshi\n' '5\n102\ny\n\n8\n\n0\n'
check "delete and reverse" "$(has 'Deleted.')" "student deleted"
check "delete and reverse" "$(has '[103 | *] -> [101 | *] -> NULL')" "list reversed without the deleted node"
check "delete and reverse" "$( [ "$(head -n 1 "$DIR/students.txt")" = '103 9.25 Kabir Joshi' ]; echo $?)" "reversed order saved"

# 10. Statistics
run '101 8.9 Aarav Sharma\n102 7.4 Diya Kulkarni\n105 4.6 Rohan Deshmukh\n' '9\n\n0\n'
check "statistics" "$(has 'Average CGPA   : 6.97')" "average"
check "statistics" "$(has 'Passed (>= 5.00): 2   Failed: 1')" "pass and fail counts"

echo "$pass passed, $fail failed"
[ "$fail" -eq 0 ]
