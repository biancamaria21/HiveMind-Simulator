# HiveMind-Simulator
## Descriere

HiveMind este un simulator de livrare de pachete cu mai mulți agenți autonomi pe o hartă de tip grilă. Scopul algoritmului este să livreze pachetele clienților astfel încât profitul net să fie pozitiv.

Pe hartă există:

- **B**: baza, de unde pornesc agenții
- **S**: stații de încărcare
- **D**: clienți
- **#**: obstacole

Simulatorul include trei tipuri de agenți: **drone**, **roboți** și **scutere**. Fiecare are baterie, viteză, consum, cost și capacitate, iar unele pot traversa anumite tipuri de teren.

## Algoritmul HiveMind

1. Se selectează pachetele care nu au fost încă livrate sau alocate.
2. Pentru fiecare pachet, se caută un agent în stare IDLE, cu baterie peste 40%.
3. Pentru agentul disponibil se calculează ruta optimă cu **BFS** (Breadth-First Search).
4. Se verifică dacă livrarea este profitabilă. Dacă toate condițiile sunt îndeplinite, pachetul este alocat și se trece la următorul.

## Generarea hărții

Harta poate fi generată în două moduri:

- **FileMapLoader**: citește harta dintr-un fișier text (`map.txt`), util pentru testare și depanare.
- **ProceduralMapGenerator**: generează harta aleator și reîncearcă până când există un drum între bază, stații și clienți.

## Structura proiectului

- `main.cpp`: codul sursă al simulatorului
- `map.txt`: harta de intrare
- `simulation_setup.txt`: parametrii simulării (dimensiunea hărții, numărul de agenți, numărul de pachete etc.)
- `docs/Documentatie.pdf`: descrierea algoritmilor
- `docs/Diagrama.pdf`: diagrama UML a claselor

## Rulare

Compilare (din folderul proiectului):

```bash
g++ main.cpp -o main
```

Rulare:

```bash
./main        # Linux / macOS
main.exe      # Windows
```

Fișierele `map.txt` și `simulation_setup.txt` trebuie să fie în același folder cu executabilul.

## Exemplu de rezultat

Pentru configurația din `simulation_setup.txt`, o rulare a produs:

```
Profit final: 14334
Pachete livrate: 50
Pachete nelivrate: 0
Agenti morti: 1
```
