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
        transaction_id = "";
        // TODO hashavimas
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
    int difficulty;

public:
    Blokas(string ph, string d, string v, string tIDh, int di)
    {
        prevHash = ph;
        date = d;
        version = v;
        tranIDHash = tIDh;
        nonce = 0;
        difficulty = di;
    }
    void changeNonce(int n)
    {
        nonce = n;
    }
};

class Blockchain
{
private:
    std::list<Blokas> list;

public:
    // metodai
};

string generuotiPK(vector<string> &pkvec);
Transakcija generuotiTransakcija(vector<Vartotojas> &var, vector<Transakcija> &tr);
void formuotiBloka(vector<Transakcija> &tran);