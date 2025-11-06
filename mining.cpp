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
        prevHash = string(64, '0'); 
    } else {
        prevHash = blockchain.back().getHash();
    }

    vector<Transakcija> tempTransactions = transactions;
    
    for (int i = 0; i < count && !tempTransactions.empty(); i++) {
        vector<Transakcija> blockTransactions;
        int transactionCount = min(100, (int)tempTransactions.size());
        blockTransactions.reserve(transactionCount);
    
        for (int j = 0; j < transactionCount; j++) {
            if (tempTransactions.empty()) break;
            
            int randomIndex = rand() % tempTransactions.size();
            
            if (validateTransaction(tempTransactions[randomIndex], users)) {
                blockTransactions.push_back(tempTransactions[randomIndex]);
            }
            
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

bool validateTransaction(const Transakcija& transaction, const vector<Vartotojas>& users) {
    string calculatedID = stringHash(transaction.getSender() + transaction.getReceiver() + to_string(transaction.getAmount()));
    if (calculatedID != transaction.getID()) {
        cout << "Transaction hash validation failed!" << endl;
        return false;
    }

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
                if (!blockMined) { 
                    blockMined = true;
    
                    blockchain.pushBack(block);
             
                    for (const auto& tx : block.getTran()) {
                        for (auto& user : users) {
                            if (tx.getSender() == user.getPK()) {
                                user.updateBal(user.getBal() - tx.getAmount());
                            } else if (tx.getReceiver() == user.getPK()) {
                                user.updateBal(user.getBal() + tx.getAmount());
                            }
                        }
           
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
            
            auto currentTime = steady_clock::now();
            if (duration_cast<seconds>(currentTime - startTime).count() >= maxTimeSeconds) {
                break;
            }
        }
    };
    
    vector<thread> threads;
    for (int i = 0; i < candidateBlocks.size(); i++) {
        threads.emplace_back(miningFunction, i, candidateBlocks[i]);
    }

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