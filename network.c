// network.c
#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

//Function to send blockchain
void send_blockchain(int socket, Block *blockchain)
{
    //Serialize blockchain to json
    char *json = serialize_blockchain_to_json(blockchain);

    // Print the JSON to the terminal
    printf("Serialized Blockchain JSON: %s\n", json);

    //Prepare HTTP Response
    char response[2048 * 10];
    //Format HTTP Response
    sprintf(response,
            "HTTP/1.1 200 OK\n"
            "Content-Type: application/json\n"
            "Content-Length: %ld\n"
            "\n"
            "%s",
            strlen(json), json);

    //Send HTTP Response
     if (send(socket, response, strlen(response), 0) == -1)
     {
        perror("send failed");
     }
    free(json);
}

//Function to handle client
void* handle_client(void* arg)
{
    // Cast the argument to the ThreadArgs structure
    ThreadArgs *args = (ThreadArgs*)arg;
    // Extract the socket and blockchain from the arguments
    int new_socket = args->socket;
    Block *blockchain = args->blockchain;

    // Free the dynamically allocated memory for arguments
    free(arg);

    // Send the blockchain
    send_blockchain(new_socket, blockchain);

    // Close the socket
    close(new_socket);
    return NULL;
}

void start_server(Block *blockchain)
{
    // Declare the server socket file descriptor
    int server_fd, new_socket;
    // Declare the server address structure
    struct sockaddr_in address;
    // Declare the size of the address structure
    int addrlen = sizeof(address);
    // Declare the socket options
    int opt = 1;
    // Create a socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    // Set the server address
    address.sin_family = AF_INET;
    // Set the IP address to localhost
    address.sin_addr.s_addr = INADDR_ANY;
    // Set the port number
    address.sin_port = htons(PORT);
    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    // Accept incoming connections
    while (1)
    {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("accept");
            continue;
        }

        // Allocate memory for the thread arguments
        ThreadArgs *args = malloc(sizeof(ThreadArgs));
        if (!args) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        args->socket = new_socket;
        args->blockchain = blockchain;

        pthread_t thread_id; // Declare thread_id of type pthread_t
        // Create a new thread to handle the client
        pthread_create(&thread_id, NULL, handle_client, args);
        pthread_detach(thread_id);
    }
}
//Function to connect to server
void connect_to_server(const char *address, int port, Block **blockchain)
{
    //Suppress unused parameter warning by casting to void
    (void)blockchain;

    // Declare the socket file descriptor
    int sock;
    int attempt = 0;
    int max_attempts = 5;
    int connected = 0;
    // Declare the server address structure
    struct sockaddr_in serv_addr;

    // Initialize server address structure
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, address, &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }
    // Try to connect to the server
    while (attempt < max_attempts && !connected)
    {
        // Create socket
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return;
        }

        // Try to connect
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            printf("Connection Failed on attempt %d\n", attempt + 1);
            close(sock); // Close the socket before retrying
        }
        else
        {
            printf("Connected to the server\n");
            connected = 1;
        }
        attempt++;
    }

    if (!connected)
    {
        printf("Failed to connect after %d attempts.\n", max_attempts);
    }
}

// Declaration of receive_data
int receive_data(int socket, Block *block)
{
    // Declare variables to store the number of bytes received
    ssize_t bytes_received;
    size_t total_bytes = 0;
    size_t block_size = sizeof(Block);
    // Cast the block to a char pointer
    char *buffer = (char*)block;
    // Receive the block data
    while (total_bytes < block_size)
    {
        // Receive the data
        bytes_received = recv(socket, buffer + total_bytes, block_size - total_bytes, 0);
        if (bytes_received < 0) {
            // Error in receiving data
            perror("recv failed");
            return -1; // Return an error code
        }
        // Check if the connection was closed
        else if (bytes_received == 0)
        {
            // Connection closed
            printf("Connection closed by peer\n");
            return -2; // Return a different error code
        }
        total_bytes += bytes_received;
    }

    // Successfully received a block
    return SUCCESS;
}

void receive_blockchain(int socket, Block **blockchain)
{
    // Allocate memory for the new block
    Block *new_block = (Block*)malloc(sizeof(Block));
    // Check if memory allocation was successful
    if (!new_block) {
        perror("malloc failed");
        return;
    }
    // Initialize the new block
    Block *last_block = NULL;
    // Receive the blocks
    while (1)
    {
        int receive_status = receive_data(socket, new_block);
        // Check if the block was received successfully
        if (receive_status != SUCCESS)
        {
            free(new_block);
            break;
        }
        // Add the block to the blockchain
        if (*blockchain == NULL)
        {
            *blockchain = new_block;
        }
        // Add the block to the end of the blockchain
        else
        {
            last_block->next = new_block;
        }
        last_block = new_block;
        // Allocate memory for the next block
        new_block = (Block*)malloc(sizeof(Block));
        // Check if memory allocation was successful
        if (!new_block)
        {
            perror("malloc failed");
            break;
        }
    }
    free(new_block);
}