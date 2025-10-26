#include "header.h"

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
    cout << "-----" << endl;
    formuotiBloka(transakcijos);
    string input = "hsdjhf";
    string output = stringHash(input);
    cout << output << endl;
    return 0;
}

string generuotiPK(vector<string> &pkvec)
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

Transakcija generuotiTransakcija(vector<Vartotojas> &var, vector<Transakcija> &tran)
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
    int bal1 = var[s].getBal() - a;
    var[s].updateBal(bal1);
    int bal2 = var[r].getBal() + a;
    var[r].updateBal(bal2);

    return tr;
}

void formuotiBloka(vector<Transakcija> &tran)
{
    vector<Transakcija> tr;
    for (int i = 0; i < 10; i++)
    {
        int num = rand() % tran.size();
        tr.push_back(tran[num]);
        tran.erase(tran.begin() + num);
    }
    for (auto t1 : tr)
    {
        cout << t1.getAmount() << " ";
    }
    cout << endl;
    for (auto t2 : tran)
    {
        cout << t2.getAmount() << " ";
    }
    cout << endl;
}