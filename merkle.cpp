// merkle.cpp
#include "header.h"

shared_ptr<MerkleNode> buildMerkleTree(const vector<string>& hashes) {
    if (hashes.empty()) {
        return make_shared<MerkleNode>(stringHash(""));
    }
    
    vector<shared_ptr<MerkleNode>> nodes;
    for (const auto& hash : hashes) {
        nodes.push_back(make_shared<MerkleNode>(hash));
    }
    
    while (nodes.size() > 1) {
        vector<shared_ptr<MerkleNode>> newLevel;
        
        for (size_t i = 0; i < nodes.size(); i += 2) {
            if (i + 1 < nodes.size()) {
                string combinedHash = stringHash(nodes[i]->hash + nodes[i + 1]->hash);
                newLevel.push_back(make_shared<MerkleNode>(combinedHash, nodes[i], nodes[i + 1]));
            } else {
                // Odd number of nodes, duplicate the last one
                string combinedHash = stringHash(nodes[i]->hash + nodes[i]->hash);
                newLevel.push_back(make_shared<MerkleNode>(combinedHash, nodes[i], nodes[i]));
            }
        }
        
        nodes = move(newLevel);
    }
    
    return nodes[0];
}

string calculateMerkleRoot(const vector<Transakcija>& transactions) {
    if (transactions.empty()) {
        return stringHash("");
    }
    
    vector<string> transactionHashes;
    for (const auto& tx : transactions) {
        transactionHashes.push_back(tx.getID());
    }
    
    auto merkleRootNode = buildMerkleTree(transactionHashes);
    return merkleRootNode->hash;
}