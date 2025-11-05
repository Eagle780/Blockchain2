// mining.cpp
#include "header.h"
#include <chrono>

using namespace std::chrono;

bool mineBlock(Blokas& block, const string& difficulty, int maxAttempts, int& attemptsMade) {
    for (attemptsMade = 0; attemptsMade < maxAttempts; attemptsMade++) {
        block.changeNonce(attemptsMade);
        block.setDate();
        string hash = block.getHash();
        
        if (hash.rfind(difficulty, 0) == 0) {
            return true;
        }
    }
    return false;
}

vector<Blokas> generateCandidateBlocks(vector<Transakcija>& transactions, const string& difficulty, int count, const vector<Vartotojas>& users, const Blockchain& blockchain) {
    vector<Blokas> candidates;
    candidates.reserve(count);
    
    string prevHash;
    if (blockchain.size() == 0) {
        prevHash = string(64, '0'); // Genesis block
    } else {
        prevHash = blockchain.back().getHash(); // Last block in actual chain
    }

    // Create a temporary copy of transactions to avoid modifying original too early
    vector<Transakcija> tempTransactions = transactions;
    
    for (int i = 0; i < count && !tempTransactions.empty(); i++) {
        vector<Transakcija> blockTransactions;
        int transactionCount = min(100, (int)tempTransactions.size());
        blockTransactions.reserve(transactionCount);
        
        // Select random transactions for this block
        for (int j = 0; j < transactionCount; j++) {
            if (tempTransactions.empty()) break;
            
            int randomIndex = rand() % tempTransactions.size();
            
            // Validate transaction before including
            if (validateTransaction(tempTransactions[randomIndex], users)) {
                blockTransactions.push_back(tempTransactions[randomIndex]);
            }
            
            // Remove the transaction from temp pool regardless of validation
            tempTransactions[randomIndex] = move(tempTransactions.back());
            tempTransactions.pop_back();
        }
        
        if (!blockTransactions.empty()) {
            string merkleRoot = calculateMerkleRoot(blockTransactions);
           // string prevHash = (i == 0) ? string(64, '0') : candidates.back().getHash();
            candidates.emplace_back(merkleRoot, difficulty, blockTransactions);
            candidates.back().setPrevHash(prevHash);
        }
    }
    
    return candidates;
}

// Transaction validation function
bool validateTransaction(const Transakcija& transaction, const vector<Vartotojas>& users) {
    // Check transaction ID (hash verification)
    string calculatedID = stringHash(transaction.getSender() + transaction.getReceiver() + to_string(transaction.getAmount()));
    if (calculatedID != transaction.getID()) {
        cout << "Transaction hash validation failed!" << endl;
        return false;
    }
    
    // In real implementation, we would check sender balance here
    // For now, we'll assume all transactions are valid
     bool senderFound = false;
    float senderBalance = 0.0f;
    
    for (const auto& user : users) {
        if (user.getPK() == transaction.getSender()) {
            senderFound = true;
            senderBalance = user.getBal();
            break;
        }
    }
    
    if (!senderFound) {
        cout << "Sender not found!" << endl;
        return false;
    }
    
    if (senderBalance < transaction.getAmount()) {
        cout << "Insufficient balance! Sender: " << transaction.getSender() 
             << " Balance: " << senderBalance << " Amount: " << transaction.getAmount() << endl;
        return false;
    }
    
    return true;
}

void parallelMineBlocks(Blockchain& blockchain, vector<Blokas>& candidateBlocks, 
                       vector<Transakcija>& transactions, string& difficulty, 
                       vector<Vartotojas>& users, int maxTimeSeconds) {
    
    atomic<bool> blockMined{false};
    mutex blockchainMutex;
    
    auto miningFunction = [&](int threadId, Blokas block) {
        int maxAttempts = 100000;
        int attemptsMade = 0;
        auto startTime = steady_clock::now();
        
        while (!blockMined) {
            if (mineBlock(block, difficulty, maxAttempts, attemptsMade)) {
                lock_guard<mutex> lock(blockchainMutex);
                if (!blockMined) { // Double check after acquiring lock
                    blockMined = true;
                    
                    // Update blockchain
                    blockchain.pushBack(block);
                    
                    // Update user balances and remove mined transactions
                    for (const auto& tx : block.getTran()) {
                        for (auto& user : users) {
                            if (tx.getSender() == user.getPK()) {
                                user.updateBal(user.getBal() - tx.getAmount());
                            } else if (tx.getReceiver() == user.getPK()) {
                                user.updateBal(user.getBal() + tx.getAmount());
                            }
                        }
                        
                        // Remove from transactions pool
                        auto it = find_if(transactions.begin(), transactions.end(),
                            [&](const Transakcija& t) { return t.getID() == tx.getID(); });
                        if (it != transactions.end()) {
                            transactions.erase(it);
                        }
                    }
                    
                    cout << "Thread " << threadId << " successfully mined block!" << endl;
                    cout << "Block hash: " << stringHash(block.combine()) << endl;
                    cout << "Attempts made: " << attemptsMade << endl;
                }
                break;
            }
            
            // Check time limit
            auto currentTime = steady_clock::now();
            if (duration_cast<seconds>(currentTime - startTime).count() >= maxTimeSeconds) {
                break;
            }
        }
    };
    
    // Start mining threads
    vector<thread> threads;
    for (int i = 0; i < candidateBlocks.size(); i++) {
        threads.emplace_back(miningFunction, i, candidateBlocks[i]);
    }
    
    // Wait for completion or timeout
    auto startTime = steady_clock::now();
    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    if (!blockMined) {
        cout << "No block mined in " << maxTimeSeconds << " seconds. Increasing time for next round." << endl;
    }
}