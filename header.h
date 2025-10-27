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

using namespace std;

string stringHash(string str);

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

public:
    Transakcija(string s, string r, float a)
    {
        sender = s;
        receiver = r;
        amount = a;
        transaction_id = stringHash(s + r + to_string(a));
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
    string tranIDHash;
    int nonce;
    string difficulty;
    vector<Transakcija> transakcijos;

public:
    Blokas(string tIDh, string di, vector<Transakcija> tr)
    {
        prevHash = string(64, '0');
        version = "1.0";
        tranIDHash = tIDh;
        nonce = 0;
        difficulty = di;
        transakcijos = tr;
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
    string combine()
    {
        return prevHash + date + version + tranIDHash + to_string(nonce) + difficulty;
    }
    const vector<Transakcija> &getTran() const
    {
        return transakcijos;
    }
    string print()
    {
        return (prevHash + " " + date + " " + version + " " + tranIDHash + " " + to_string(nonce) + " " + difficulty);
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
    void print()
    {
        for (auto blokas : list)
        {
            cout << blokas.print() << endl;
        }
    }
    int size()
    {
        return list.size();
    }
    Blokas back()
    {
        return list.back();
    }
};

string generuotiPK(vector<string> &pkvec);
Transakcija generuotiTransakcija(vector<Vartotojas> &var);
Blokas formuotiBloka(vector<Transakcija> tran, const string &diff);
string visuTranHash(const vector<Transakcija> &tr);
void kastiBloka(Blockchain &b, Blokas a, vector<Transakcija> &tr, string &diff);