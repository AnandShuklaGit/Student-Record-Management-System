/*
 * Student Record Management System (compact version)
 * File modes covered: w, r, a, r+, w+, a+  |  plus delete
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME  50
#define DATA_FILE "students.dat"
#define TMP_FILE  "students_tmp.dat"

typedef struct { int roll; char name[MAX_NAME]; float marks; char grade; } Student;

char compute_grade(float m) {
    return m>=90?'O': m>=80?'A': m>=70?'B': m>=60?'C': m>=50?'D':'F';
}

void print_student(Student *s) {
    printf("  Roll:%-5d Name:%-25s Marks:%6.2f Grade:%c\n", s->roll, s->name, s->marks, s->grade);
}

/* Prompt for N students, write each to fp. Returns count written. */
int add_students(FILE *fp) {
    int n; Student s;
    printf("\n  How many students? "); scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        printf("  Roll: ");  scanf("%d", &s.roll);
        printf("  Name: ");  scanf(" %[^\n]", s.name);
        printf("  Marks: "); scanf("%f", &s.marks);
        s.grade = compute_grade(s.marks);
        fprintf(fp, "%d|%s|%.2f|%c\n", s.roll, s.name, s.marks, s.grade);
    }
    return n;
}

/* Read and print all records from fp. Returns count. */
int show_all(FILE *fp) {
    Student s; int count = 0;
    while (fscanf(fp, "%d|%49[^|]|%f|%c\n", &s.roll, s.name, &s.marks, &s.grade) == 4) {
        print_student(&s); count++;
    }
    if (!count) printf("  (No records)\n");
    printf("  Total: %d\n", count);
    return count;
}

void create_records(void)   { FILE *fp=fopen(DATA_FILE,"w");  if(!fp){perror("w");return;}  add_students(fp); fclose(fp); }
void read_records(void)     { FILE *fp=fopen(DATA_FILE,"r");  if(!fp){printf("  [!] No file.\n");return;} show_all(fp); fclose(fp); }
void append_records(void)   { FILE *fp=fopen(DATA_FILE,"a");  if(!fp){perror("a");return;}  add_students(fp); fclose(fp); }
void recreate_records(void) { FILE *fp=fopen(DATA_FILE,"w+"); if(!fp){perror("w+");return;} add_students(fp); rewind(fp); show_all(fp); fclose(fp); }
void append_and_read(void)  { FILE *fp=fopen(DATA_FILE,"a+"); if(!fp){perror("a+");return;} add_students(fp); rewind(fp); show_all(fp); fclose(fp); }

/* Shared logic for modify (r+) and delete: rewrite file via temp,
 * acting on the matching roll number. is_delete=0 -> modify marks, 1 -> drop record. */
void edit_record(int is_delete) {
    int roll; printf("\n  Roll number: "); scanf("%d", &roll);
    FILE *fp = fopen(DATA_FILE, "r");
    if (!fp) { printf("  [!] No file.\n"); return; }
    FILE *tmp = fopen(TMP_FILE, "w");
    if (!tmp) { fclose(fp); perror("tmp"); return; }

    Student s; int found = 0;
    while (fscanf(fp, "%d|%49[^|]|%f|%c\n", &s.roll, s.name, &s.marks, &s.grade) == 4) {
        if (s.roll == roll) {
            found = 1;
            if (is_delete) { printf("  Deleted: "); print_student(&s); continue; }
            printf("  Found: "); print_student(&s);
            printf("  New marks: "); scanf("%f", &s.marks);
            s.grade = compute_grade(s.marks);
        }
        fprintf(tmp, "%d|%s|%.2f|%c\n", s.roll, s.name, s.marks, s.grade);
    }
    fclose(fp); fclose(tmp);

    if (found) { remove(DATA_FILE); rename(TMP_FILE, DATA_FILE); printf("  \u2714 Done.\n"); }
    else       { remove(TMP_FILE);  printf("  [!] Roll %d not found.\n", roll); }
}

int main(void) {
    int choice;
    const char *menu =
        "\n  1.Create(w) 2.Read(r) 3.Append(a) 4.Modify(r+)\n"
        "  5.Recreate(w+) 6.Append+Read(a+) 7.Delete 0.Exit\n  Choice: ";
    do {
        printf("%s", menu);
        if (scanf("%d", &choice) != 1) choice = -1;
        switch (choice) {
            case 1: create_records();   break;
            case 2: read_records();     break;
            case 3: append_records();   break;
            case 4: edit_record(0);     break;
            case 5: recreate_records(); break;
            case 6: append_and_read();  break;
            case 7: edit_record(1);     break;
            case 0: printf("\n  Goodbye!\n"); break;
            default: printf("  [!] Invalid.\n");
        }
    } while (choice != 0);
    return 0;
}
