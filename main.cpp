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
    int balansas;

public:
    Vartotojas(string p_k)
    {
        vardas = randomVardas();
        public_key = p_k;
        balansas = (rand() % 999901) + 100;
    }
    string randomVardas()
    {
        vector<string> vardai = {"Jonas", "Antanas", "Petras", "Dovydas", "Tomas", "Greta", "Dominyka", "Ramūnė", "Ieva", "Miglė"};
        string vardas = vardai[rand() % vardai.size()];
        return vardas;
    }
    inline string getVardas() const { return vardas; }
    inline string getPK() const { return public_key; }
    inline int getBal() const { return balansas; }
};

string generuotiPK(vector<string> pkvec);

int main()
{
    vector<Vartotojas> vartotojai;
    vector<string> pkvector;
    srand(time(0));
    for (int i = 0; i < 10; i++)
    {
        string pk = generuotiPK(pkvector);
        Vartotojas var(pk);
        vartotojai.push_back(var);
    }
    for (auto var : vartotojai)
    {
        cout << var.getVardas() << " " << var.getPK() << " " << var.getBal() << endl;
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
