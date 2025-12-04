#include "header.h"

vector<UTXO> utxoPool;

// Initialize UTXO pool with initial funds for users
void initializeUTXOPool(const vector<Vartotojas>& users) {
    utxoPool.clear();
    for (const auto& user : users) {
        // Create initial UTXOs for each user
        string initialTxId = "genesis_" + user.getPK();
        utxoPool.emplace_back(initialTxId, 0, user.getPK(), user.getBal());
    }
}

// Find all UTXOs owned by a specific public key
vector<UTXO*> findUTXOsByOwner(const string& owner) {
    vector<UTXO*> result;
    for (auto& utxo : utxoPool) {
        if (utxo.getOwner() == owner && !utxo.isSpent()) {
            result.push_back(&utxo);
        }
    }
    return result;
}

// Get total balance from UTXOs
float getBalanceFromUTXO(const string& owner) {
    float balance = 0.0f;
    for (const auto& utxo : utxoPool) {
        if (utxo.getOwner() == owner && !utxo.isSpent()) {
            balance += utxo.getAmount();
        }
    }
    return balance;
}

// Mark UTXOs as spent
void markUTXOsAsSpent(const vector<string>& utxoIds) {
    for (const auto& utxoId : utxoIds) {
        for (auto& utxo : utxoPool) {
            if (utxo.getUTXOId() == utxoId && !utxo.isSpent()) {
                utxo.markSpent();
                break;
            }
        }
    }
}

// Add new UTXOs to the pool
void addUTXOsToPool(const vector<UTXO>& newUTXOs) {
    utxoPool.insert(utxoPool.end(), newUTXOs.begin(), newUTXOs.end());
}