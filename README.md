# Pokemon Client-Server Project (Final Stage)

## Description

This is the final stage of the project. The application has been split into two separate programs:

1. **The Server (PPS):** Listens on Port 57744, reads the CSV file, and processes searches.
2. **The Client (PQC):** Connects to the server, sends user queries, and saves results to disk.

## How to Compile (Makefile)

We have provided a Makefile to build everything at once. Run:

```
make
```

## How to Run

**Step 1: Start the Server**

1. Run:

```
./pokemon-server
```

2. Enter `pokemon.csv` when prompted.
3. To stop the server, press `Ctrl+C`.

**Step 2: Start the Client**
Open a new terminal window (keep the server running in the first one).

1. Run:

```
./pokemon-client
```

2. Use the menu to Search and Save.

## Design Choices

- **TCP Sockets:** We used TCP sockets to ensure reliable communication between the client and the server.
- **Shared Header:** We created a `common.h` file to define the Pokemon structure and port settings in one place. This ensures both the Client and Server use the exact same data format.
- **Port 57744:** Changed the port to 50000 + the last 4 digits of my student number as requested by the TA.
- **Separation of Logic:**
  - The Server handles all File I/O for the database (pokemon.csv).
  - The Client handles the User Interface and saving the results to the local machine.
