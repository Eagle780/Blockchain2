# Supaprastinta blokų grandinė

## Aprašymas

Ši programa sukuria supaprastintą blokų grandinę.
Tam buvo sukurtos vartotojo, transakcijos, bloko ir pačios blokų grandinės klasės.

1. Vartotojų generavimas
   Programa sukuria 1,000 vartotojų, kiekvienam iš jų priskirdama vardą, viešąjį raktą (kuris sudarytas iš 64 atsitiktinių ASCII simbolių) ir atsitiktinį balansą nuo 100 iki 1,000,000.
   Kiekvieno vartotojo viešasis raktas išsaugomas atskirame raktų vektoriuje, kuris yra peržiūrima skiekvieną kartą kuriant naują viešąjį raktą, taip išvengiant pasikartojimų.
2. Transakcijų generavimas
   Programa sugeneruoja 10,000 transakcijų paimdama atsitiktinį siuntėją, gavėją (prieš tai patikrinant, ar jie nesutampa) ir atsitiktinį skaičių tarp 1 ir siuntėjo balanso. Kiekviena transakcija tusi savo ID kuris yra siuntėjo,
   gavėjo ir siunčiamos sumos hash'as.
4. Naujo bloko formavimas
   Programa pasirenka 100 atsitiktinių transakcijų iš sąrašo ir sukuria naują bloką su transakcijomis, jų bendru hash'u ir sunkumo lygiu (nulių kiekį, kurį reikia gauti bloko hash'o pradžioje kasimo metu)
5. Bloko kasimas
   Keičiant nonce (skaičių, kuris vis keičiasi bloko antraštėje) hash'uojama viso bloko antraštė iki tol, kol gaunamas hash'as atitinkantis sunkumą arba pasiekiamas nonce limitas (100,000).
   Jei blokas iškasamas (gaunamas tinkamas hash'as), bloke esančios transakcijos pašalinamos iš bendro transakcijų vektoriaus, atnaujinami vartotojų balansai, atnaujinamas sunkumas (jei reikia) ir blokas pridedamas prie blokų grandinės.
   Jei blokas neiškasamas, sunkumas pamažinamas ir kuriamas naujas blokas su 100 atsitiktinių transakcijų.

3-4 žingsniai kartojami tol, kol nebelieka neįtrauktų transakcijų.

## Naudojimas

1. Klonuokite repozitoriją:

    git clone https://github.com/Eagle780/Blockshain2.git

2. Repositoriją atidarykite savo pasirinktame kodo redaktoriuje (pvz. VS Code) ir atidarykite terminalą.
3. Įrašykite:

    g++ blockchain main.cpp hash.cpp
   ./blockchain

Visus sekančius žingsnius programa atliks automatiškai.

## Nuotraukos

<img width="983" height="629" alt="Blockchaino spausdinamos informacijos pavyzdys" src="https://github.com/user-attachments/assets/2c742b8b-63f2-42ba-ab8e-7ce30e3311e6" />

<img width="914" height="185" alt="Transakcijos informacijso pavyzdys" src="https://github.com/user-attachments/assets/2e8d8ee0-5bcc-4319-99b0-994c1389fbb6" />

