# Build both the client and server
all: pokemon-server pokemon-client

# Compile the server
pokemon-server: pokemon-server.c common.h
	gcc -Wall -pthread -o pokemon-server pokemon-server.c

# Compile the client
pokemon-client: pokemon-client.c common.h
	gcc -Wall -pthread -o pokemon-client pokemon-client.c

# Remove the compiled files
clean:
	rm -f pokemon-server pokemon-clients