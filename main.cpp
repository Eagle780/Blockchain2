#include "header.h"

int main()
{
    vector<Vartotojas> vartotojai;
    vector<string> pkvector;
    vector<Transakcija> transakcijos;
    Blockchain blockchain;
    string diff = string(3, '0');
    cout << "priekis" << endl;
    srand(time(0));
    int varInt = 100;
    int trInt = 1000;
    vartotojai.reserve(varInt);
    pkvector.reserve(varInt);
    transakcijos.reserve(trInt);
    for (int i = 0; i < varInt; i++)
    {
        string pk = generuotiPK(pkvector);
        Vartotojas var(pk);
        vartotojai.push_back(var);
    }
    cout << "vartotojai sugeneruoti" << endl;
    for (int i = 0; i < trInt; i++)
    {
        Transakcija tr = generuotiTransakcija(vartotojai);
        transakcijos.push_back(tr);
    }
    cout << "transakcijos sugeneruotos" << endl;
    /*
    cout << "sugeneruotos transakcijos: ";
    for (auto tran : transakcijos)
    {
        cout << tran.getAmount() << " ";
    }
    cout << endl;
    */
    while (!transakcijos.empty())
    {
        Blokas b = formuotiBloka(transakcijos, diff);
        kastiBloka(blockchain, b, transakcijos, diff);
    }
    cout << "blockchainas sudarytas" << endl;
    blockchain.print();
    cout << "galas" << endl;
    return 0;
}

string generuotiPK(vector<string> &pkvec)
{
    string pk;
    char a;
    while (true)
    {
        pk.clear();
        for (int i = 0; i < 64; i++)
        {
            a = (rand() % 94) + 33;
            pk += a;
        }
        bool arKartojasi = false;
        for (const auto &pkv : pkvec)
        {
            if (pkv == pk)
            {
                arKartojasi = true;
                continue;
            }
        }
        if (!arKartojasi)
            break;
    }
    pkvec.push_back(pk);
    return pk;
}

Transakcija generuotiTransakcija(vector<Vartotojas> &var)
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

Blokas formuotiBloka(vector<Transakcija> tran, const string &diff)
{
    vector<Transakcija> tr;
    int n = 100;
    tr.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        int num = rand() % tran.size();
        tr.push_back(move(tran[num]));
        tran[num] = move(tran.back());
        tran.pop_back();
    }
    /*
    cout << "pasirinktos transakcijos: ";
    for (auto t1 : tr)
    {
        cout << t1.getAmount() << " ";
    }
    cout << endl;
    */
    return Blokas(visuTranHash(tr), diff, tr);
}

string visuTranHash(const vector<Transakcija> &tr)
{
    if (tr.empty())
        return string();

    string hash = tr[0].getID();
    int n = (int)tr.size();
    for (int i = 1; i < n; i++)
    {
        hash = stringHash(hash + tr[i].getID());
    }
    return hash;
}

void kastiBloka(Blockchain &b, Blokas a, vector<Transakcija> &tr, string &diff)
{
    int max = 100000;
    if (b.size() != 0)
    {
        string hash = stringHash(b.back().combine());
        a.setPrevHash(hash);
    }
    for (int i = 0; i < max; i++)
    {
        a.changeNonce(i);
        a.setDate();
        string hash = stringHash(a.combine());
        if (hash.rfind(diff, 0) == 0)
        {
            b.pushBack(a);
            cout << hash << endl;
            for (const auto &tran : a.getTran())
            {
                for (int i = 0; i < tr.size(); i++)
                {
                    if (tr[i].getID() == tran.getID())
                    {
                        tr[i] = move(tr.back());
                        tr.pop_back();
                        continue;
                    }
                }
            }
            if (i < max / 2)
            {
                diff += '0';
            }
            return;
        }
    }
    if (diff.size() > 1)
        diff.pop_back();
}