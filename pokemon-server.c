#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // This is needed for case-insensitive search
#include <pthread.h> // This is needed for multi threading
#include <unistd.h>  // This is needed for sleep function

#define MAX_LEN 100

// This structure holds the data for one Pokemon
struct Pokemon {
    int id;                 // The Pokemon's ID number
    char name[MAX_LEN];     // The Pokemon's Name
    char type1[MAX_LEN];    // The Pokemon's primary Type
    struct Pokemon *next;   // A pointer to the next Pokemon in our list
};

// Structure to bundle arguments for the thread
struct ThreadArgs {
    char filename[MAX_LEN];
    char type[MAX_LEN];
};

// This variable points to the start of our list in memory
struct Pokemon *head = NULL;

// A lock to protect the list when multiple threads access it
pthread_mutex_t lock; 

// Counter to keep track of how many searches are running
int activeSearches = 0; 

// This function adds a new Pokemon to the front of our list
void addToList(int id, char *name, char *type) {
    // Allocate memory for a new Pokemon structure
    struct Pokemon *newPoke = (struct Pokemon *)malloc(sizeof(struct Pokemon));
    
    // Set the values for this new Pokemon
    newPoke->id = id;
    strcpy(newPoke->name, name);
    strcpy(newPoke->type1, type);
    
    // Lock the mutex so no one else changes the list right now
    pthread_mutex_lock(&lock);

    // Point this new Pokemon to whatever used to be first
    newPoke->next = head;
    
    // Update the head to point to this new Pokemon
    head = newPoke;

    // Unlock the mutex so others can use the list
    pthread_mutex_unlock(&lock);
}

// NEW: This function now runs as a thread
void *searchThread(void *args) {
    // Get the arguments passed to this thread
    struct ThreadArgs *inputs = (struct ThreadArgs *)args;

    // Increase the counter of active searches
    activeSearches++;

    // Try to open the file for reading
    FILE *f = fopen(inputs->filename, "r");
    
    int count = 0;
    // If the file exists, start reading
    if (f) {
        char line[1024];
        // Read the first line to skip the headers (ID, Name, etc.)
        fgets(line, 1024, f);

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
            if (strcasecmp(type, inputs->type) == 0) {
                // If it matches, add it to our list
                addToList(id, name, type);
                
                // Sleep for a tiny bit to simulate a long task
                usleep(1000); 
                count++;
            }
        }
        // Close the file to free up resources
        fclose(f);
    }
    
    // Notify the user that the background job is done
    printf("\n\n[Notification] Background search for '%s' finished.", inputs->type);
    if (count > 0) printf(" Found %d matches.\n", count);
    else printf(" No matches found.\n");
    printf(">> Press Enter to refresh the menu...\n"); // Updated instruction
    
    // Decrease the active search counter
    activeSearches--;
    
    // Free the memory used for arguments
    free(inputs);
    return NULL;
}

// This function saves the current list to a new CSV file
void saveToFile() {
    // If searches are still running, wait for them
    if (activeSearches > 0) {
        printf("\n>> Waiting for background searches to finish...\n");
        // Loop here until searches drop to 0
        while (activeSearches > 0) {
            sleep(1);
        }
    }

    // Lock the list to safely check if it is empty
    pthread_mutex_lock(&lock);

    // First, check if the list is empty
    if (head == NULL) {
        pthread_mutex_unlock(&lock);
        printf("\n>> Warning: Nothing to save! Try searching first.\n");
        return;
    }
    pthread_mutex_unlock(&lock);

    char outName[MAX_LEN];
    printf("\nEnter filename to save results (e.g. results.csv): ");
    scanf("%s", outName);
    getchar(); // Consume the newline left by scanf

    // Create a new file for writing
    FILE *f = fopen(outName, "w");
    if (f == NULL) {
        printf(">> Error creating file.\n");
        return;
    }

    // Write the header row first
    fprintf(f, "ID,Name,Type 1\n");

    // Lock the list while we read from it to save
    pthread_mutex_lock(&lock);

    // Start at the beginning of the list
    struct Pokemon *current = head;
    
    // Loop through every item in the list
    while (current != NULL) {
        // Write the data to the file
        fprintf(f, "%d,%s,%s\n", current->id, current->name, current->type1);
        // Move to the next item
        current = current->next;
    }
    
    // Unlock when done reading
    pthread_mutex_unlock(&lock);
    
    // Close the file when done
    fclose(f);
    printf(">> Results successfully saved to %s.\n", outName);
}

int main() {
    char filename[MAX_LEN];
    // Initialize the mutex lock
    pthread_mutex_init(&lock, NULL);
    
    // Print the welcome banner
    printf("================================================\n");
    printf("    POKEMON QUERY SYSTEM (Multi-Threaded)       \n");
    printf("================================================\n");
    printf("Enter Pokemon Data Filename: ");
    scanf("%s", filename);
    getchar(); // Consume the newline so it doesn't mess up the menu loop

    // Quickly check if the main file actually exists
    FILE *test = fopen(filename, "r");
    if (test == NULL) {
        printf(">> File not found. Restart and try again.\n");
        return 1;
    } else {
        fclose(test);
    }

    int choice = 0;
    char inputBuffer[10]; // Buffer to hold user input

    // Loop the menu until the user chooses to Exit (3)
    while (choice != 3) {
        printf("\n================ MAIN MENU =====================\n");
        printf("[1] Search for Pokemon\n");
        printf("[2] Save Results to File\n");
        printf("[3] Exit Program\n");
        printf("------------------------------------------------\n");
        printf("Select Option: ");
        
        // Use fgets so pressing Enter (empty line) simply refreshes the menu
        if (fgets(inputBuffer, 10, stdin) != NULL) {
            choice = atoi(inputBuffer);
        }

        if (choice == 1) {
            // Create memory to hold the thread arguments
            struct ThreadArgs *args = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
            strcpy(args->filename, filename);
            printf("Enter Pokemon Type (e.g. Fire, Water): ");
            scanf("%s", args->type);
            getchar(); // Consume newline
            
            pthread_t threadId;
            // Create a new thread that runs the searchThread function
            if (pthread_create(&threadId, NULL, searchThread, args) == 0) {
                // Detach the thread so it runs independently
                pthread_detach(threadId); 
                printf(">> Search started in background. You can keep using the menu.\n");
            }
        } else if (choice == 2) {
            // Run the save function
            saveToFile();
        } else if (choice == 3) {
            printf("\n>> Exiting... Gotta catch 'em all!\n");
        } else if (choice == 0) {
            // Do nothing, just reprint menu (happens on empty Enter press)
        } else {
            printf(">> Invalid option.\n");
        }
    }
    // Clean up the mutex lock
    pthread_mutex_destroy(&lock);
    return 0;
}