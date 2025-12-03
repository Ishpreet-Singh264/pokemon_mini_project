#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "common.h"

// This variable points to the start of our list in memory
struct Pokemon *head = NULL;

// Helper to save a line of data received from the server
// (Similar to addToList from Stage 2)
void saveResult(char *data) {
    // Allocate memory for a new Pokemon structure
    struct Pokemon *newPoke = (struct Pokemon *)malloc(sizeof(struct Pokemon));
    
    // Parse the CSV line data
    char *token = strtok(data, ",");
    newPoke->id = atoi(token);
    
    token = strtok(NULL, ",");
    strcpy(newPoke->name, token);
    
    token = strtok(NULL, ",");
    strcpy(newPoke->type1, token);
    
    // Point this new Pokemon to whatever used to be first
    newPoke->next = head;
    
    // Update the head to point to this new Pokemon
    head = newPoke;
}

// Function to connect to server and ask for Pokemon
void requestQuery(char *type) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create the socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    
    // Set up server address (localhost)
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    // Try to connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf(">> Error: Could not connect to Server.\n");
        return;
    }

    // Send the query type to the server
    send(sock, type, strlen(type), 0);
    printf(">> Query sent. Waiting for response...\n");

    int valread;
    int count = 0;

    // Keep reading data until the server stops sending
    while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
        buffer[valread] = '\0';

        // Fix for client reading the stream as a single line
        // NEW LOGIC: Pointer to walk through the buffer
        char *start = buffer;
        char *newline;

        // Loop to find every newline character in this chunk of data
        while ((newline = strchr(start, '\n')) != NULL) {
            *newline = '\0'; // Turn the newline into a string terminator

            // Check for the stop signal
            if (strstr(start, "DONE")) goto finished;

            // Make a safe copy of the line
            char *temp = strdup(start);
            // Save this specific line
            saveResult(temp);
            free(temp); // Clean up the copy
            count++;

            // Move the pointer to the next line
            start = newline + 1;
        }
    }

    finished: // Label to jump to when "DONE" is found
    if (count > 0) printf(">> Received %d new records.\n", count);
    else printf(">> No records found for type '%s'.\n", type);
    // Close the connection
    close(sock);
}

// This function saves the current list to a new CSV file
void saveToDisk() {
    // First, check if the list is empty
    if (head == NULL) {
        printf("\n>> Warning: Nothing to save! Try searching first.\n");
        return;
    }

    char name[100];
    printf("\nEnter filename to save results (e.g. results.csv): ");
    scanf("%s", name);
    getchar(); // Consume the newline left by scanf
    
    // Create a new file for writing
    FILE *f = fopen(name, "w");
    if (f == NULL) {
        printf(">> Error creating file.\n");
        return;
    }
    
    // Write the header row first
    fprintf(f, "ID,Name,Type 1\n");
    
    // Start at the beginning of the list
    struct Pokemon *curr = head;
    
    // Loop through every item in the list
    while(curr) {
        // Write the data to the file
        fprintf(f, "%d,%s,%s\n", curr->id, curr->name, curr->type1);
        // Move to the next item
        curr = curr->next;
    }
    // Close the file when done
    fclose(f);
    printf(">> Results successfully saved to %s.\n", name);
}

int main() {
    int choice = 0;
    char inputBuffer[10];

    printf("======================================\n");
    printf("     POKEMON QUERY CLIENT (PQC)       \n");
    printf("======================================\n");

    // Loop the menu until the user chooses to Exit (3)
    while (choice != 3) {
        printf("\n------------- CLIENT MENU ------------\n");
        printf("[1] Search Type\n[2] Save Results\n[3] Exit\n");
        printf("Select: ");
        
        // Use fgets so pressing Enter (empty line) simply refreshes the menu
        if (fgets(inputBuffer, 10, stdin) != NULL) {
            choice = atoi(inputBuffer);
        }

        if (choice == 1) {
            char type[100];
            printf("Enter Pokemon Type (e.g. Fire, Water): ");
            scanf("%s", type);
            getchar(); // Consume newline to keep menu clean
            // Send the request to the server
            requestQuery(type);
        } else if (choice == 2) {
            saveToDisk();
        } else if (choice == 3) {
            printf("Exiting Client.\n");
        } else if (choice == 0) {
            // Do nothing, loop reprints menu
        }
    }
    return 0;
}