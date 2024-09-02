// blockchain.h
#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <time.h>
#include <openssl/sha.h>

// Define the port number
#define PORT 8080

// Define the maximum number of connection attempts
#define MAX_ATTEMPTS 5

// Define success
#define SUCCESS 0

// Define the transaction structure
typedef struct Transaction
{
    char sender[50];
    char receiver[50];
    double amount;
    struct Transaction *next;
}
Transaction;

// Define the block structure
typedef struct Block
{
    int index;
    time_t timestamp;
    Transaction *transactions;
    char prev_hash[SHA256_DIGEST_LENGTH*2+1];
    char hash[SHA256_DIGEST_LENGTH*2+1];
    struct Block *next;
}
Block;

// Define the thread arguments structure
typedef struct
{
    int socket;
    Block *blockchain;
}
ThreadArgs;

// Function declarations
void calculate_hash(Block *block, char *output);
char* serialize_transactions(Transaction *transaction);
Transaction* create_transaction(const char *sender, const char *receiver, double amount);
void add_transaction(Block *block, Transaction *transaction);
void free_transactions(Transaction *transaction);
Block* create_block(int index, const char *data, const char *prev_hash);
char* serialize_blockchain_to_json(Block *blockchain);
Block* add_block(Block *head, const char *data);
void print_blockchain(Block *head);
void free_blockchain(Block *head);
void send_blockchain(int socket, Block *blockchain);
void start_server(Block *blockchain);
void connect_to_server(const char *address, int port, Block **blockchain);
int receive_data(int socket, Block *block);
void receive_blockchain(int socket, Block **blockchain);

#endif // BLOCKCHAIN_H