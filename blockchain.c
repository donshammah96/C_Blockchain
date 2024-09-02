#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to calculate SHA-256 hash
void calculate_hash(Block *block, char *output)
{
    // Create a buffer to store the input data
    char input[2048];
    // Create a buffer to store the transaction data
    char transaction_data[1024] = "";
    // Get the first transaction in the block
    Transaction *current_transaction = block->transactions;
    // Concatenate the transaction data
    while (current_transaction != NULL)
    {
        // Create a temporary buffer to store the transaction data
        char temp[256];
        // Format the transaction data
        snprintf(temp, sizeof(temp), "%s%s%f", current_transaction->sender, current_transaction->receiver, current_transaction->amount);
        // Concatenate the transaction data
        strcat(transaction_data, temp);
        // Move to the next transaction
        current_transaction = current_transaction->next;
    }
    // Format the input data
    snprintf(input, sizeof(input), "%d%ld%s%s", block->index, block->timestamp, block->prev_hash, transaction_data);
    // Calculate the hash
    unsigned char hash[SHA256_DIGEST_LENGTH];
    // Calculate the SHA-256 hash
    SHA256((unsigned char*)input, strlen(input), hash);
    // Convert the hash to a string
    for(int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    {
        // Format the output
        sprintf(output + (i * 2), "%02x", hash[i]);
    }
    // Null-terminate the output
    output[SHA256_DIGEST_LENGTH*2] = '\0';
}

// Function to serialize transactions
char* serialize_transactions(Transaction *transaction)
{
    // Allocate memory for the serialized data
    char *data = (char*)malloc(256);
    // Format the transaction data
    snprintf(data, 256, "%s,%s,%f", transaction->sender, transaction->receiver, transaction->amount);
    return data;
}

// Function to create a new transaction
Transaction* create_transaction(const char *sender, const char *receiver, double amount)
{
    // Allocate memory for the transaction
    Transaction *transaction = (Transaction*)malloc(sizeof(Transaction));
    // Check if memory allocation was successful
    if (!transaction)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    // Copy the sender, receiver, and amount to the transaction
    strncpy(transaction->sender, sender, sizeof(transaction->sender) - 1);
    strncpy(transaction->receiver, receiver, sizeof(transaction->receiver) - 1);
    transaction->amount = amount;
    transaction->next = NULL;
    return transaction;
}


// Function to add a transaction to a block
void add_transaction(Block *block, Transaction *transaction)
{
    // Check if the block has any transactions
    if (block->transactions == NULL)
    {
        // Add the transaction to the block
        block->transactions = transaction;
    }
    else
    {
        // Find the last transaction in the block
        Transaction *current = block->transactions;
        // Iterate through the transactions
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = transaction;
    }
}

// Function to free transactions
void free_transactions(Transaction *transaction)
{
    // Free the memory allocated for the transactions
    Transaction *current = transaction;
    Transaction *next;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
}

// Function to create a new block
Block* create_block(int index, const char *data, const char *prev_hash)
{
    //Suppress unused parameter warning by casting to void
    (void)data;

    // Allocate memory for the block
    Block *block = (Block*)malloc(sizeof(Block));
    if (!block)
    {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    // Set the block data
    block->index = index;
    block->timestamp = time(NULL);
    block->transactions = NULL;
    // Create a transaction from the data
    strncpy(block->prev_hash, prev_hash, sizeof(block->prev_hash) - 1);
    // Calculate the hash of the block
    calculate_hash(block, block->hash);
    block->next = NULL;
    return block;
}

// Function to serialize the blockchain to JSON
char* serialize_blockchain_to_json(Block *blockchain)
{
    //Allocate memory for the JSON string
    char *json = malloc(1024 * 20);
    //Memory allocation check
    if (!json)
    {
        perror("malloc failed");
        return NULL;
    }

    // Start of JSON array
    strcpy(json, "[");
    // Pointer to the current block
    Block *current = blockchain;
    // Iterate through the blocks
    while (current != NULL)
    {
        //Buffer for a single block's JSON data
        char block_data[1024];
        // Serialize the current block's data into JSON format
        sprintf(block_data,
                "{\"index\":%d, \"timestamp\":%ld, \"prev_hash\":\"%s\", \"hash\":\"%s\"}",
                current->index,
                (long)current->timestamp,
                current->prev_hash,
                current->hash);
        // Concatenate the block data to the JSON string
        strcat(json, block_data);
        // Check if there are more blocks
        if (current->next != NULL) strcat(json, ", ");
        current = current->next;
    }
    // End of JSON array
    strcat(json, "]");
    return json;
}
// Function to add a block to the blockchain
Block* add_block(Block *head, const char *data)
{
    // Check if the blockchain is empty
    Block *new_block = create_block(head->index + 1, data, head->hash);
    head->next = new_block;
    return new_block;
}

// Function to print the blockchain
void print_blockchain(Block *head)
{
    // Pointer to the current block
    Block *current = head;
    // Iterate through the blocks
    while (current != NULL)
    {
        printf("Block %d\n", current->index);
        printf("Timestamp: %ld\n", current->timestamp);
        printf("Prev Hash: %s\n", current->prev_hash);
        printf("Hash: %s\n", current->hash);
        // Pointer to the current transaction
        Transaction *transaction = current->transactions;
        // Iterate through the transactions
        while (transaction != NULL)
        {
            printf("Transaction: %s -> %s : %f\n", transaction->sender, transaction->receiver, transaction->amount);
            transaction = transaction->next;
        }
        printf("\n");
        current = current->next;
    }
}

//Function to free blockchain
void free_blockchain(Block *head)
{
    // Pointer to the current block
    Block *current = head;
    // Pointer to the next block
    Block *next;
    // Iterate through the blocks
    while (current != NULL)
    {
        // Free the transactions
        next = current->next;
        free_transactions(current->transactions);
        free(current);
        current = next;
    }
}