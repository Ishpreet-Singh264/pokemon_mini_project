# Pokemon Server - Stage 2

## Description

This is the second stage of the project. It updates the search function to run in a background thread. This allows the user to continue using the menu while the program searches through the file.

## How to Compile

Run the following command in your terminal:

```
gcc -o stage2 pokemon_server-stage-1.c
```

## How to Run

1. Start the program:
   ```
   ./stage2
   ```
2. When prompted for the filename, enter:
   ```
   pokemon.csv
   ```

## Design Choices

- **Pthreads:** I used the pthread library to create a separate thread for searching. This keeps the main menu responsive.
- **Mutex Locks:** I added a Mutex lock to protect the shared list. This prevents the program from crashing if the search thread tries to write data at the same time the main thread tries to read it.
- **Safe Saving:** The save function checks if a search is currently running and waits for it to finish before writing to the disk.
