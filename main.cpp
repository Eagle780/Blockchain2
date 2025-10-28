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
    int varInt = 1000;
    int trInt = 10000;
    vartotojai.reserve(varInt);
    pkvector.reserve(varInt);
    transakcijos.reserve(trInt);
    for (int i = 0; i < varInt; i++)
    {
        string pk = generuotiPK(pkvector);
        Vartotojas var(pk);
        vartotojai.push_back(var);
        if (i <= 10)
        {
            cout << var.getPK() << " " << var.getBal() << endl;
        }
    }
    cout << "vartotojai sugeneruoti" << endl;
    for (int i = 0; i < trInt; i++)
    {
        Transakcija tr = generuotiTransakcija(vartotojai);
        transakcijos.push_back(tr);
        if (i > 9900)
        {
            cout << "Sender: " << tr.getSender() << endl;
            cout << "Receiver: " << tr.getReceiver() << endl;
            cout << "Amount: " << tr.getAmount() << endl;
            cout << "ID: " << tr.getID() << endl;
        }
    }
    cout << "transakcijos sugeneruotos" << endl;
    while (!transakcijos.empty())
    {
        Blokas b = formuotiBloka(transakcijos, diff);
        kastiBloka(blockchain, b, transakcijos, diff, vartotojai);
    }
    cout << "blockchainas sudarytas" << endl;
    blockchain.print();
    for (int i = 0; i < 10; i++)
    {
        cout << vartotojai[i].getPK() << " " << vartotojai[i].getBal() << endl;
    }
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
    float a = 0;
    while (var[s].getPK() == var[r].getPK())
    {
        r = rand() % var.size();
    }
    while (a == 0)
    {
        a = rand() % int(var[s].getBal());
    }
    /*
    if (var[s].getBal() > 1)
    {
        while (a == 0)
        {
            a = rand() % int(var[s].getBal()) / 2.0;
        }
    }
    else
    {
        a = 2.0;
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);
        while (a >= var[s].getBal())
        {
            a = dis(gen);
        }
    }
    int bal1 = var[s].getBal() - a;
    var[s].updateBal(bal1);
    int bal2 = var[r].getBal() + a;
    var[r].updateBal(bal2);
    */
    Transakcija tr(var[s].getPK(), var[r].getPK(), a);

    return tr;
}

Blokas formuotiBloka(vector<Transakcija> &tran, const string &diff)
{
    vector<Transakcija> tr;
    int n = 100;
    tr.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        int num = rand() % tran.size();
        tr.push_back(tran[num]);
        if (tran.empty())
            break;
    }
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

void kastiBloka(Blockchain &b, Blokas a, vector<Transakcija> &tr, string &diff, vector<Vartotojas> &var)
{
    int max = 100000;
    float bal;
    if (b.size() != 0)
    {
        string hash = stringHash(b.back().combine());
        a.setPrevHash(hash);
    }
    a.setDiff(diff);
    for (int i = 0; i < max; i++)
    {
        a.changeNonce(i);
        a.setDate();
        string hash = stringHash(a.combine());
        if (hash.rfind(diff, 0) == 0)
        {
            b.pushBack(a);
            if (i < max / 2)
            {
                diff += '0';
            }
            for (auto const &tran : a.getTran())
            {
                for (int i = 0; i < tr.size(); i++)
                {
                    if (tran.getID() == tr[i].getID())
                    {
                        tr[i] = move(tr.back());
                        tr.pop_back();
                    }
                }
                for (auto &v : var)
                {
                    if (tran.getSender() == v.getPK())
                    {
                        bal = v.getBal() - tran.getAmount();
                        v.updateBal(bal);
                    }
                    else if (tran.getReceiver() == v.getPK())
                    {
                        bal = v.getBal() + tran.getAmount();
                        v.updateBal(bal);
                    }
                }
            }
            return;
        }
    }
    if (diff.size() > 1)
        diff.pop_back();
}