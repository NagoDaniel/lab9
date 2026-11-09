# Passhálózatok elemzése — FC Barcelona (2009/10)

## Bevezetés

Buldú és társai (2019) a hálózattudomány eszközeivel vizsgálták Guardiola
Barcelonáját. Minden meccsen minden csapatot egy súlyozott, irányított gráffal
modelleztek: a csúcsok a 11 kezdő játékos, az *u → v* irányított él akkor létezik,
ha *u* legalább egyszer sikeresen passzolt *v*-nek, az él súlya pedig a passzok
száma. A feladat ezeknek a gráfoknak a felépítése és a rájuk vonatkozó mérőszámok
kiszámítása, majd a Barcelona (35 gráf) és az ellenfelek (35 gráf)
összehasonlítása.

## Bemenet

 `passes_clean2.csv`, soronként egy sikeres passz.

| oszlop | típus | jelentés |
|--------|-------|----------|
| `team` | szöveg | passzoló csapat |
| `player_1`, `player_2` | szöveg | passzoló, illetve passzt kapó játékos |
| `x1`, `y1` | double | a passz kiinduló koordinátái, `[0,100]` |
| `x2`, `y2` | double | a passz cél koordinátái, `[0,100]` |
| `match_id` | int | meccs azonosító |
| `elapsed_seconds` | double | a passz ideje a meccs kezdetétől (mp) |

Az x a támadás iránya (pálya hossza), az y a szélesség. Egy gráfot a
`(match_id, team)` pár azonosít; a `team == "Barcelona"` jelöli a Barcelona
gráfjait.

## A gráf

Minden `(match_id, team)` csoportra rendeljünk minden játékoshoz egy `0…N-1`
indexet (`N = 11`), és építsünk egy `N×N`-es súlymátrixot: `W[i][j]` az *i*-ből
*j*-be menő passzok száma. A gráf irányított, `W[i][j]` és `W[j][i]` független.

## Javasolt struktúrák

```cpp
// (match_id, team) egyértelműen azonosít egy gráfot
struct GraphKey {
    int match_id;
    std::string team;
    bool operator<(const GraphKey& o) const {
        if (match_id != o.match_id) return match_id < o.match_id;
        return team < o.team;
    }
};

// a passes_clean2.csv egy sora
struct PassRow {
    std::string team;
    std::string player1, player2;
    double x1, y1, x2, y2;
    int match_id;
    double elapsed_seconds;
};

// súlyozott, irányított gráf: N×N szomszédsági mátrix, W[i][j] = passzok i -> j
struct Graph {
    std::vector<std::string>         players;       // index -> név
    std::map<std::string, int>       player_index;  // név -> index
    std::vector<std::vector<double>> W;
    int  N() const { return players.size(); }
};

```

## Kiszámítandó mérőszámok

Minden mérőszámot mind a 70 gráfra ki kell számítani.

**Térbeli mérőszámok**

| mérőszám | definíció |
|----------|-----------|
| `total_passes` | az összes élsúly összege |
| `centroid_x`, `centroid_y` | az `x1`, illetve `y1` átlaga minden passzon |
| `centroid_dispersion` | a játékosok átlagos pozíciójának a centroidtól vett távolságainak populációs szórása |
| `advance_ratio` | `Σ\|y2−y1\| / Σ\|x2−x1\|` minden passzon (fabs) |

**Gráf mérőszámok**

| mérőszám | definíció |
|----------|-----------|
| `avg_shortest_path_d` | átlagos legrövidebb út az elérhető rendezett párokon, él hossza = `1 / súly` (Floyd–Warshall) |
| `clustering_Cw` | súlyozott klaszterezési együttható (Ahnert), játékosonként átlagolva: `c_i = Σ W[i][j]·W[j][k]·W[i][k] / Σ W[i][j]·W[i][k]` (`j,k ≠ i`, `j ≠ k`) |

## Elvárt eredmények (`mean ± std`)

| mérőszám | Barcelona | Ellenfelek |
|----------|-----------|-----------|
| `total_passes` | 573.371 ± 94.795 | 240.486 ± 62.900 |
| `centroid_x` | 49.135 ± 2.893 | 46.142 ± 2.859 |
| `centroid_y` | 50.286 ± 3.406 | 50.707 ± 3.465 |
| `centroid_dispersion` | 11.529 ± 0.822 | 11.392 ± 1.108 |
| `advance_ratio` | 1.997 ± 0.159 | 1.332 ± 0.239 |
| `avg_shortest_path_d` | 0.195 ± 0.031 | 0.467 ± 0.113 |
| `lambda1_adjacency` | 59.282 ± 11.388 | 23.489 ± 6.614 |
| `eig_centrality_max` | 0.462 ± 0.035 | 0.472 ± 0.054 |
| `eig_centrality_dispersion` | 0.112 ± 0.017 | 0.119 ± 0.029 |


## Kiterjesztés, 50 passzos csúszóablak

Egy gráf passzait `elapsed_seconds` szerint rendezve csúsztassunk végig egy 50
passz széles ablakot (egyesével lépve), és számítsuk ki minden ablakra a fenti
mérőszámokat, valamint a `t_diff` értéket (az ablak utolsó és első passzának
időkülönbsége). A `t_diff` a játék tempóját méri.
