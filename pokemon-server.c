#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // This is needed for case-insensitive search
#include <pthread.h> // This is needed for multi threading
#include <unistd.h>  // This is needed for sleep function
#include <arpa/inet.h> // This is needed for networking
#include "common.h"

// This function handles the logic for a connected client
void processClient(int socket, char *filename) {
    char buffer[BUFFER_SIZE];
    // Read the message from the client
    int len = recv(socket, buffer, BUFFER_SIZE, 0);
    
    if (len > 0) {
        // Add a null terminator to make it a valid string
        buffer[len] = '\0';
        // Remove any newline characters at the end
        buffer[strcspn(buffer, "\n")] = 0;
        printf(">> Received query for: %s\n", buffer);

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

        int matches = 0;
        // Keep reading lines until we reach the end of the file
        while (fgets(line, 1024, f)) {
            char temp[1024];
            strcpy(temp, line); // Keep a copy of the line to send later

            // Break the line apart using commas
            char *token = strtok(line, ",");
            token = strtok(NULL, ","); // Name
            token = strtok(NULL, ","); // Type

            // Compare the type found with the type the user wants (ignore case)
            if (token && strcasecmp(token, buffer) == 0) {
                // Send the matching line back to the client
                send(socket, temp, strlen(temp), 0);
                matches++;
            }
        }
        // Close the file to free up resources
        fclose(f);
        printf(">> Sent %d matches to client.\n", matches);
        
        // Send a specific message to tell the client we are finished
        send(socket, "DONE", 4, 0);
    }
    // Close the connection with this client
    close(socket);
}

int main() {
    int serverFd, newSocket;
    struct sockaddr_in address;
    int addrLen = sizeof(address);
    char filename[100];

    printf("======================================\n");
    printf("   POKEMON PROPERTY SERVER (PPS)      \n");
    printf("======================================\n");
    printf("Enter CSV filename: ");
    scanf("%s", filename);

    // Create a new socket for the server
    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Setup the address settings
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from anywhere
    address.sin_port = htons(PORT);       // Use Port 80

    // Bind the socket to the port
    if (bind(serverFd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    // Start listening for incoming connections
    listen(serverFd, 3);
    printf(">> Server listening on port %d...\n", PORT);

    // Infinite loop to keep the server running
    while (1) {
        // Accept a new connection from a client
        newSocket = accept(serverFd, (struct sockaddr *)&address, (socklen_t*)&addrLen);
        if (newSocket >= 0) {
            // Process the client's request
            processClient(newSocket, filename);
        }
    }
    return 0;
}