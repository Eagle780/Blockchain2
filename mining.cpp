#include "header.h"
#include <chrono>

using namespace std::chrono;

bool mineBlock(Blokas &block, const string &difficulty, int maxAttempts, int &attemptsMade)
{
    for (int i = 0; i < maxAttempts; i++)
    {
        block.changeNonce(attemptsMade);
        block.setDate();
        string hash = block.getHash();

        if (hash.rfind(difficulty, 0) == 0)
        {
            return true;
        }
        attemptsMade++;
    }
    return false;
}

vector<Blokas> generateCandidateBlocks(vector<Transakcija> &transactions, const string &difficulty, int count, const vector<Vartotojas> &users, const Blockchain &blockchain)
{
    vector<Blokas> candidates;
    candidates.reserve(count);

    string prevHash;
    if (blockchain.size() == 0)
    {
        prevHash = string(64, '0');
    }
    else
    {
        prevHash = blockchain.back().getHash();
    }

    vector<Transakcija> tempTransactions = transactions;

    for (int i = 0; i < count && !tempTransactions.empty(); i++)
    {
        vector<Transakcija> blockTransactions;
        int transactionCount = min(100, (int)tempTransactions.size());
        blockTransactions.reserve(transactionCount);

        for (int j = 0; j < transactionCount; j++)
        {
            if (tempTransactions.empty())
                break;

            int randomIndex = rand() % tempTransactions.size();

            if (validateTransaction(tempTransactions[randomIndex], users))
            {
                blockTransactions.push_back(tempTransactions[randomIndex]);
            }

            tempTransactions[randomIndex] = move(tempTransactions.back());
            tempTransactions.pop_back();
        }

        if (!blockTransactions.empty())
        {
            string merkleRoot = calculateMerkleRoot(blockTransactions);
            // string prevHash = (i == 0) ? string(64, '0') : candidates.back().getHash();
            candidates.emplace_back(merkleRoot, difficulty, blockTransactions);
            candidates.back().setPrevHash(prevHash);
        }
    }

    return candidates;
}

bool validateTransaction(const Transakcija &transaction, const vector<Vartotojas> &users)
{
    // Check if transaction has inputs and outputs
    vector<TxIn> inputs = transaction.getInputs();
    vector<TxOut> outputs = transaction.getOutputs();
    
    if (inputs.empty() || outputs.empty()) {
        return false;
    }
    
    // Validate that all inputs reference valid UTXOs
    int totalInput = 0;
    for (const auto& input : inputs) {
        bool found = false;
        for (const auto& utxo : utxoPool) {
            if (utxo.txId == input.getPrevId() && 
                utxo.outputIndex == input.getPrevIndex() && 
                !utxo.spent) {
                totalInput += utxo.amount;
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Input UTXO not found or already spent!" << endl;
            return false;
        }
    }
    
    // Validate that outputs don't exceed inputs (no money creation)
    int totalOutput = 0;
    for (const auto& output : outputs) {
        totalOutput += output.getAmount();
    }
    
    if (totalOutput > totalInput) {
        cout << "Transaction output (" << totalOutput << ") exceeds input (" << totalInput << ")!" << endl;
        return false;
    }
    
    return true;
}

void parallelMineBlocks(Blockchain &blockchain, vector<Blokas> &candidateBlocks,
                        vector<Transakcija> &transactions, string &difficulty,
                        vector<Vartotojas> &users, int maxTimeSeconds)
{

    atomic<bool> blockMined{false};
    mutex blockchainMutex;

    auto miningFunction = [&](int threadId, Blokas block)
    {
        int maxAttempts = 100000;
        int attemptsMade = 100000 * threadId;
        auto startTime = steady_clock::now();

        while (!blockMined)
        {
            if (mineBlock(block, difficulty, maxAttempts, attemptsMade))
            {
                lock_guard<mutex> lock(blockchainMutex);
                if (!blockMined)
                {
                    blockMined = true;

                    blockchain.pushBack(block);

                    for (const auto &tx : block.getTran())
                    {
                        // Process UTXO changes for this transaction
                        spendUTXOs(tx.getInputs());
                        addNewUTXOs(tx.getTxHash(), tx.getOutputs());
                        
                        // Remove transaction from pending pool
                        auto it = find_if(transactions.begin(), transactions.end(),
                                          [&](const Transakcija &t)
                                          { return t.getTxHash() == tx.getTxHash(); });
                        if (it != transactions.end())
                        {
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
            if (duration_cast<seconds>(currentTime - startTime).count() >= maxTimeSeconds)
            {
                break;
            }
        }
    };

    vector<thread> threads;
    for (int i = 0; i < candidateBlocks.size(); i++)
    {
        threads.emplace_back(miningFunction, i, candidateBlocks[i]);
    }

    auto startTime = steady_clock::now();
    for (auto &t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    if (!blockMined)
    {
        cout << "No block mined in " << maxTimeSeconds << " seconds. Increasing time for next round." << endl;
    }
}