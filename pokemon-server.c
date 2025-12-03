#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // This is needed for case-insensitive search

#define MAX_LEN 100

// This structure holds the data for one Pokemon
struct Pokemon {
    int id;                 // The Pokemon's ID number
    char name[MAX_LEN];     // The Pokemon's Name
    char type1[MAX_LEN];    // The Pokemon's primary Type
    struct Pokemon *next;   // A pointer to the next Pokemon in our list
};

// A lock to protect the list when multiple threads access it 
pthread_mutex_t lock; 

// This variable points to the start of our list in memory
struct Pokemon *head = NULL;

// This function adds a new Pokemon to the front of our list
void addToList(int id, char *name, char *type) {
    // Allocate memory for a new Pokemon structure
    struct Pokemon *newPoke = (struct Pokemon *)malloc(sizeof(struct Pokemon));
    
    // Set the values for this new Pokemon
    newPoke->id = id;
    strcpy(newPoke->name, name);
    strcpy(newPoke->type1, type);
    
    // Point this new Pokemon to whatever used to be first
    newPoke->next = head;
    
    // Update the head to point to this new Pokemon
    head = newPoke;
}

// This function reads the file and looks for matches
void searchPokemon(char *filename, char *typeToFind) {
    // Try to open the file for reading
    FILE *f = fopen(filename, "r");
    
    // If the file doesn't exist, print an error and stop
    if (f == NULL) {
        printf(">> Error: Pokemon file not found.\n");
        return;
    }

    char line[1024];
    // Read the first line to skip the headers (ID, Name, etc.)
    fgets(line, 1024, f);

    int count = 0;
    // Keep reading lines until we reach the end of the file
    while (fgets(line, 1024, f)) {
        // Break the line apart using commas
        char *token = strtok(line, ",");
        int id = atoi(token); // Convert the ID string to a number

        token = strtok(NULL, ","); // Get the next part (Name)
        char name[MAX_LEN];
        strcpy(name, token);

        token = strtok(NULL, ","); // Get the next part (Type 1)
        char type[MAX_LEN];
        strcpy(type, token);

        // Compare the type found with the type the user wants (ignore case)
        if (strcasecmp(type, typeToFind) == 0) {
            // If it matches, add it to our list
            addToList(id, name, type);
            count++;
        }
    }
    
    // Print the results nicely
    printf("\n------------------------------------------------\n");
    if (count > 0) {
        printf(">> Success! Found %d Pokemon of type '%s'.\n", count, typeToFind);
        printf(">> These have been added to your save list.\n");
    } else {
        printf(">> No Pokemon found of type '%s'.\n", typeToFind);
    }
    printf("------------------------------------------------\n");
    
    // Close the file to free up resources
    fclose(f);
}

// This function saves the current list to a new CSV file
void saveToFile() {
    // First, check if the list is empty
    if (head == NULL) {
        printf("\n>> Warning: Nothing to save! Try searching first.\n");
        return;
    }

    char outName[MAX_LEN];
    printf("\nEnter filename to save results (e.g. results.csv): ");
    scanf("%s", outName);

    // Create a new file for writing
    FILE *f = fopen(outName, "w");
    if (f == NULL) {
        printf(">> Error creating file.\n");
        return;
    }

    // Write the header row first
    fprintf(f, "ID,Name,Type 1\n");

    // Start at the beginning of the list
    struct Pokemon *current = head;
    
    // Loop through every item in the list
    while (current != NULL) {
        // Write the data to the file
        fprintf(f, "%d,%s,%s\n", current->id, current->name, current->type1);
        // Move to the next item
        current = current->next;
    }
    
    // Close the file when done
    fclose(f);
    printf(">> Results successfully saved to %s.\n", outName);
}

int main() {
    char filename[MAX_LEN];
    
    // Print the welcome banner
    printf("================================================\n");
    printf("        WELCOME TO POKEMON QUERY SYSTEM         \n");
    printf("================================================\n");
    printf("Enter Pokemon Data Filename: ");
    scanf("%s", filename);

    // Quickly check if the main file actually exists
    FILE *test = fopen(filename, "r");
    if (test == NULL) {
        printf(">> File not found. Restart and try again.\n");
        return 1;
    } else {
        fclose(test);
    }

    int choice = 0;
    // Loop the menu until the user chooses to Exit (3)
    while (choice != 3) {
        printf("\n================ MAIN MENU =====================\n");
        printf("[1] Search for Pokemon\n");
        printf("[2] Save Results to File\n");
        printf("[3] Exit Program\n");
        printf("------------------------------------------------\n");
        printf("Select Option: ");
        scanf("%d", &choice);

        if (choice == 1) {
            char type[MAX_LEN];
            printf("Enter Pokemon Type (e.g. Fire, Water): ");
            scanf("%s", type);
            // Run the search function
            searchPokemon(filename, type);
        } else if (choice == 2) {
            // Run the save function
            saveToFile();
        } else if (choice == 3) {
            printf("\n>> Exiting... Gotta catch 'em all!\n");
        } else {
            printf(">> Invalid option.\n");
        }
    }
    return 0;
}