#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#define INITIAL_PATIENTS 5
#define MAX 100

typedef struct {
    int ward_no;
    int id;
    char name[30];
    char admission_date[11];  
    char reason[100];
} Patient;

int is_alpha_string(const char *str) {
    for (int i = 0; str[i]; i++) {
        if (!isalpha(str[i]))
            return 0;
    }
    return 1;
}

void Write_Initial_Records()
{
    FILE *fp1 = fopen("Ward1_Records.txt", "w");
    if (fp1 == NULL) {
        printf("\n Error opening file!");
        return;
    }

    Patient Ward1_Patients[INITIAL_PATIENTS] = {
        {1,101,"Abi","2025-01-20","Fever"},
        {1,103,"Bhar","2025-01-20","Fever"},
        {1,107,"Gokul","2025-01-25","Fever"},
        {1,108,"JP","2025-03-16","Leg Injury"},
        {1,110,"Kavi","2025-07-18","Baby Delivery"}
    };
     for (int i = 0; i < INITIAL_PATIENTS; i++) 
    {
        fprintf(fp1, "%d %d %s %s %s \n",Ward1_Patients[i].ward_no,Ward1_Patients[i].id,Ward1_Patients[i].name,Ward1_Patients[i].admission_date,Ward1_Patients[i].reason);
    }

    fclose(fp1);

    FILE *fp2 = fopen("Ward2_Records.txt", "w");
    if (fp2 == NULL) {
        printf("\n Error opening file!");
        return;
    }

    Patient Ward2_Patients[INITIAL_PATIENTS] = {
        {1,102,"Aadhi","2025-01-20","Leg Fracture"},
        {1,104,"Harish","2025-01-20","Fever"},
        {1,105,"Harini","2025-01-25","Fever"},
        {1,106,"Lakshmi","2025-03-16","Leg Injury"},
        {1,109,"Vinod","2025-07-18","Wrist Fracture"}
    };
     for (int i = 0; i < INITIAL_PATIENTS; i++)
    {
        fprintf(fp2, "%d %d %s %s %s \n",Ward2_Patients[i].ward_no,Ward2_Patients[i].id,Ward2_Patients[i].name,Ward2_Patients[i].admission_date,Ward2_Patients[i].reason);
    }

    fclose(fp2);
}

int find_patient_name_by_id(int id, char *found_name) {
    FILE *fp;
    Patient patients;

    const char *files[] = {"Ward1_Records.txt", "Ward2_Records.txt"};
    for (int i = 0; i < 2; i++) {
        fp = fopen(files[i], "r");
        if (!fp) continue;

        while (fscanf(fp, "%d %d %s %s %[^\n]", &patients.ward_no, &patients.id, patients.name, patients.admission_date, patients.reason) == 5) {
            if (patients.id == id) {
                strcpy(found_name, patients.name);
                fclose(fp);
                return 1;
            }
        }
        fclose(fp);
    }
    return 0;
}

int compare_by_id_and_date(const void *a, const void *b) {
    Patient *pa = (Patient *)a;
    Patient *pb = (Patient *)b;

    if (pa->id != pb->id)
        return pa->id - pb->id;
    else
        return strcmp(pa->admission_date, pb->admission_date);
}


void Merge_Master() {
    FILE *fp1 = fopen("Ward1_Records.txt", "r");
    FILE *fp2 = fopen("Ward2_Records.txt", "r");
    FILE *fmaster = fopen("Master_Records.txt", "w");

    if (!fmaster) {
        printf("Error opening master file.\n");
        return;
    }

    Patient temp[MAX];
    int count = 0;

    if (fp1) {
        while (fscanf(fp1, "%d %d %s %s %[^\n]",&temp[count].ward_no, &temp[count].id,temp[count].name, temp[count].admission_date,temp[count].reason) == 5)
        {
            count++;
        }
        fclose(fp1);
    }

    if (fp2) {
        while (fscanf(fp2, "%d %d %s %s %[^\n]",&temp[count].ward_no, &temp[count].id,temp[count].name, temp[count].admission_date,temp[count].reason) == 5)
        {
            count++;
        }
        fclose(fp2);
    }

    qsort(temp, count, sizeof(Patient), compare_by_id_and_date);

    for (int i = 0; i < count; i++) {
        fprintf(fmaster, "%d %d %s %s %s\n",temp[i].ward_no, temp[i].id, temp[i].name,temp[i].admission_date, temp[i].reason);
    }

    fclose(fmaster);
    printf("Master file updated and sorted by ID and admission date.\n");
}


void Add_Patient() {
    Patient patients;
    FILE *fp;
    char filename[25];
    char existing_name[30];

    printf("Enter Ward Number (1 or 2): ");
    if (scanf("%d", &patients.ward_no) != 1 || (patients.ward_no != 1 && patients.ward_no != 2)) {
        printf("Invalid ward number.\n");
        while (getchar() != '\n');  
        return;
    }

    printf("Enter Patient ID (positive number): ");
    if (scanf("%d", &patients.id) != 1 || patients.id <= 0) {
        printf("Invalid patient ID.\n");
        while (getchar() != '\n');
        return;
    }

    if (find_patient_name_by_id(patients.id, existing_name)) {
        printf("Patient ID already exists. Please enter the same name as the existing record.\n");
        printf("Enter Name: ");
        scanf(" %s", patients.name);

        if (strcmp(patients.name, existing_name) != 0) {
            printf("Error: Name doesn't match existing patient with the same ID (%s expected).\n", existing_name);
            return;
        }
    } 
    else
    {
        printf("Enter Name (only letters): ");
        scanf(" %s", patients.name);
        if (!is_alpha_string(patients.name)) {
            printf("Invalid name. Only alphabetic characters allowed.\n");
            return;
        }
    }

    printf("Enter Admission Date (YYYY-MM-DD): ");
    scanf("%s", patients.admission_date);
    

    printf("Enter Reason for Admission: ");   
    fgets(patients.reason, sizeof(patients.reason), stdin);
    

    for (int i = 0; patients.reason[i]; i++) {
        if (!isalnum(patients.reason[i]) && !isspace(patients.reason[i])) {
            printf("Invalid characters in reason. Only letters, digits and spaces allowed.\n");
            return;
        }
    }

    sprintf(filename, "Ward%d_Records.txt", patients.ward_no);
    fp = fopen(filename, "a");
    if (!fp) {
        printf("Error opening %s.\n", filename);
        return;
    }

    fprintf(fp, "%d %d %s %s %s\n", patients.ward_no, patients.id,
            patients.name, patients.admission_date, patients.reason);
    fclose(fp);

    Merge_Master();  
    printf("Patient record added to %s.\n", filename);
    
}

void List_by_Date() {
    FILE *fp = fopen("Master_Records.txt", "r");
    if (!fp) {
        printf("Master file not found.\n");
        return;
    }

    char date[11];
    printf("Enter Admission Date (YYYY-MM-DD): ");
    scanf("%s", date);

    Patient patients;
    int found = 0;

    printf("\nPatients Admitted on %s:\n", date);
    printf("----------------------------------------------------------\n");
    printf("Ward\tID\tName\t\tReason\n");

    while (fscanf(fp, "%d %d %s %s %[^\n]",&patients.ward_no,&patients.id, patients.name, patients.admission_date, patients.reason) == 5) {
        if (strcmpi(patients.admission_date, date) == 0) {
            printf("%d\t%d\t%s\t\t%s\n", patients.ward_no, patients.id, patients.name, patients.reason);
            found = 1;
        }
    }

    if (!found) {
        printf("No patients found for the given date.\n");
    }

    fclose(fp);
}

void View_Patient_Records()
{
    int choice;
    Patient patients;
    int count = 0;
    FILE *fp = NULL;
    char filename[25];

    printf("\n Records\n 1. Ward-1 Records \n 2. Ward-2 Records \n 3. Master Records \n Enter your choice: ");
    scanf("%d", &choice);

    if(choice == 1)
        strcpy(filename, "Ward1_Records.txt");
    else if(choice == 2)
        strcpy(filename, "Ward2_Records.txt");
    else if(choice == 3)
        strcpy(filename, "Master_Records.txt");
    else {
        printf("\n Enter valid option.\n");
        return;
    }
    
    Merge_Master();

    fp = fopen(filename, "r");
    if (!fp) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    printf("\n------------------------------ All Patients Records -----------------------------------------------\n");
    printf("Ward Number\tPatient ID\tName\t\t\tAdmission Date\t\tReason\n");
    printf("-----------------------------------------------------------------------------------------------------\n");

    while (fscanf(fp, "%d %d %s %s %[^\n]", &patients.ward_no, &patients.id,
                  patients.name, patients.admission_date, patients.reason) == 5) 
    {
        printf("%d\t\t%d\t\t%s\t\t\t%s\t\t%s\n", 
            patients.ward_no, patients.id, patients.name, 
            patients.admission_date, patients.reason);
        count++;
    }

    if (count == 0)
        printf("\nNo records found in file.\n");

    fclose(fp);
}


int main() {
    int choice;
    Write_Initial_Records();
    Merge_Master();
    do {
        printf("\n--- Hospital Patient Management System ---\n");
        printf("1. Add Patient Record\n");
        printf("2. View Patient Records\n");
        printf("3. List Patients by Date\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: Add_Patient(); break;
            case 2: View_Patient_Records();break;
            case 3: List_by_Date(); break;
            case 4: printf("Exiting...\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }

    } while (choice != 4);

    return 0;
}
