#define _GLIBCXX_USE_CXX11_ABI 0

#include <iostream>
#include <functional>
#include <vector>
#include <random>

using namespace std;

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
    }
    inline string getID() const { return transaction_id; }
    inline string getSender() const { return sender; }
    inline string getReceiver() const { return receiver; }
    inline float getAmount() const { return amount; }
};

string generuotiPK(vector<string> pkvec);
Transakcija generuotiTransakcija(vector<Vartotojas> var, vector<Transakcija> tr);

int main()
{
    vector<Vartotojas> vartotojai;
    vector<string> pkvector;
    vector<Transakcija> transakcijos;
    srand(time(0));
    for (int i = 0; i < 10; i++)
    {
        string pk = generuotiPK(pkvector);
        Vartotojas var(pk);
        vartotojai.push_back(var);
    }
    for (auto var : vartotojai)
    {
        cout << var.getPK() << " " << var.getBal() << endl;
    }
    for (int i = 0; i < 10; i++)
    {
        Transakcija tr = generuotiTransakcija(vartotojai, transakcijos);
        transakcijos.push_back(tr);
        for (int i = 0; i < vartotojai.size(); i++)
        {
            if (tr.getSender() == vartotojai[i].getPK())
            {
                int bal = vartotojai[i].getBal() - tr.getAmount();
                vartotojai[i].updateBal(bal);
            }
            else if (tr.getReceiver() == vartotojai[i].getPK())
            {
                int bal = vartotojai[i].getBal() + tr.getAmount();
                vartotojai[i].updateBal(bal);
            }
        }
    }
    for (auto tran : transakcijos)
    {
        cout << tran.getSender() << endl;
        cout << tran.getReceiver() << endl;
        cout << tran.getAmount() << endl;
    }
    for (auto var : vartotojai)
    {
        cout << var.getPK() << " " << var.getBal() << endl;
    }
    return 0;
}

string generuotiPK(vector<string> pkvec)
{
    string pk;
    char a;
    while (true)
    {
        pk = "";
        for (int i = 0; i < 64; i++)
        {
            a = (rand() % 94) + 33;
            pk += a;
        }
        for (auto pkv : pkvec)
        {
            if (pkv == pk)
            {
                continue;
            }
        }
        break;
    }
    pkvec.push_back(pk);
    return pk;
}

Transakcija generuotiTransakcija(vector<Vartotojas> var, vector<Transakcija> tran)
{
    int s = rand() % var.size();
    int r = rand() % var.size();
    while (var[s].getPK() == var[r].getPK())
    {
        r = rand() % var.size();
    }
    float a = rand() % int(var[s].getBal());
    if (a == 0.0)
    {
        a = 1.0;
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);
        while (a >= var[s].getBal())
        {
            a = dis(gen);
        }
    }

    Transakcija tr(var[s].getPK(), var[r].getPK(), a);

    return tr;
}