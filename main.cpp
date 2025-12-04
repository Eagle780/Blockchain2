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

    // Initialize UTXO pool with starting balances for each user
    initializeUTXOPool(vartotojai);
    cout << "UTXO pool'as sukurtas" << endl;
    for (int i = 0; i < trInt; i++)
    {
        Transakcija tr = generuotiTransakcija(vartotojai);
        transakcijos.push_back(tr);
        if (i > 9900)
        {
            cout << "Transaction " << i << " created" << endl;
            cout << "Inputs: " << tr.getInputs().size() << endl;
            cout << "Outputs: " << tr.getOutputs().size() << endl;
            cout << "ID: " << tr.getTxHash() << endl;
        }
    }
    cout << "transakcijos sugeneruotos" << endl;

    int miningRound = 1;
    int baseTimeLimit = 5;

    while (!transakcijos.empty() && miningRound <= 10)
    {
        cout << "\n--- Mining Round " << miningRound << " ---" << endl;
        cout << "Likusios transakcijos: " << transakcijos.size() << endl;
        cout << "Sudetingumas: " << diff << endl;

         auto candidateBlocks = generateCandidateBlocks(transakcijos, diff, 5, vartotojai, blockchain);
        cout << "Sugeneruoti " << candidateBlocks.size() << " kandidatiniai blokai" << endl;
    
        if (candidateBlocks.empty()) {
        cout << "Kandidatiniai blokai nesugeneruoti. Kasimas nutraukiamas" << endl;
        break;}

        parallelMineBlocks(blockchain, candidateBlocks, transakcijos, diff, vartotojai, baseTimeLimit);

        miningRound++;

        if (miningRound % 3 == 0 && diff.size() > 2)
        {
            if (blockchain.size() == 0 || blockchain.back().getHash().find(diff) != 0)
            {

                if (diff.size() > 1)
                {
                    diff.pop_back();
                    cout << "Sumazintas sudetingumas iki: " << diff << endl;
                }
            }
            else
            {
                if (diff.size() < 5)
                {
                    diff += '0';
                    cout << "Padidintas sudetingumas iki: " << diff << endl;
                }
            }
        }

        /* Blokas b = formuotiBloka(transakcijos, diff);
         kastiBloka(blockchain, b, transakcijos, diff, vartotojai);*/
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
    int senderIdx = rand() % var.size();
    int receiverIdx = rand() % var.size();

    while (var[senderIdx].getPK() == var[receiverIdx].getPK())
    {
        receiverIdx = rand() % var.size();
    }

    string senderPK = var[senderIdx].getPK();
    string receiverPK = var[receiverIdx].getPK();

    // Find unspent UTXOs for the sender
    vector<UTXO> senderUTXOs;
    for (const auto &utxo : utxoPool)
    {
        if (utxo.owner == senderPK && !utxo.spent)
        {
            senderUTXOs.push_back(utxo);
        }
    }

    if (senderUTXOs.empty())
    {
        // Return a transaction with no inputs (will be invalid)
        Transakcija tr(stringHash("empty_" + senderPK));
        return tr;
    }

    // Select UTXOs to spend (for simplicity, just pick one)
    vector<TxIn> inputs;
    int totalInput = 0;

    // Use a random UTXO as input
    int utxoIdx = rand() % senderUTXOs.size();
    UTXO selectedUtxo = senderUTXOs[utxoIdx];
    inputs.push_back(TxIn(selectedUtxo.txId, selectedUtxo.outputIndex));
    totalInput = selectedUtxo.amount;

    // Generate transaction amount (less than available input)
    int amount = (totalInput > 1) ? (rand() % (totalInput - 1)) + 1 : 1;

    // Create outputs: one to receiver, one as change to sender (if any)
    vector<TxOut> outputs;
    outputs.push_back(TxOut(receiverPK, amount));

    int change = totalInput - amount;
    if (change > 0)
    {
        outputs.push_back(TxOut(senderPK, change));
    }

    // Create transaction ID
    string txHash = stringHash(senderPK + receiverPK + to_string(amount));
    Transakcija tr(txHash, inputs, outputs);

    return tr;
}

Blokas formuotiBloka(vector<Transakcija> &tran, const string &diff)
{
    vector<Transakcija> tr;
    int n = 100;
    tr.reserve(n);
    for (int i = 0; i < n && !tran.empty(); ++i)
    {
        int num = rand() % tran.size();
        tr.push_back(tran[num]);
        tran[num] = move(tran.back());
        tran.pop_back();
        /*if (tran.empty())
            break;*/
    }
    string merkleRoot = calculateMerkleRoot(tr);
    return Blokas(merkleRoot, diff, tr);
}

string visuTranHash(const vector<Transakcija> &tr)
{
    if (tr.empty())
        return string();

    string hash = tr[0].getTxHash();
    int n = (int)tr.size();
    for (int i = 1; i < n; i++)
    {
        hash = stringHash(hash + tr[i].getTxHash());
    }
    return hash;
}

void kastiBloka(Blockchain &b, Blokas a, vector<Transakcija> &tr, string &diff, vector<Vartotojas> &var)
{
    int max = 100000;
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
                // Process UTXO changes
                spendUTXOs(tran.getInputs());
                addNewUTXOs(tran.getTxHash(), tran.getOutputs());

                // Remove from pending transactions
                for (int j = 0; j < tr.size(); j++)
                {
                    if (tran.getTxHash() == tr[j].getTxHash())
                    {
                        tr[j] = move(tr.back());
                        tr.pop_back();
                    }
                }
            }
            return;
        }
    }
    if (diff.size() > 1)
        diff.pop_back();
}