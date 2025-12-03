#pragma once

#include <iostream>
#include <functional>
#include <vector>
#include <random>
#include <string>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <list>
#include <ctime>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <memory>
#include <algorithm>

using namespace std;

class Vartotojas;
class Transakcija;
class Blokas;
class Blockchain;
struct MerkleNode;

string stringHash(string str);

struct MerkleNode
{
    string hash;
    shared_ptr<MerkleNode> left;
    shared_ptr<MerkleNode> right;

    MerkleNode(const string &h) : hash(h), left(nullptr), right(nullptr) {}
    MerkleNode(const string &h, shared_ptr<MerkleNode> l, shared_ptr<MerkleNode> r)
        : hash(h), left(l), right(r) {}
};

shared_ptr<MerkleNode> buildMerkleTree(const vector<string> &hashes);
string calculateMerkleRoot(const vector<Transakcija> &transactions);

class TxOut
{
private:
    string receiver;
    int amount;

public:
    TxOut(const string &rec, const int &am) : receiver(rec), amount(am) {}
    string getReceiver() const { return receiver; }
    int getAmount() const { return amount; }
};

class TxIn
{
private:
    string prev_tx_id;
    int index;

public:
    TxIn(const string &id, const int &index) : prev_tx_id(id), index(index) {}
    string getPrevId() const { return prev_tx_id; }
    int getPrevIndex() const { return index; }
};

class Transakcija
{
private:
    string tx_hash;
    vector<TxIn> inputs;
    vector<TxOut> outputs;

public:
    Transakcija(const string &hash, const vector<TxIn> &in, vector<TxOut> out) : tx_hash(hash), inputs(in), outputs(out) {}
    Transakcija(const string &hash) : tx_hash(hash) {}
    string getTxHash() const { return tx_hash; }
    vector<TxIn> getInputs() const { return inputs; }
    vector<TxOut> getOutputs() const { return outputs; }
    vector<TxIn> setInputs(vector<TxIn> in) { inputs = in; }
    vector<TxOut> setOutputs(vector<TxOut> out) { outputs = out; }
};

// UTXO (Unspent Transaction Output) structure
struct UTXO
{
    string txId;     // ID of the transaction that created this output
    int outputIndex; // Index of this output in the transaction
    string owner;    // Public key of the owner
    int amount;      // Amount in this output
    bool spent;      // Whether this UTXO has been spent

    // patikrink, ar jei transakcija jau egzistuoja, naudoja jos hasha, kuris sudarytas is visu kitu jos elementu hashu
    UTXO(const string &id, int idx, const string &own, int amt)
        : txId(id), outputIndex(idx), owner(own), amount(amt), spent(false) {}
    string getId() const { return txId + "_" + to_string(outputIndex); }
};

// Global UTXO pool - tracks all unspent outputs
extern vector<UTXO> utxoPool;

class Vartotojas
{
private:
    string vardas;
    string public_key;

public:
    Vartotojas(string p_k)
    {
        vardas = randomVardas();
        public_key = p_k;
    }
    string randomVardas()
    {
        vector<string> vardai = {"Jonas", "Antanas", "Petras", "Dovydas", "Tomas", "Greta", "Dominyka", "Ramune", "Ieva", "Migle"};
        string vardas = vardai[rand() % vardai.size()];
        return vardas;
    }
    inline string getVardas() const { return vardas; }
    inline string getPK() const { return public_key; }
    inline float getBal() const { return calculateBalance(public_key); }
};

class Blokas
{
private:
    string prevHash;
    string date;
    string version;
    string merkleRoot;
    int nonce;
    string difficulty;
    vector<Transakcija> transakcijos;

public:
    Blokas(string mr, string di, vector<Transakcija> tr)
    {
        prevHash = "";
        version = "1.0";
        merkleRoot = mr;
        nonce = 0;
        difficulty = di;
        transakcijos = tr;
        setDate();
    }
    void changeNonce(int n)
    {
        nonce = n;
    }
    void setDate()
    {
        auto t = chrono::system_clock::now();
        time_t data = chrono::system_clock::to_time_t(t);
        date = ctime(&data);
    }
    void setPrevHash(string hash)
    {
        prevHash = hash;
    }
    void setDiff(string diff)
    {
        difficulty = diff;
    }
    string combine() const
    {
        return prevHash + date + version + merkleRoot + to_string(nonce) + difficulty;
    }
    string getHash() const
    {
        return stringHash(combine());
    }
    string calculateMerkleRoot() const
    {
        return ::calculateMerkleRoot(transakcijos);
    }
    const vector<Transakcija> &getTran() const
    {
        return transakcijos;
    }
    float sumTr() const
    {
        int sum = 0;
        for (auto const &tr : transakcijos)
        {
            int trSum = 0;
            for (auto const &out : tr.getOutputs())
            {
                trSum += out.getAmount();
            }
            sum += trSum;
        }
        return sum;
    }
    void print() const
    {
        cout << "prevHash: " << prevHash << endl;
        cout << "date: " << date;
        cout << "version: " << version << endl;
        cout << "merkleRoot: " << merkleRoot << endl;
        cout << "nonce: " << nonce << endl;
        cout << "difficulty: " << difficulty << endl;
        cout << "transactions count: " << transakcijos.size() << endl;
        cout << "}" << endl;
        cout << endl;
    }
};

class Blockchain
{
private:
    list<Blokas> list;

public:
    Blockchain() : list() {}
    void pushBack(Blokas a)
    {
        list.push_back(a);
    }
    void print() const
    {
        int n = 0;
        for (auto blokas : list)
        {
            if (list.size() - n <= 10)
            {
                cout << "{" << endl;
                cout << "heigh: " << n << endl;
                cout << "depth: " << list.size() - n << endl;
                cout << "total amount: " << blokas.sumTr() << endl;
                cout << "transactions: " << blokas.getTran().size() << endl;
                cout << "avg amount: " << blokas.sumTr() * 1.0 / blokas.getTran().size() << endl;
                blokas.print();
            }
            n++;
        }
    }
    int size() const
    {
        return list.size();
    }
    Blokas back() const
    {
        return list.back();
    }
};

string generuotiPK(vector<string> &pkvec);
Transakcija generuotiTransakcija(vector<Vartotojas> &var);
Blokas formuotiBloka(vector<Transakcija> &tran, const string &diff);
string visuTranHash(const vector<Transakcija> &tr);
void kastiBloka(Blockchain &b, Blokas a, vector<Transakcija> &tr, string &diff, vector<Vartotojas> &var);

bool mineBlock(Blokas &block, const string &difficulty, int maxAttempts, int &attemptsMade);
vector<Blokas> generateCandidateBlocks(vector<Transakcija> &transactions, const string &difficulty, int count, const vector<Vartotojas> &user, const Blockchain &bockchain);
void parallelMineBlocks(Blockchain &blockchain, vector<Blokas> &candidateBlocks, vector<Transakcija> &transactions,
                        string &difficulty, vector<Vartotojas> &users, int maxTimeSeconds);

bool validateTransaction(const Transakcija &transaction, const vector<Vartotojas> &users);

float calculateBalance(string p_k);
void initializeUTXOPool(const vector<Vartotojas> &users);
void spendUTXOs(const vector<TxIn> &inputs);
void addNewUTXOs(const string &txId, const vector<TxOut> &outputs);

// UTXO model
/*
class UTXO
{
private:
    string txId;
    int output_index;
    string owner;
    float amount;
    bool spent;

public:
    UTXO(const string &id, const int &in, const string &own, float amt) : txId(id), output_index(in), owner(own), amount(amt), spent(false) {}
    string getTxId() const { return txId; }
    int getOutputIn() const { return output_index; }
    string getOwner() const { return owner; }
    float getAmount() const { return amount; }
    bool isSpent() const { return spent; }
    void markSpent() { spent = true; }
};

// Global UTXO pool
extern
*/