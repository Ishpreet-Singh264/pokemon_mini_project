#ifndef COMMON_H
#define COMMON_H


#define PORT 57744 //50000 + last 4 digits of mu student ID
#define BUFFER_SIZE 2048
#define MAX_LEN 100

// This structure holds the data for one Pokemon
struct Pokemon {
    int id;                 // The Pokemon's ID number
    char name[MAX_LEN];     // The Pokemon's Name
    char type1[MAX_LEN];    // The Pokemon's primary Type
    struct Pokemon *next;   // A pointer to the next Pokemon in our list
};

#endif