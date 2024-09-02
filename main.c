#include "blockchain.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    // Create the genesis block
    Block *genesis_block = create_block(0, "0", "0");
    add_transaction(genesis_block, create_transaction("Don", "Pam", 50.0));
    add_transaction(genesis_block, create_transaction("Nikki", "Sauce", 25.0));

    // Add subsequent blocks
    Block *blockchain = genesis_block;
    Block *new_block = create_block(blockchain->index + 1, serialize_transactions(blockchain->transactions), blockchain->hash);
    add_transaction(new_block, create_transaction("Sauce", "Don", 15.0));
    blockchain->next = new_block;

    // Start server in a separate thread
    if (fork() == 0)
    {
        start_server(blockchain);
        exit(0);
    }

    // Connect to server to receive blockchain
    Block *received_blockchain = NULL;
    connect_to_server("127.0.0.1", 8080, &received_blockchain);
    print_blockchain(received_blockchain);

    // Free the blockchain
    free_blockchain(genesis_block);
    return 0;
}