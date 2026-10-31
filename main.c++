#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <memory>
#include <string>
#include <cstdlib>
#include <ctime>

using namespace std;

enum StareAgent
{
    IDLE,
    MOVING,
    CHARGING,
    DEAD
};

struct Pozitie
{
    int x, y;
};

class Harta
{
public:
    int randuri, coloane;
    vector<vector<char>> grila;

    Harta(int r, int c)
        : randuri(r), coloane(c),
          grila(r, vector<char>(c, '.')) {}

    bool estePeHarta(int x, int y)
    {
        return x >= 0 && y >= 0 && x < randuri && y < coloane;
    }
};

// ABSTRACTIZARE - IMapGenerator este o clasa pur virtuala
class IMapGenerator
{
public:
    virtual Harta generate() = 0;
    virtual ~IMapGenerator() {}
};

// MOSTENIRE- FileMapLoader mosteneste interfata IMapGenerator.
class FileMapLoader : public IMapGenerator
{
    string numeFisier;

public:
    FileMapLoader(const string &fisier) : numeFisier(fisier) {}

    // POLIMORFISM - suprascriem metoda generate
    Harta generate() override
    {
        ifstream in(numeFisier);
        if (!in)
            throw runtime_error("Nu pot deschide fisierul hartii");

        vector<string> linii;
        string linie;

        while (getline(in, linie))
        {
            if (!linie.empty())
                linii.push_back(linie);
        }

        int r = linii.size();
        int c = linii[0].size();

        Harta hartaNoua(r, c);

        for (int i = 0; i < r; i++)
            for (int j = 0; j < c; j++)
                hartaNoua.grila[i][j] = linii[i][j];

        return hartaNoua;
    }
};

// MOSTENIRE- ProceduralMapGenerator mosteneste interfata IMapGenerator
class ProceduralMapGenerator : public IMapGenerator
{
    int randuri, coloane, statii, clienti;

public:
    ProceduralMapGenerator(int r, int c, int s, int cl)
        : randuri(r), coloane(c), statii(s), clienti(cl) {}

    // POLIMORFISM
    Harta generate() override
    {
        while (true)
        {
            Harta harta(randuri, coloane);
            harta.grila[0][0] = 'B';

            for (int i = 0; i < randuri * coloane / 5; i++)
            {
                int x = rand() % randuri;
                int y = rand() % coloane;
                if (harta.grila[x][y] == '.')
                    harta.grila[x][y] = '#';
            }

            for (int i = 0; i < statii; i++)
            {
                int x, y;
                do
                {
                    x = rand() % randuri;
                    y = rand() % coloane;
                } while (harta.grila[x][y] != '.');
                harta.grila[x][y] = 'S';
            }

            for (int i = 0; i < clienti; i++)
            {
                int x, y;
                do
                {
                    x = rand() % randuri;
                    y = rand() % coloane;
                } while (harta.grila[x][y] != '.');
                harta.grila[x][y] = 'D';
            }

            if (valideaza(harta))
                return harta;
        }
    }

    bool valideaza(Harta &h)
    {
        vector<vector<bool>> vizitat(h.randuri, vector<bool>(h.coloane, false));
        queue<Pozitie> coada;

        int numar = 0;
        for (int i = 0; i < h.randuri; i++)
            for (int j = 0; j < h.coloane; j++)
                if (h.grila[i][j] == 'S' || h.grila[i][j] == 'D')
                    numar++;

        coada.push({0, 0});
        vizitat[0][0] = true;

        int dx[4] = {1, -1, 0, 0};
        int dy[4] = {0, 0, 1, -1};

        while (!coada.empty())
        {
            Pozitie p = coada.front();
            coada.pop();

            if (h.grila[p.x][p.y] == 'S' || h.grila[p.x][p.y] == 'D')
                numar--;

            if (numar == 0)
                return true;

            for (int i = 0; i < 4; i++)
            {
                int nx = p.x + dx[i];
                int ny = p.y + dy[i];

                if (h.estePeHarta(nx, ny) &&
                    !vizitat[nx][ny] &&
                    h.grila[nx][ny] != '#')
                {
                    vizitat[nx][ny] = true;
                    coada.push({nx, ny});
                }
            }
        }

        return false;
    }
};

// INCAPSULARE - membrii protected
class Agent
{
protected:
    Pozitie pozitie;
    int baterie, maxBat, viteza, consum, cost, capacitate;
    StareAgent stare;
    bool penalizareAplicata = false;
    vector<Pozitie> traseuCurent;

public:
    Agent(int b, int v, int c, int cm, int cap)
        : baterie(b), maxBat(b), viteza(v), consum(c), cost(cm), capacitate(cap), stare(IDLE)
    {
        pozitie = {0, 0};
    }

    // POLIMORFISM - metodele simbol si poateTrece, sunt implementate diferit de Drona, Robot, Scuter
    virtual char simbol() = 0;
    virtual bool poateTrece(char tipCelula) = 0;

    // INCAPSULARE - getteri
    int getBaterie() const { return baterie; }
    int getViteza() const { return viteza; }
    int getConsum() const { return consum; }
    int getCost() const { return cost; }
    StareAgent getStare() const { return stare; }
    Pozitie getPozitie() const { return pozitie; }

    bool penalizat() const
    {
        return penalizareAplicata;
    }

    void marcheazaPenalizat()
    {
        penalizareAplicata = true;
    }

    bool esteDisponibil() const
    {
        return stare == IDLE && baterie > 40;
    }

    void traseu(const vector<Pozitie> &drum)
    {
        traseuCurent = drum;
        if (!traseuCurent.empty())
        {
            stare = MOVING;
        }
    }

    void pas()
    {
        if (stare != MOVING || traseuCurent.empty())
        {
            stare = IDLE;
            return;
        }

        for (int i = 0; i < viteza && !traseuCurent.empty(); i++)
        {
            pozitie = traseuCurent.front();
            traseuCurent.erase(traseuCurent.begin());
        }

        if (traseuCurent.empty())
            stare = IDLE;
    }

    virtual void tick(Harta &h)
    {
        char tipCelCurenta = h.grila[pozitie.x][pozitie.y];

        if (stare == DEAD)
            return;

        if (tipCelCurenta == 'B' || tipCelCurenta == 'S')
        {
            if (baterie < maxBat)
            {
                baterie += maxBat / 4;
                if (baterie > maxBat)
                    baterie = maxBat;
                if (stare != MOVING)
                    stare = CHARGING;
            }
            else if (stare == CHARGING)
                stare = IDLE;
        }

        if (stare == MOVING)
        {
            pas();
            baterie -= consum;
        }

        if (baterie <= 0 && tipCelCurenta != 'S' && tipCelCurenta != 'B')
        {
            baterie = 0;
            stare = DEAD;
        }
    }
};

// MOSTENIRE - Drone, Robot si Scooter mostenesc clasa Agent
class Drona : public Agent
{
public:
    Drona() : Agent(100, 3, 10, 15, 1) {}
    char simbol() override { return '^'; }
    bool poateTrece(char tipCel) override { return true; } // Drona zboara peste ziduri
};

class Robot : public Agent
{
public:
    Robot() : Agent(300, 1, 2, 1, 4) {}
    char simbol() override { return 'R'; }
    bool poateTrece(char tipCel) override { return tipCel != '#'; }
};

class Scuter : public Agent
{
public:
    Scuter() : Agent(200, 2, 5, 4, 2) {}
    char simbol() override { return 'S'; }
    bool poateTrece(char tipCel) override { return tipCel != '#'; }
};

struct Pachet
{
    int recompensa, termenLimita, tickAparitie;
    Pozitie destinatie;
    bool alocat = false, livrat = false;
};

class Simulator
{
    Harta harta;
    vector<unique_ptr<Agent>> agenti;
    vector<Pachet> pachete;
    vector<Pozitie> locatiiClienti;
    int profit = 0, tickCurent = 0;

public:
    Simulator(Harta h) : harta(h) {}

    void adaugaAgent(unique_ptr<Agent> a)
    {
        agenti.push_back(move(a));
    }

    void gasesteClienti()
    {
        for (int i = 0; i < harta.randuri; i++)
            for (int j = 0; j < harta.coloane; j++)
                if (harta.grila[i][j] == 'D')
                    locatiiClienti.push_back({i, j});
    }

    void genereazaPachet()
    {
        if (locatiiClienti.empty())
            gasesteClienti();
        Pachet p;
        p.recompensa = 200 + rand() % 601;
        p.termenLimita = 10 + rand() % 11;
        p.tickAparitie = tickCurent;
        p.destinatie = locatiiClienti[rand() % locatiiClienti.size()];
        pachete.push_back(p);
    }

    vector<Pozitie> calculeazaRuta(Pozitie start, Pozitie scop, bool esteAerian)
    {
        queue<pair<Pozitie, vector<Pozitie>>> coada;
        coada.push({start, {}});
        vector<vector<bool>> vizitat(harta.randuri, vector<bool>(harta.coloane, false));
        vizitat[start.x][start.y] = true;

        int dx[] = {0, 0, 1, -1}, dy[] = {1, -1, 0, 0};

        while (!coada.empty())
        {
            pair<Pozitie, vector<Pozitie>> elementCurent = coada.front();
            coada.pop();
            Pozitie curr = elementCurent.first;
            vector<Pozitie> cale = elementCurent.second;

            if (curr.x == scop.x && curr.y == scop.y)
                return cale;

            for (int i = 0; i < 4; i++)
            {
                Pozitie urm = {curr.x + dx[i], curr.y + dy[i]};
                if (harta.estePeHarta(urm.x, urm.y) && !vizitat[urm.x][urm.y])
                {
                    if (esteAerian || harta.grila[urm.x][urm.y] != '#')
                    {
                        vizitat[urm.x][urm.y] = true;
                        vector<Pozitie> caleNoua = cale;
                        caleNoua.push_back(urm);
                        coada.push({urm, caleNoua});
                    }
                }
            }
        }
        return {};
    }

    void hiveMind()
    {
        for (size_t i = 0; i < pachete.size(); i++)
        {
            Pachet &p = pachete[i];

            if (p.livrat || p.alocat)
                continue;

            for (size_t j = 0; j < agenti.size(); j++)
            {
                unique_ptr<Agent> &a = agenti[j];

                if (a->esteDisponibil())
                {
                    vector<Pozitie> ruta =
                        calculeazaRuta(a->getPozitie(), p.destinatie, a->simbol() == '^');
                    if (!ruta.empty())
                    {
                        int costRuta = (int)ruta.size() * a->getConsum();

                        if (a->getBaterie() >= costRuta &&
                            p.recompensa > costRuta * a->getCost())
                        {
                            a->traseu(ruta);
                            p.alocat = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    void tick()
    {
        hiveMind();

        for (size_t i = 0; i < agenti.size(); i++)
        {
            unique_ptr<Agent> &a = agenti[i];

            if (a->getStare() == DEAD)
                continue;

            if (a->getStare() == IDLE && a->getBaterie() < 30)
            {
                vector<Pozitie> rutaIncarcare =
                    calculeazaRuta(a->getPozitie(), Pozitie{0, 0}, a->simbol() == '^');
                a->traseu(rutaIncarcare);
            }

            a->tick(harta);
            profit -= a->getCost() * (a->getStare() == MOVING);

            if (a->getStare() == DEAD && !a->penalizat())
            {
                profit -= 500;
                a->marcheazaPenalizat();
            }

            for (size_t j = 0; j < pachete.size(); j++)
            {
                Pachet &p = pachete[j];

                if (p.alocat && !p.livrat &&
                    a->getPozitie().x == p.destinatie.x &&
                    a->getPozitie().y == p.destinatie.y)
                {
                    p.livrat = true;
                    profit += p.recompensa;

                    if (tickCurent - p.tickAparitie > p.termenLimita)
                        profit -= 50;
                }
            }
        }

        tickCurent++;
    }

    void genereazaRaport()
    {
        int pachetePierdute = 0;

        for (size_t i = 0; i < pachete.size(); i++)
        {
            if (pachete[i].alocat && !pachete[i].livrat)
            {
                profit -= 200;
                pachetePierdute++;
            }
        }

        int agentiMorti = 0;

        for (size_t i = 0; i < agenti.size(); i++)
        {
            if (agenti[i]->getStare() == DEAD)
                agentiMorti++;
        }

        ofstream out("simulation.txt");
        out << "Profit final: " << profit << " \n";
        out << "Pachete livrate: " << pachete.size() - pachetePierdute << "\n";
        out << "Pachete nelivrate: " << pachetePierdute << "\n";
        out << "Agenti morti: " << agentiMorti << "\n";
        out.close();
    }
};

struct Configuratie
{
    int randuri, coloane, maxTicks, statii, clienti, drone, roboti, scutere, totalPachete, frecventa;
};

Configuratie citesteConfig(const string &numeFisier)
{
    ifstream in(numeFisier);
    if (!in)
        throw runtime_error("Nu se deschide simulation_setup.txt");

    Configuratie c;
    string cheie;
    while (in >> cheie)
    {
        if (cheie == "MAP_SIZE:")
            in >> c.randuri >> c.coloane;
        else if (cheie == "MAX_TICKS:")
            in >> c.maxTicks;
        else if (cheie == "MAX_STATIONS:")
            in >> c.statii;
        else if (cheie == "CLIENTS_COUNT:")
            in >> c.clienti;
        else if (cheie == "DRONES:")
            in >> c.drone;
        else if (cheie == "ROBOTS:")
            in >> c.roboti;
        else if (cheie == "SCOOTERS:")
            in >> c.scutere;
        else if (cheie == "TOTAL_PACKAGES:")
            in >> c.totalPachete;
        else if (cheie == "SPAWN_FREQUENCY:")
            in >> c.frecventa;
    }
    return c;
}

int main()
{
    srand(time(nullptr));

    // exceptie
    try
    {
        Configuratie cfg = citesteConfig("simulation_setup.txt");

        unique_ptr<IMapGenerator> generatorHarta = make_unique<ProceduralMapGenerator>(cfg.randuri, cfg.coloane, cfg.statii, cfg.clienti);

        Harta hartaSimulare = generatorHarta->generate();
        Simulator sim(hartaSimulare);

        for (int i = 0; i < cfg.drone; i++)
            sim.adaugaAgent(make_unique<Drona>());
        for (int i = 0; i < cfg.roboti; i++)
            sim.adaugaAgent(make_unique<Robot>());
        for (int i = 0; i < cfg.scutere; i++)
            sim.adaugaAgent(make_unique<Scuter>());

        int pacheteCreate = 0;
        for (int t = 0; t < cfg.maxTicks; t++)
        {
            if (t % cfg.frecventa == 0 && pacheteCreate < cfg.totalPachete)
            {
                sim.genereazaPachet();
                pacheteCreate++;
            }
            sim.tick();
        }

        sim.genereazaRaport();
    }
    catch (const exception &e)
    {
        cerr << ">> Eroare " << e.what() << endl;
        return 1;
    }
    return 0;
}
