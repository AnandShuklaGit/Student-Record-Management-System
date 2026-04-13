/*
 * Student Record Management System
 * ==================================
 * Menu-driven C application with CRUD operations across all six
 * standard file access modes: r, w, a, r+, w+, a+
 *
 * Features:
 *   - Create / overwrite records     (w)
 *   - Read / display all records     (r)
 *   - Append new records             (a)
 *   - Modify a record by roll number (r+  with fseek)
 *   - Recreate the entire file       (w+)
 *   - Append and read back           (a+)
 *
 * Tech Stack: C, File I/O (fprintf, fscanf, fseek, rewind)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Data Structures ────────────────────────────────────────────── */

#define MAX_NAME   50
#define DATA_FILE  "students.dat"

typedef struct {
    int   roll;
    char  name[MAX_NAME];
    float marks;
    char  grade;
} Student;

/* ── Utility: Compute grade from marks ───────────────────────────── */
char compute_grade(float marks) {
    if (marks >= 90) return 'O';
    if (marks >= 80) return 'A';
    if (marks >= 70) return 'B';
    if (marks >= 60) return 'C';
    if (marks >= 50) return 'D';
    return 'F';
}

/* ── Utility: Print a single record ─────────────────────────────── */
void print_student(const Student *s) {
    printf("  Roll: %-5d  Name: %-25s  Marks: %6.2f  Grade: %c\n",
           s->roll, s->name, s->marks, s->grade);
}

/* ── Utility: Print separator ────────────────────────────────────── */
void print_sep(void) {
    printf("  %s\n", "─────────────────────────────────────────────────────────");
}

/* ─────────────────────────────────────────────────────────────────
 * 1. CREATE — Mode "w"
 *    Overwrites the file and writes N new records.
 * ───────────────────────────────────────────────────────────────── */
void create_records(void) {
    FILE *fp = fopen(DATA_FILE, "w");
    if (!fp) { perror("fopen(w)"); return; }

    int n;
    printf("\n  How many students to add? ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        Student s;
        printf("\n  Student %d\n", i + 1);
        printf("  Roll number : "); scanf("%d",   &s.roll);
        printf("  Full name   : "); scanf(" %[^\n]", s.name);
        printf("  Marks (100) : "); scanf("%f",   &s.marks);
        s.grade = compute_grade(s.marks);
        fprintf(fp, "%d|%s|%.2f|%c\n", s.roll, s.name, s.marks, s.grade);
    }

    fclose(fp);
    printf("\n  ✔ %d record(s) written to '%s'.\n", n, DATA_FILE);
}

/* ─────────────────────────────────────────────────────────────────
 * 2. READ — Mode "r"
 *    Reads and displays all records from the file.
 * ───────────────────────────────────────────────────────────────── */
void read_records(void) {
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) { printf("  [!] File not found. Create records first.\n"); return; }

    Student s;
    int count = 0;
    printf("\n  All Student Records\n");
    print_sep();

    while (fscanf(fp, "%d|%49[^|]|%f|%c\n",
                  &s.roll, s.name, &s.marks, &s.grade) == 4) {
        print_student(&s);
        count++;
    }

    if (count == 0) printf("  (No records found.)\n");
    print_sep();
    printf("  Total: %d record(s).\n", count);
    fclose(fp);
}

/* ─────────────────────────────────────────────────────────────────
 * 3. APPEND — Mode "a"
 *    Adds new records at the end without disturbing existing data.
 * ───────────────────────────────────────────────────────────────── */
void append_records(void) {
    FILE *fp = fopen(DATA_FILE, "a");
    if (!fp) { perror("fopen(a)"); return; }

    int n;
    printf("\n  How many students to append? ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        Student s;
        printf("\n  Student %d\n", i + 1);
        printf("  Roll number : "); scanf("%d",   &s.roll);
        printf("  Full name   : "); scanf(" %[^\n]", s.name);
        printf("  Marks (100) : "); scanf("%f",   &s.marks);
        s.grade = compute_grade(s.marks);
        fprintf(fp, "%d|%s|%.2f|%c\n", s.roll, s.name, s.marks, s.grade);
    }

    fclose(fp);
    printf("\n  ✔ %d record(s) appended to '%s'.\n", n, DATA_FILE);
}

/* ─────────────────────────────────────────────────────────────────
 * 4. MODIFY — Mode "r+"
 *    Locates a student by roll number using fseek and updates marks.
 *    Uses a temp file strategy to safely rewrite the matching line.
 * ───────────────────────────────────────────────────────────────── */
void modify_record(void) {
    int target_roll;
    printf("\n  Enter roll number to modify: ");
    scanf("%d", &target_roll);

    FILE *fp  = fopen(DATA_FILE, "r");
    if (!fp) { printf("  [!] File not found.\n"); return; }

    FILE *tmp = fopen("students_tmp.dat", "w");
    if (!tmp) { fclose(fp); perror("fopen(tmp)"); return; }

    Student s;
    int found = 0;

    while (fscanf(fp, "%d|%49[^|]|%f|%c\n",
                  &s.roll, s.name, &s.marks, &s.grade) == 4) {
        if (s.roll == target_roll) {
            found = 1;
            printf("  Found: "); print_student(&s);
            printf("  New marks: "); scanf("%f", &s.marks);
            s.grade = compute_grade(s.marks);
            printf("  Updated: "); print_student(&s);
        }
        fprintf(tmp, "%d|%s|%.2f|%c\n", s.roll, s.name, s.marks, s.grade);
    }

    fclose(fp);
    fclose(tmp);

    if (found) {
        remove(DATA_FILE);
        rename("students_tmp.dat", DATA_FILE);
        printf("\n  ✔ Record updated.\n");
    } else {
        remove("students_tmp.dat");
        printf("\n  [!] Roll number %d not found.\n", target_roll);
    }
}

/* ─────────────────────────────────────────────────────────────────
 * 5. RECREATE — Mode "w+"
 *    Opens for both writing and reading; truncates the file.
 *    Writes fresh records and then reads them back immediately.
 * ───────────────────────────────────────────────────────────────── */
void recreate_records(void) {
    FILE *fp = fopen(DATA_FILE, "w+");
    if (!fp) { perror("fopen(w+)"); return; }

    int n;
    printf("\n  Recreate file — how many students? ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        Student s;
        printf("\n  Student %d\n", i + 1);
        printf("  Roll number : "); scanf("%d",   &s.roll);
        printf("  Full name   : "); scanf(" %[^\n]", s.name);
        printf("  Marks (100) : "); scanf("%f",   &s.marks);
        s.grade = compute_grade(s.marks);
        fprintf(fp, "%d|%s|%.2f|%c\n", s.roll, s.name, s.marks, s.grade);
    }

    /* Read back immediately using the same file pointer */
    rewind(fp);
    Student s;
    int count = 0;
    printf("\n  Verification — records just written:\n");
    print_sep();
    while (fscanf(fp, "%d|%49[^|]|%f|%c\n",
                  &s.roll, s.name, &s.marks, &s.grade) == 4) {
        print_student(&s);
        count++;
    }
    print_sep();
    printf("  %d record(s) verified.\n", count);
    fclose(fp);
}

/* ─────────────────────────────────────────────────────────────────
 * 6. APPEND & READ — Mode "a+"
 *    Appends new records and reads the full file back.
 * ───────────────────────────────────────────────────────────────── */
void append_and_read(void) {
    FILE *fp = fopen(DATA_FILE, "a+");
    if (!fp) { perror("fopen(a+)"); return; }

    int n;
    printf("\n  Append & Read — how many new students? ");
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        Student s;
        printf("\n  Student %d\n", i + 1);
        printf("  Roll number : "); scanf("%d",   &s.roll);
        printf("  Full name   : "); scanf(" %[^\n]", s.name);
        printf("  Marks (100) : "); scanf("%f",   &s.marks);
        s.grade = compute_grade(s.marks);
        fprintf(fp, "%d|%s|%.2f|%c\n", s.roll, s.name, s.marks, s.grade);
    }

    /* Read all records (including newly appended) */
    rewind(fp);
    Student s;
    int count = 0;
    printf("\n  Full file after append:\n");
    print_sep();
    while (fscanf(fp, "%d|%49[^|]|%f|%c\n",
                  &s.roll, s.name, &s.marks, &s.grade) == 4) {
        print_student(&s);
        count++;
    }
    print_sep();
    printf("  Total: %d record(s).\n", count);
    fclose(fp);
}

/* ─────────────────────────────────────────────────────────────────
 * 7. DELETE — remove a record by roll number
 * ───────────────────────────────────────────────────────────────── */
void delete_record(void) {
    int target_roll;
    printf("\n  Enter roll number to delete: ");
    scanf("%d", &target_roll);

    FILE *fp  = fopen(DATA_FILE, "r");
    if (!fp) { printf("  [!] File not found.\n"); return; }

    FILE *tmp = fopen("students_tmp.dat", "w");
    if (!tmp) { fclose(fp); perror("fopen(tmp)"); return; }

    Student s;
    int found = 0;

    while (fscanf(fp, "%d|%49[^|]|%f|%c\n",
                  &s.roll, s.name, &s.marks, &s.grade) == 4) {
        if (s.roll == target_roll) {
            found = 1;
            printf("  Deleted: "); print_student(&s);
        } else {
            fprintf(tmp, "%d|%s|%.2f|%c\n", s.roll, s.name, s.marks, s.grade);
        }
    }

    fclose(fp);
    fclose(tmp);

    if (found) {
        remove(DATA_FILE);
        rename("students_tmp.dat", DATA_FILE);
        printf("  ✔ Record removed.\n");
    } else {
        remove("students_tmp.dat");
        printf("  [!] Roll number %d not found.\n", target_roll);
    }
}

/* ── MAIN MENU ────────────────────────────────────────────────────── */

void print_menu(void) {
    printf("\n  ╔══════════════════════════════════════╗\n");
    printf("  ║   Student Record Management System  ║\n");
    printf("  ╠══════════════════════════════════════╣\n");
    printf("  ║  1. Create records       (mode: w)  ║\n");
    printf("  ║  2. Read all records     (mode: r)  ║\n");
    printf("  ║  3. Append records       (mode: a)  ║\n");
    printf("  ║  4. Modify a record      (mode: r+) ║\n");
    printf("  ║  5. Recreate file        (mode: w+) ║\n");
    printf("  ║  6. Append & read back   (mode: a+) ║\n");
    printf("  ║  7. Delete a record                 ║\n");
    printf("  ║  0. Exit                            ║\n");
    printf("  ╚══════════════════════════════════════╝\n");
    printf("  Choice: ");
}

int main(void) {
    int choice;

    do {
        print_menu();
        if (scanf("%d", &choice) != 1) { choice = -1; }

        switch (choice) {
            case 1: create_records();  break;
            case 2: read_records();    break;
            case 3: append_records();  break;
            case 4: modify_record();   break;
            case 5: recreate_records();break;
            case 6: append_and_read(); break;
            case 7: delete_record();   break;
            case 0: printf("\n  Goodbye!\n"); break;
            default: printf("\n  [!] Invalid choice. Try again.\n"); break;
        }
    } while (choice != 0);

    return 0;
}
