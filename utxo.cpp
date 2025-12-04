#include "header.h"

vector<UTXO> utxoPool;

void initializeUTXOPool(const vector<Vartotojas> &users)
{
    // Create initial UTXOs for each user with a random starting balance
    for (const auto &user : users)
    {
        int initialBalance = (rand() % 999901) + 100;
        // Create a genesis transaction ID for initial UTXOs
        string genesisId = stringHash("GENESIS_" + user.getPK());
        UTXO initialUtxo(genesisId, 0, user.getPK(), initialBalance);
        utxoPool.push_back(initialUtxo);
    }
}

float calculateBalance(const string &p_k)
{
    float balance = 0;
    for (const auto &utxo : utxoPool)
    {
        if (utxo.owner == p_k && !utxo.spent)
        {
            balance += utxo.amount;
        }
    }
    return balance;
}

void spendUTXOs(const vector<TxIn> &inputs)
{
    // Mark UTXOs as spent when they are used as inputs
    for (const auto &input : inputs)
    {
        for (auto &utxo : utxoPool)
        {
            if (utxo.txId == input.getPrevId() && utxo.outputIndex == input.getPrevIndex())
            {
                utxo.spent = true;
                break;
            }
        }
    }
}

void addNewUTXOs(const string &txId, const vector<TxOut> &outputs)
{
    // Add new UTXOs to the pool from transaction outputs
    for (int i = 0; i < outputs.size(); i++)
    {
        UTXO newUtxo(txId, i, outputs[i].getReceiver(), outputs[i].getAmount());
        utxoPool.push_back(newUtxo);
    }
}