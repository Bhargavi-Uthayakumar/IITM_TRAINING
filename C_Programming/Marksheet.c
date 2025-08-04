#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_STUDENTS 5

typedef struct {
    int Roll_No;
    char Student_Name[20];
    int Mark[3];
    int Total_Marks;
    float Average;
} Student;


void Calculate_Total_Marks(Student *students) {
    students->Total_Marks = 0;
    for (int i = 0; i < 3; i++) {
        students->Total_Marks += students->Mark[i];
    }
    students->Average = students->Total_Marks / 3.0;
}


void Write_Initial_Records() {
    FILE *fp = fopen("Marksheet.txt", "w");
    if (fp == NULL) {
        printf("\n Error opening file!");
        return;
    }

    Student students[INITIAL_STUDENTS] = {
        {1, "Abi",   {85, 92, 78}},
        {2, "Bhar",  {75, 64, 70}},
        {3, "Gokul", {88, 90, 84}},
        {4, "JP",    {95, 97, 93}},
        {5, "Kavi",  {60, 58, 65}}
    };

    for (int i = 0; i < INITIAL_STUDENTS; i++) {
        Calculate_Total_Marks(&students[i]);
        fprintf(fp, "%d %s %d %d %d %d %.2f\n",
                students[i].Roll_No,
                students[i].Student_Name,
                students[i].Mark[0],
                students[i].Mark[1],
                students[i].Mark[2],
                students[i].Total_Marks,
                students[i].Average);
    }

    fclose(fp);
}


int is_valid_integer_input() {
    int val;
    char term;
    if (scanf("%d%c", &val, &term) != 2 || term != '\n') {
        while (getchar() != '\n');  
        return -1;
    }
    return val;
}

void Append_Student_Records() {
    Student *all_students = NULL;
    int total = 0;
    int capacity = 10; // Initial capacity for dynamic array
    all_students = malloc(capacity * sizeof(Student));
    if (!all_students) {
        printf("\n Memory allocation failed.");
        return;
    }

    FILE *fp = fopen("Marksheet.txt", "r");
    if (fp) {
        while (fscanf(fp, "%d %s %d %d %d %d %f",
                      &all_students[total].Roll_No,
                      all_students[total].Student_Name,
                      &all_students[total].Mark[0],
                      &all_students[total].Mark[1],
                      &all_students[total].Mark[2],
                      &all_students[total].Total_Marks,
                      &all_students[total].Average) == 7) {

            total++;
            if (total >= capacity) {
                capacity *= 2;
                Student *temp = realloc(all_students, capacity * sizeof(Student));
                if (!temp) {
                    printf("\n Memory reallocation failed.");
                    free(all_students);
                    fclose(fp);
                    return;
                }
                all_students = temp;
            }
        }
        fclose(fp);
    }

    int new_count;
    printf("\nEnter number of students to add: ");
    if ((new_count = is_valid_integer_input()) <= 0) {
        printf("\n Invalid number of students.\n");
        free(all_students);
        return;
    }

    for (int i = 0; i < new_count; i++) {
        Student students;

        printf("\n--- Enter details for student %d ---\n", i + 1);

        while (1) {
            printf("\n Roll Number: ");
            int roll = is_valid_integer_input();
            if (roll < 0) {
                printf("\n Invalid input. Try again.");
                continue;
            }

            int duplicate = 0;
            for (int j = 0; j < total; j++) {
                if (all_students[j].Roll_No == roll) {
                    printf("\n Roll number already exists.");
                    duplicate = 1;
                    break;
                }
            }

            if (!duplicate) {
                students.Roll_No = roll;
                break;
            }
        }

        while (1) {
            printf("Name: ");
            scanf(" %19s", students.Student_Name);
            int valid = 1;
            for (int k = 0; k < strlen(students.Student_Name); k++) {
                if (!isalpha(students.Student_Name[k])) {
                    valid = 0;
                    break;
                }
            }
            if (!valid) {
                printf("\n Name must contain only alphabets. Try again.");
                continue;
            }

            int duplicate = 0;
            for (int j = 0; j < total; j++) {
                if (strcasecmp(all_students[j].Student_Name, students.Student_Name) == 0) {
                    printf("\n Name already exists.");
                    duplicate = 1;
                    break;
                }
            }
            if (!duplicate) break;
        }

        for (int j = 0; j < 3; j++) {
            while (1) {
                printf("Marks for Subject %d: ", j + 1);
                int mark = is_valid_integer_input();
                if (mark < 0 || mark > 100) {
                    printf("\n Enter a valid mark between 0 and 100.");
                } else {
                    students.Mark[j] = mark;
                    break;
                }
            }
        }

        Calculate_Total_Marks(&students);

        if (total >= capacity) {
            capacity *= 2;
            Student *temp = realloc(all_students, capacity * sizeof(Student));
            if (!temp) {
                printf("\n Memory reallocation failed.");
                free(all_students);
                return;
            }
            all_students = temp;
        }

        all_students[total++] = students;
    }

    fp = fopen("Marksheet.txt", "w");
    if (!fp) {
        printf("\n Error rewriting to file.\n");
        free(all_students);
        return;
    }

    for (int i = 0; i < total; i++) {
        fprintf(fp, "%d %s %d %d %d %d %.2f\n",
                all_students[i].Roll_No,
                all_students[i].Student_Name,
                all_students[i].Mark[0],
                all_students[i].Mark[1],
                all_students[i].Mark[2],
                all_students[i].Total_Marks,
                all_students[i].Average);
    }

    fclose(fp);
    free(all_students);
    printf("\n Records added and updated successfully!\n");
}


void Read_Student_Record() {
    int roll, found = 0;
    printf("\n Enter roll number to search: ");
    scanf("%d", &roll);

    FILE *fp = fopen("Marksheet.txt", "r");
    if (!fp) {
        printf("\n File open error.");
        return;
    }

    Student students;
    while (fscanf(fp, "%d %s %d %d %d %d %f",&students.Roll_No, students.Student_Name,&students.Mark[0], &students.Mark[1], &students.Mark[2],&students.Total_Marks, &students.Average) == 7) 
    {
        if (students.Roll_No == roll) {
            printf("\n--- Student Found ---");
            printf("\n Roll No: %d", students.Roll_No);
            printf("\n Name: %s", students.Student_Name);
            for (int i = 0; i < 3; i++) {
                printf("\n Subject %d: %d", i + 1, students.Mark[i]);
            }
            printf("\n Total Marks: %d\nAverage: %.2f", students.Total_Marks, students.Average);
            found = 1;
            break;
        }
    }

    if (!found)
        printf("\nStudent with Roll No %d not found.", roll);

    fclose(fp);
}


void Calculate_Class_Performance() {
    FILE *fp = fopen("Marksheet.txt", "r");
    if (!fp) {
        printf("\n Unable to open 'Marksheet.txt'");
        return;
    }

    Student students;
    int total_students = 0;
    float total_average = 0;
    int pass_count = 0;

    while (fscanf(fp, "%d %s %d %d %d %d %f",&students.Roll_No, students.Student_Name,&students.Mark[0], &students.Mark[1], &students.Mark[2],&students.Total_Marks, &students.Average) == 7) 
    {
        total_students++;
        total_average += students.Average;

        int pass = 1;
        for (int i = 0; i < 3; i++) {
            if (students.Mark[i] < 35) {
                pass = 0;
                break;
            }
        }
        if (pass) pass_count++;
    }

    fclose(fp);

    if (total_students == 0) {
        printf("\n No student records found.");
        return;
    }

    float class_average = total_average / total_students;
    float pass_percentage = (pass_count / (float)total_students) * 100.0;

    printf("\n----- Class Performance -----\n");
    printf("\n Total Students     : %d", total_students);
    printf("\n Class Average %%     : %.2f%%", class_average);
    printf("\n Pass Percentage    : %.2f%%", pass_percentage);
}

void Display_All_Records() {
    FILE *fp = fopen("Marksheet.txt", "r");
    if (!fp) {
        printf("\n Unable to open 'Marksheet.txt'");
        return;
    }

    Student students;
    int count = 0;

    printf("\n------------------------------ All Student Records -----------------------------------------------\n");
    printf("Roll No\t\tName\t\tSubject1\tSubject2\tSubject3\tTotal\tAverage\n");
    printf("--------------------------------------------------------------------------------------------------\n");

    while (fscanf(fp, "%d %s %d %d %d %d %f",&students.Roll_No, students.Student_Name,&students.Mark[0], &students.Mark[1], &students.Mark[2],&students.Total_Marks, &students.Average) == 7) 
    {

            printf("%d\t\t", students.Roll_No);
            printf("%s\t\t", students.Student_Name);
            for (int i = 0; i < 3; i++)
            {
                printf("%d\t\t", students.Mark[i]);
            }
            printf("%d\t %.2f\t\n", students.Total_Marks, students.Average);
            count++;
    }

    if (count == 0)
        printf("\nNo records found in file.\n");

    fclose(fp);
}


int main() {
    int choice;

    // Write predefined records only once at program start
    Write_Initial_Records();

    do {
        printf("\n\n----- Student Marksheet System -----\n\n");
        printf("\n 1. Display all the Student Records");
        printf("\n 2. Add new student entry");
        printf("\n 3. Read student details");
        printf("\n 4. Show Class Performance");
        printf("\n 5. Help");
        printf("\n 6. Quit");
        
        printf("\n \n Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                Display_All_Records();
                break;
            case 2:
                Append_Student_Records();
                break;
            case 3:
                Read_Student_Record();
                break;
            case 4:
                    Calculate_Class_Performance();
                    break;
            case 5:
                    printf("\n Help Menu ");
                    printf("\n 1. Display all the Student Records");
                    printf("\n 2. Add new student entry");
                    printf("\n 3. Read student details");
                    printf("\n 4. Show Class Performance");
                    printf("\n 5. Help");
                    printf("\n 6. Quit");
                    break;
            case 6:
                printf("\n Exiting program.");
                break;
            default:
                printf("\nInvalid choice. Try again.\n");
        }

    } while (choice != 6);

    return 0;
}
