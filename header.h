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

class Vartotojas
{
private:
    string vardas;
    string public_key; // 64 simboliu ilgio random simboliu stringas
    float balansas;

public:
    Vartotojas(string p_k)
    {
        vardas = randomVardas();
        public_key = p_k;
        balansas = (rand() % 999901) + 100;
    }
    string randomVardas()
    {
        vector<string> vardai = {"Jonas", "Antanas", "Petras", "Dovydas", "Tomas", "Greta", "Dominyka", "Ramune", "Ieva", "Migle"};
        string vardas = vardai[rand() % vardai.size()];
        return vardas;
    }
    void updateBal(float num)
    {
        balansas = num;
    }
    inline string getVardas() const { return vardas; }
    inline string getPK() const { return public_key; }
    inline float getBal() const { return balansas; }
};

class Transakcija
{
private:
    string transaction_id;
    string sender;
    string receiver;
    float amount;
    vector<string> inputUTXOs;  // UTXOs being spent
    vector<pair<string, float>> outputs; // New UTXOs being created

public:
    Transakcija(string s, string r, float a)
    {
        sender = s;
        receiver = r;
        amount = a;
        transaction_id = stringHash(s + r + to_string(a));
    }

    Transakcija(string s, string r, float a, 
                const vector<string>& inputs, 
                const vector<pair<string, float>>& outputList) {
        sender = s;
        receiver = r;
        amount = a;
        inputUTXOs = inputs;
        outputs = outputList;
        
        // Create transaction ID from all components
        string txData = s + r + to_string(a);
        for (const auto& input : inputs) {
            txData += input;
        }
        for (const auto& output : outputs) {
            txData += output.first + to_string(output.second);
        }
        transaction_id = stringHash(txData);
    }
    inline string getID() const { return transaction_id; }
    inline string getSender() const { return sender; }
    inline string getReceiver() const { return receiver; }
    inline float getAmount() const { return amount; }
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
            sum += tr.getAmount();
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

// UTXO model
class UTXO {
private:
    string txId;
    int outputIndex;  // Which output in the transaction
    string owner;
    float amount;
    bool spent;

public:
    UTXO(const string& id, int index, const string& own, float amt) 
        : txId(id), outputIndex(index), owner(own), amount(amt), spent(false) {}
    
    string getTxId() const { return txId; }
    int getOutputIndex() const { return outputIndex; }
    string getOwner() const { return owner; }
    float getAmount() const { return amount; }
    bool isSpent() const { return spent; }
    void markSpent() { spent = true; }
    string getUTXOId() const { return txId + "_" + to_string(outputIndex); }
};

// Global UTXO pool
extern vector<UTXO> utxoPool;