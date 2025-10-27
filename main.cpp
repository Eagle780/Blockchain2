#include "header.h"

int main()
{
    vector<Vartotojas> vartotojai;
    vector<string> pkvector;
    vector<Transakcija> transakcijos;
    Blockchain blockchain;
    srand(time(0));
    for (int i = 0; i < 10; i++)
    {
        string pk = generuotiPK(pkvector);
        Vartotojas var(pk);
        vartotojai.push_back(var);
    }
    for (int i = 0; i < 20; i++)
    {
        Transakcija tr = generuotiTransakcija(vartotojai, transakcijos);
        transakcijos.push_back(tr);
    }
    cout << "sugeneruotos transakcijos: ";
    for (auto tran : transakcijos)
    {
        cout << tran.getAmount() << " ";
    }
    cout << endl;
    while (!transakcijos.empty())
    {
        Blokas b = formuotiBloka(transakcijos);
        kastiBloka(blockchain, b, transakcijos);
    }
    blockchain.print();
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

Blokas formuotiBloka(vector<Transakcija> tran)
{
    vector<Transakcija> tr;
    for (int i = 0; i < 10; i++)
    {
        int num = rand() % tran.size();
        tr.push_back(tran[num]);
        tran.erase(tran.begin() + num);
    }
    cout << "pasirinktos transakcijos: ";
    for (auto t1 : tr)
    {
        cout << t1.getAmount() << " ";
    }
    cout << endl;
    return Blokas(visuTranHash(tr), 1, tr);
    // TODO: bloko kintamuosius passint i pacia funk
}

string visuTranHash(vector<Transakcija> tr)
{
    string hash = tr[0].getID();
    int n = tr.size();
    for (int i = 1; i < n; i++)
    {
        hash = stringHash(hash + tr[i].getID());
    }
    return hash;
}

void kastiBloka(Blockchain &b, Blokas a, vector<Transakcija> &tr)
{
    for (int i = 0; i < 10000; i++)
    {
        a.changeNonce(i);
        a.setDate();
        string hash = stringHash(a.combine());
        if (hash[0] == '0' && hash[1] == '0' && hash[2] == '0')
        {
            a.setDate();
            b.pushBack(a);
            cout << "yo" << endl;
            for (auto tran : a.getTran())
            {
                for (int i = 0; i < tr.size(); i++)
                {
                    if (tr[i].getID() == tran.getID())
                    {
                        tr.erase(tr.begin() + i);
                        continue;
                    }
                }
            }
            return;
        }
    }
}