# Pokemon Server - Stage 1

## Description

This is the first stage of the Pokemon Query project. It is a single program that reads a CSV file, allows the user to search for Pokemon by type, and saves the results to a new file.

## How to Compile

Run the following command in your terminal:

```
gcc -o stage1 pokemon_server.c
```

## How to Run

1. Start the program:
   ```
   ./stage1
   ```
2. When prompted for the filename, enter:
   ```
   pokemon.csv
   ```

## Design Choices

- **Linked List:** We used a linked list to store the search results in memory so we can save them later without needing to know the exact number of results beforehand. (Not part of the course though)
- **File I/O:** The program uses standard C functions (fopen, fgets) to read the CSV file line-by-line.
- **Case Insensitivity:** We included logic to ignore capitalization so "fire" and "Fire" both work.
