# Supaprastinta Blokų Grandinė (Blockchain) – Versija v0.1

## Projekto aprašymas

Ši programa sukurta kaip supaprastinta blokų grandinės (blockchain) versija, skirta pademonstruoti pagrindinius šios technologijos veikimo principus naudojant **C++** programavimo kalbą.

Projekte naudojamos pagrindinės objektinio programavimo (OOP) sąvokos – **klasės, konstruktoriai, destruktoriai, duomenų inkapsuliacija** ir **duomenų saugojimas į failus**.  
Sistema imituoja paprastą kriptovaliutos tinklą su vartotojais, jų balansais, transakcijomis ir kasamais blokais.

Programoje įgyvendinti pagrindiniai **blokų grandinės** principai:

- kiekvienas blokas turi savo „hash“ reikšmę, priklausančią nuo ankstesnio bloko;
- transakcijos saugomos blokų grandinėje po jų patvirtinimo;
- duomenys saugomi lokaliuose `.txt` failuose: `users.txt` ir `transactions.txt`;
- kasimo (mining) procesas patvirtina blokus pagal nustatytą sunkumo lygį (difficulty);
- programa tikrina transakcijų galiojimą (pvz., ar vartotojas turi pakankamai lėšų).

Programos tikslas – parodyti, kaip veikia **Proof-of-Work algoritmas**, **transakcijų tikrinimas**, ir kaip iš jų formuojama vientisa, validi blokų grandinė.

---

## Pagrindinės funkcijos

### Vartotojų generavimas

- Sugeneruojami 1000 vartotojų su unikaliais vardais ir pradiniu balansu.
- Kiekvienas vartotojas gali atlikti pervedimus.
- Balansas saugomas `users.txt`.

### Transakcijų kūrimas ir validacija

- Sugeneruojamos 10 000–20 000 transakcijų.
- Tikrinama:
- Ar siuntėjas egzistuoja
- Ar turi pakankamai lėšų
- Ar transakcijos ID atitinka duomenis
- Galiojančios transakcijos įrašomos į `transactions.txt`.

---

### Blokų kasimas (Proof-of-Work)

- Visi patvirtinti pervedimai sugrupuojami į **blokus po 100 transakcijų**.
- Kiekvienam blokui sukuriama struktūra su ankstesnio bloko hash reikšmė, supaprastinta Merkle šaknies hash, laiko žyma, atsitiktiniu skaičiumi, naudojamu kasimo algoritme, reikalaujamu maišos sudėtingumu, blokų versijos numeriu.
- Blokas laikomas „iškastu“, kai jo hash prasideda trimis nuliais (`000`) (jei sudėtingumas = 3).
- Kiekvieno kasimo metu į konsolę išvedama: kasimo trukmė (sekundėmis), bloko numeris, transakcijų kiekis, hash reikšmė, nonce reikšmė.

---

### Duomenų išsaugojimas

Visi duomenys išsaugomi kataloge **`data/`**:

| Failas             | Turinys                      | Pavyzdys                              |
|--------------------|------------------------------|---------------------------------------|
| `users.txt`        | Visi vartotojai su balansais | `as,10`                               |
| `transactions.txt` | Patvirtintos transakcijos    | `as,jis,10`                           |
| `blocks.txt`       | Visi iškasti blokai          | `Block #1, (headeris + transakcijos)` |

---

### Blokų grandinės validacija

Po kiekvieno kasimo paleidžiama funkcija `isChainValid()`, kuri tikrina:

- Ar kiekvieno bloko prevHash atitinka ankstesnio bloko hash.
- Ar grandinė nesugadinta (pakeistų blokų nėra)
- Konsolėje rodoma validumo būsena.

---

## Naudojimosi instrukcija

### Programos paleidimas

- Paleiskite programą (`main.exe` arba per IDE).
- Konsolėje bus pasiūlytas pasirinkimas:

``` txt
0 – atsitiktinai generuoti 1000 vartotoju ir apie 10 000 transakciju
1 – perskaityti vartotojus ir transakcijas is failu
```

---

### Režimas 0 – automatinis duomenų generavimas

Jei pasirinksite **0**, programa automatiškai:

- Sugeneruos 1000 vartotojų su vardu ir pradiniu balansu.
- Sugeneruos 10 000 – 20 000 atsitiktinių transakcijų tarp vartotojų.
- Supakuos transakcijas į blokus po 100 pervedimų.
- Iškas kiekvieną bloką naudodama Proof-of-Work algoritmą (hash prasideda trimis nuliais, jei sudėtingumas = 3).

**Konsolėje bus rodoma:**

- Techinė informacija apie naudotojus, transakcijas **prieš ir po grandinės sukurimo**.
- Techinė informacija apie kiekvieną iškasta bloką ir apie visus.

Baigus darbą, visi rezultatai (kiti duomenys) bus išsaugoti kataloge `data/`:

- `users.txt` – visi vartotojai su pakeistais balansais
- `transactions.txt` – tiktai patvirtintos transakcijos
- `blocks.txt` – visi iškasti blokai

### Režimas 1 – naudojimas su paruoštais failais

Jei pasirinksite **1**, prieš paleidimą reikia **paruošti du failus kataloge `data/`**:

- **users.txt** – vartotojų sąrašas
Kiekviena eilutė turi turėti tokią struktūrą:
vardas,balansas

``` txt
as,10
jis,20
tomas,50
ieva,35
```

- **transactions.txt** – transakcijų sąrašas  
Kiekviena eilutė turi turėti tokią struktūrą:
siuntejas,gavejas,suma

``` txt
as,jis,10
tomas,ieva,25
jis,ieva,5
```

Paleidus programą, sistema:

- Patikrins, ar visi vartotojai ir transakcijos galioja
- Suskaičiuos balansus
- Sugrupuos transakcijas į blokus po 100
- Iškasys kiekvieną bloką (Proof-of-Work)

Konsolėje bus rodoma ir failuose talpinama tokio pat tipo infomacija kaip ir režimo 0 atveju

---

## 🏁 Išvada

Šis projektas parodo, kaip net ir be išorinės kriptografijos bibliotekos galima įgyvendinti supaprastintą blokų grandinę su:

- aiškia vartotojų sistema,
- realiais balanso patikrinimais,
- Proof-of-Work algoritmu,
- automatiniu duomenų išsaugojimu ir validacija.

## AI pagalba ir autorius

**AI pagalba:**
Visas logikos ir funkcionalumo kūrimas atliktas savarankiškai.
ChatGPT padėjo optimizuoti kodą (buvo problemos su duomenų skaitymų iš failų), padėjo gražiai struktūruoti kodą darbo pabaigoje, atsikratyti nereikiamų funkcijų. Padėjo su testavimo atvėjų parinkimu ir su README dokumentacijos formatavimu.  

**Autorius:** Julius Vilkanec  
**Studijų programa:** Informacinių sistemų inžinerija, Vilniaus Universitetas  
**Kursas:** 2 kursas  
**Versija:** v0.1  
**Data:** 2025-10-28
