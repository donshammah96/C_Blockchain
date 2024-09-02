# Blockchain Project

#### Video Demo: [https://youtu.be/w1kc3rBdTx4]

## Overview

This project implements a basic blockchain in C, featuring a linked list of blocks containing transactions, hash computation using SHA-256, and networking capabilities to start a server and connect to it. The server can send the entire blockchain in JSON format to any client that connects. The project aims to provide a fundamental understanding of how a blockchain works, including block creation, transaction handling, hashing for data integrity, and simple client-server communication for blockchain transmission.

## Files

### `blockchain.c`
This file contains the core functionality of the blockchain, including the definition of the block and transaction structures, functions for creating transactions and blocks, calculating hashes, adding transactions to blocks, serializing the blockchain to JSON, and freeing allocated memory.

- **Structures**:
  - `Transaction`: Defines a transaction with fields for the sender, receiver, amount, and a pointer to the next transaction.
  - `Block`: Defines a block with fields for the index, timestamp, transactions, previous hash, current hash, and a pointer to the next block.

- **Functions**:
  - `calculate_hash`: Computes the SHA-256 hash of a block by concatenating its index, timestamp, previous hash, and transactions.
  - `serialize_transactions`: Serializes a transaction into a string format.
  - `create_transaction`: Allocates and initializes a new transaction.
  - `add_transaction`: Adds a transaction to a block.
  - `free_transactions`: Frees the memory allocated for transactions in a block.
  - `create_block`: Allocates and initializes a new block, calculates its hash.
  - `serialize_blockchain_to_json`: Serializes the entire blockchain into JSON format for easy transmission.
  - `add_block`: Adds a new block to the blockchain.
  - `print_blockchain`: Prints the details of each block and its transactions.
  - `free_blockchain`: Frees the memory allocated for the entire blockchain.

### `network.c`
This file manages the networking aspects of the project, allowing the blockchain to be sent over a network and received by a client. It includes functions to start a server, send the blockchain, and connect to a server to receive the blockchain.

- **Functions**:
  - `send_blockchain`: Serializes the blockchain to JSON and sends it over a socket.
  - `start_server`: Starts a server that listens for incoming connections and sends the blockchain to any connected client. Uses pthreads for handling multiple clients concurrently.
  - `connect_to_server`: Connects to a server to receive the blockchain.
  - `receive_data`: Helper function to receive data from a socket.
  - `receive_blockchain`: Receives the blockchain from a server and reconstructs it in memory.

### `main.c`
The entry point of the application. This file creates the genesis block and adds subsequent blocks with transactions. It starts the server in a separate thread and attempts to connect to the server to receive the blockchain.

- **Functions**:
  - `main`: Creates the initial blockchain, starts the server, and connects to the server to receive the blockchain. It then prints the received blockchain and frees the allocated memory.

## Design Choices

### Blockchain Data Structure
The blockchain is implemented as a linked list where each block points to the next block in the chain. This structure was chosen for its simplicity and ease of traversal when printing or sending the blockchain. Each block contains an index, timestamp, list of transactions, previous block hash, and its own hash. The linked list approach allows dynamic addition of blocks without reallocating memory for the entire chain.

### Hashing
SHA-256 from the OpenSSL library is used for hashing each block's data. This choice was made due to SHA-256's widespread use in blockchain technology, providing a robust and secure way to ensure data integrity. The `calculate_hash` function concatenates the block's index, timestamp, previous hash, and transactions into a single string, which is then hashed to produce the block's hash.

### Networking
The networking component allows the blockchain to be shared between a server and clients. The server listens for incoming connections and sends the blockchain in JSON format. JSON was chosen for its human-readable format and ease of parsing. The `start_server` function uses pthreads to handle multiple clients concurrently, ensuring scalability and responsiveness.

### Error Handling and Memory Management
Careful attention was given to error handling and memory management. All dynamic memory allocations are checked for success, and appropriate error messages are displayed if an allocation fails. Functions such as `free_transactions` and `free_blockchain` ensure that all allocated memory is freed, preventing memory leaks.

### Multithreading
Initially, the server was implemented using `fork()` to handle multiple clients, but this was changed to use `pthread` for better performance and scalability. Threads allow concurrent handling of clients without the overhead of creating new processes, leading to more efficient resource usage.

## Conclusion

This project provides a foundational understanding of blockchain technology, including block creation, transaction handling, data integrity through hashing, and basic client-server communication. The choice of data structures, hashing algorithms, and networking techniques reflects a balance between simplicity and functionality, making the project both educational and practical. Future improvements could include enhanced error handling, blockchain verification, and more sophisticated networking protocols.