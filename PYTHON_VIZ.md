# Adatok és hálózatok vizualizálása Pythonban

Rövid bevezető az adatok betöltéséhez és ábrázolásához. Az oszlopok jelentése: `DATA.md`.

## Környezet (virtuális környezet)

A `teaching/` mappában hozzatok létre egy saját Python-környezetet, hogy a csomagok ne keveredjenek a rendszeréivel:

```bash
python3 -m venv venv                 # egyszer: a környezet létrehozása (Windows: python -m venv venv)
source venv/bin/activate             # minden új terminálban (Windows: venv\Scripts\activate)
pip install pandas numpy networkx matplotlib scikit-learn
```

Ha aktív, a parancssor elején `(venv)` látszik. Kilépés: `deactivate`.
Jupyter notebookhoz még: `pip install jupyter`, majd `jupyter notebook`.

- **Nevek:** a `passes_clean2.csv` és a `passes_full.csv` a **kezdőjátékos** nevét használja:
  ha pl. Xavit lecserélik, a csere passzai is `Xavi` néven szerepelnek, így csapatonként
  11 csúcs marad. Minden más fájlban (`goals`, `shots`, `key_events`, `players`) a **valódi** név van,
  tehát a csere gólja a saját nevén szerepel, a passzfájlokban viszont nem találjátok meg.
  A rövid nevek a `players.csv`-ben vannak (`short_name`: Xavi, Messi, ...).
- **Lap:** a `red_cards` a piros és a második sárga lapot is számolja.

---

## 0. Betöltés

Futtassátok jupyter notebookban vagy `.py` fájlban a `teaching/` mappából.

```python
import numpy as np
import pandas as pd
import networkx as nx
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle, Ellipse

passes  = pd.read_csv("passes_clean2.csv")   # sikeres passzok
full    = pd.read_csv("passes_full.csv")     # minden passz
summary = pd.read_csv("match_summary.csv")   # egy csapat egy meccsen
goals   = pd.read_csv("goals.csv")
shots   = pd.read_csv("shots.csv")
events  = pd.read_csv("key_events.csv")
players = pd.read_csv("players.csv")

# az eredmény a lőtt és kapott gólokból számolható
summary["goal_diff"] = summary.goals_for - summary.goals_against
summary["result"]    = np.select([summary.goal_diff > 0, summary.goal_diff == 0], ["W", "D"], "L")
summary["points"]    = summary.result.map({"W": 3, "D": 1, "L": 0})

_short = dict(zip(players.player, players.short_name))
short  = lambda name: _short.get(name, name.split()[-1])

def draw_pitch(ax):                                    # focipálya rajzolása (0..100 koordináták)
    ax.set_xlim(-2, 102); ax.set_ylim(102, -2)         # y megfordítva: fent y=0
    ax.set_aspect("equal"); ax.axis("off"); ax.set_facecolor("#e9f3e6")
    kw = dict(fill=False, color="#3a6b35", lw=1.2)
    ax.add_patch(Rectangle((0, 0), 100, 100, **kw))
    ax.plot([50, 50], [0, 100], color="#3a6b35", lw=1.2)
    ax.add_patch(Ellipse((50, 50), 15.25, 22.9, **kw))
    for x0 in (0, 86.25):
        ax.add_patch(Rectangle((x0, 29.8), 13.75, 40.4, **kw))
```

Nézzük meg, mi van bennük:

```python
print(summary[["team", "opponent", "goals_for", "goals_against", "shots"]].head())   # első 5 sor
print(summary.columns.tolist())                        # oszlopnevek
print(len(passes))                                     # sorok száma
print(passes[["x1", "y1", "elapsed_seconds"]].describe())   # statisztika a számoszlopokról
```

**Eredmény:**

```text
               team         opponent  goals_for  goals_against  shots
0            Getafe        Barcelona          0              2     19
1         Barcelona           Getafe          2              0     14
2         Barcelona  Atlético Madrid          5              2      8
3   Atlético Madrid        Barcelona          2              5     13
4  Racing Santander        Barcelona          1              4      5
['match_id', 'date', 'match_week', 'team', 'opponent', 'is_barcelona', 'is_home', 'goals_for', 'goals_against', 'shots', 'shots_on_target', 'passes_attempted', 'passes_completed', 'pass_accuracy', 'pass_share', 'corners', 'fouls', 'pressures', 'yellow_cards', 'red_cards', 'substitutions', 'goal_diff', 'result', 'points']
28485
                 x1            y1  elapsed_seconds
count  28485.000000  28485.000000     28485.000000
mean      48.454634     50.422630      2680.023451
std       20.112521     28.555507      1606.455120
min        0.750000      0.125000         0.000000
25%       35.000000     26.750000      1272.000000
50%       48.416667     50.375000      2706.000000
75%       62.583333     74.250000      4044.000000
max      100.250000    100.625000      5659.000000
```

## 1. Pandas alapok, szűrés, csoportosítás

```python
barca = summary[summary.team == "Barcelona"]                       # szűrés
home  = summary[(summary.team == "Barcelona") & summary.is_home]   # két feltétel: & és zárójel
print(len(barca), len(home))                                       # hány sor maradt
print(summary.groupby("is_barcelona").shots.mean())                # csoportonkénti átlag
print(passes.groupby("player_1").size().sort_values(ascending=False).head(5))   # ki passzol a legtöbbet
```

**Eredmény:**

```text
35 16
is_barcelona
False     8.742857
True     15.342857
Name: shots, dtype: float64
player_1
Xavier Hernández Creus      2418
Daniel Alves da Silva       1746
Sergio Busquets i Burgos    1671
Gerard Piqué Bernabéu       1444
Carles Puyol i Saforcada    1425
dtype: int64
```

Minden ábra ugyanazt a három lépést követi: **szűrés → csoportosítás/összesítés → ábra**.

## 2. Ábrák: matplotlib alapok

Az egyszerű ábrákhoz elég a `plt` vagy a pandas `.plot` metódusa. A `plt.show()` mutatja meg az ábrát.

**Oszlopdiagram** — kategóriák összehasonlítása

```python
b = players[players.team == "Barcelona"].set_index("short_name")
b.passes_completed.sort_values().tail(10).plot.barh()
plt.xlabel("sikeres passzok"); plt.show()
```

**Eredmény:**

![ábra](figs/01.png)

**Hisztogram** — egy mennyiség eloszlása

```python
plt.hist(goals.elapsed_seconds / 60, bins=range(0, 106, 15))
plt.xlabel("perc"); plt.ylabel("gólok"); plt.show()
```

**Eredmény:**

![ábra](figs/02.png)

**Pontdiagram** — két mennyiség kapcsolata, csoportonként más színnel (Barcelona meccsei, eredmény szerint)

```python
b = summary[summary.team == "Barcelona"]
for res, color in [("W", "tab:green"), ("D", "tab:orange"), ("L", "tab:red")]:
    g = b[b.result == res]
    plt.scatter(g.passes_completed, g.shots, c=color, label=res, s=50)
plt.xlabel("sikeres passzok"); plt.ylabel("lövések"); plt.legend(); plt.show()
```

**Eredmény:**

![ábra](figs/03.png)

**Idősor** — passzok száma 5 percenként egy meccsen

```python
m = full[full.match_id == 69210]
for team, g in m.groupby("team"):
    plt.hist(g.elapsed_seconds / 60, bins=range(0, 100, 5), histtype="step", lw=2, label=team)
plt.xlabel("perc"); plt.ylabel("passz / 5 perc"); plt.legend(); plt.show()
```

**Eredmény:**

![ábra](figs/04.png)

**Doboz-diagram** — csoportok összehasonlítása

```python
summary[summary.team == "Barcelona"].boxplot(column="pass_share", by="points")
plt.suptitle(""); plt.show()
```

**Eredmény:**

![ábra](figs/05.png)

**Több ábra egymás mellett**

```python
fig, axs = plt.subplots(1, 2, figsize=(10, 4))
axs[0].hist(goals.elapsed_seconds / 60, bins=range(0, 106, 15)); axs[0].set_title("gólok ideje")
axs[1].scatter(summary.shots, summary.goals_for);                axs[1].set_title("lövés és gól")
plt.tight_layout(); plt.show()
```

**Eredmény:**

![ábra](figs/06.png)

Mentés fájlba: `plt.savefig("abra.png", dpi=150)` a `plt.show()` előtt.

## 3. Ábrák a pályán

A koordináták 0 és 100 között vannak, az `x` a támadás iránya. A `draw_pitch` a 0. szakaszban van.

**Lövések egy meccsen** (sárga = gól)

```python
s = shots[shots.match_id == 69210]
fig, ax = plt.subplots(figsize=(8, 5))
draw_pitch(ax)
ax.scatter(s.x, s.y, c=np.where(s.is_goal, "gold", "tab:blue"), s=60, edgecolor="k")
plt.show()
```

**Eredmény:**

![ábra](figs/07.png)

**Hőtérkép** — honnan passzol a Barcelona?

```python
g = passes[passes.team == "Barcelona"]
fig, ax = plt.subplots(figsize=(8, 5))
draw_pitch(ax)
ax.hist2d(g.x1, g.y1, bins=[20, 12], range=[[0, 100], [0, 100]], cmap="Reds", alpha=.8)
plt.show()
```

**Eredmény:**

![ábra](figs/08.png)

A középkör sötét foltja a kezdőrúgás, a kapu előtti a kapusok kirúgása.

## 4. Hálózatok: networkx alapok

Gráf = csúcsok (játékosok) és élek (passzok). Az élsúly a passzok száma.

**Gráf felépítése**

```python
p = passes[(passes.match_id == 69210) & (passes.team == "Barcelona")]
w = p.groupby(["player_1", "player_2"]).size().rename("weight").reset_index()   # (honnan, hová, súly)
G = nx.from_pandas_edgelist(w, "player_1", "player_2", "weight", create_using=nx.DiGraph)

print(G.number_of_nodes(), G.number_of_edges())
print(pd.Series(dict(G.out_degree(weight="weight"))).sort_values(ascending=False))   # ki hány passzt adott le
```

**Eredmény:**

```text
11 91
Xavier Hernández Creus            85
Daniel Alves da Silva             72
Andrés Iniesta Luján              69
Eric-Sylvain Bilal Abidal         64
Gerard Piqué Bernabéu             62
Seydou Kéita                      58
Carles Puyol i Saforcada          51
Víctor Valdés Arribas             41
Lionel Andrés Messi Cuccittini    34
Thierry Henry                     27
Zlatan Ibrahimović                27
dtype: int64
```

**Egyszerű rajz**

```python
widths = [d["weight"] / 5 for *_, d in G.edges(data=True)]
nx.draw(G, labels={n: short(n) for n in G}, node_size=900, font_size=7, width=widths)
plt.show()
```

**Eredmény:**

![ábra](figs/09.png)

**Hálózat a pályán** — a csúcs helye a játékos átlagos passzkiinduló pontja

```python
pos = p.groupby("player_1")[["x1", "y1"]].mean()
xy  = {n: tuple(pos.loc[n]) for n in G}                     # csúcs -> (x, y)

fig, ax = plt.subplots(figsize=(8, 5))
draw_pitch(ax)
nx.draw_networkx_edges(G, xy, width=widths, alpha=.4, arrows=False, ax=ax)
nx.draw_networkx_nodes(G, xy, node_color="tab:red", ax=ax,
                       node_size=[100 + 8 * G.out_degree(n, weight="weight") for n in G])
nx.draw_networkx_labels(G, xy, labels={n: short(n) for n in G}, font_size=7, font_color="white", ax=ax)
plt.show()
```

**Eredmény:**

![ábra](figs/10.png)

**Fontos csúcsok** (PageRank)

```python
pr = pd.Series(nx.pagerank(G, weight="weight")).sort_values()
pr.rename(short).plot.barh(); plt.show()
```

**Eredmény:**

![ábra](figs/11.png)

Más mérőszámok: `nx.in_degree_centrality(G)`, `nx.betweenness_centrality(G)`, `nx.eigenvector_centrality(G)`.


## 5. Saját mérőszámok és eredmények

Ha a C++ programotok egy `metrics.csv`-t ír (`match_id,team,total_passes,advance_ratio,...`):

```python
met = pd.read_csv("metrics.csv").merge(summary, on=["match_id", "team"])     # összekapcsolás
print(met[["total_passes", "advance_ratio", "points", "goal_diff"]].corr())
met.boxplot(column="advance_ratio", by="points"); plt.show()
```

**Eredmény:**

```text
               total_passes  advance_ratio    points  goal_diff
total_passes       1.000000       0.903922  0.806706   0.786431
advance_ratio      0.903922       1.000000  0.729951   0.710102
points             0.806706       0.729951  1.000000   0.891549
goal_diff          0.786431       0.710102  0.891549   1.000000
```

![ábra](figs/12.png)

## 6. Regresszió: jóslás a többi adatból

A regresszió egyenest (vagy síkot) illeszt az adatokra, és ebből **megjósolja** az egyik mennyiséget a többiből. Először egy kitalált példán nézzük meg, hogyan működik, utána valódi adaton.

### Kitalált példa

Tíz kitalált meccs: hány passz volt, és hány lövés született. Az egyenes: `lövés ≈ meredekség · passz + tengelymetszet`.

```python
from sklearn.linear_model import LinearRegression
from sklearn.metrics import r2_score

# kitalált adat: 10 meccs, passzok száma és lövések száma
mock = pd.DataFrame({
    "passes": [380, 420, 450, 480, 510, 540, 570, 600, 640, 700],
    "shots":  [  8,  11,   9,  13,  12,  15,  14,  17,  16,  21],
})

model = LinearRegression()
model.fit(mock[["passes"]], mock.shots)                     # az X két szögletes zárójellel: táblázat, nem oszlop

print("meredekség:", model.coef_[0].round(3), " tengelymetszet:", model.intercept_.round(2))
print("500 passz esetén várt lövés:", model.predict(pd.DataFrame({"passes": [500]}))[0].round(1))
print("R²:", round(r2_score(mock.shots, model.predict(mock[["passes"]])), 2))

plt.scatter(mock.passes, mock.shots)
plt.plot(mock.passes, model.predict(mock[["passes"]]), color="tab:red")      # az illesztett egyenes
plt.xlabel("passzok száma"); plt.ylabel("lövések száma"); plt.show()
```

**Eredmény:**

```text
meredekség: 0.037  tengelymetszet: -5.75
500 passz esetén várt lövés: 12.5
R²: 0.9
```

![ábra](figs/13.png)

- A **meredekség** azt mondja meg, hogy 1 passzal többhöz átlagosan mennyivel több lövés tartozik (itt 0,037, azaz 100 passzhoz kb. 3,7 lövés).
- A `predict` egy új értékre ad jóslatot: 500 passznál kb. 12,5 lövést vár.
- Az **R²** azt mutatja, mennyit magyaráz meg az egyenes: 1 = tökéletes illeszkedés, 0 = nem jobb, mint az átlag.
- Az `X` két szögletes zárójellel (`mock[["passes"]]`) táblázat, mert több jellemző is lehet. Az `y` egy oszlop.

### Valódi példa: ki mennyi passzt kap?

A passzhálózatban a csúcs ki-foka (hány passzt adott) és be-foka (hány passzt kapott) összefügg. Meg tudjuk jósolni a **kapott** passzokat az **adott** passzokból és a játékos átlagos helyéből (`avg_x`)? A hibákból az is kiderül, ki kap a vártnál többet vagy kevesebbet.

```python
from sklearn.linear_model import LinearRegression
from sklearn.metrics import r2_score
from sklearn.model_selection import LeaveOneOut, cross_val_predict

b = players[(players.team == "Barcelona") & (players.passes_attempted >= 100)].reset_index(drop=True)
X, y = b[["passes_attempted", "avg_x"]], b.passes_received

model = LinearRegression().fit(X, y)
print("együtthatók:", dict(zip(X.columns, model.coef_.round(2).tolist())))

# minden játékosra olyan modell jóslata, amelyik őt nem látta (egyet kihagyó keresztvalidáció)
b["pred"] = cross_val_predict(LinearRegression(), X, y, cv=LeaveOneOut())
b["diff"] = b.passes_received - b.pred
print("R²:", round(r2_score(y, b.pred), 2))
print(b.sort_values("diff")[["short_name", "passes_attempted", "passes_received", "pred", "diff"]].round(0).iloc[[0, 1, 2, -3, -2, -1]])

plt.scatter(b.pred, b.passes_received)
plt.plot([0, 2600], [0, 2600], "k--")
for _, r in b[b["diff"].abs() > 130].iterrows():                  # csak a legnagyobb eltérések kapnak nevet
    plt.annotate(r.short_name, (r.pred, r.passes_received), fontsize=8)
plt.xlabel("jósolt kapott passzok"); plt.ylabel("valódi kapott passzok"); plt.show()
```

**Eredmény:**

```text
együtthatók: {'passes_attempted': 0.84, 'avg_x': 10.05}
R²: 0.94
        short_name  passes_attempted  passes_received    pred   diff
5       Dani Alves              2161             1651  1903.0 -252.0
7          Maxwell              1455             1051  1287.0 -236.0
13     Éric Abidal              1005              663   858.0 -195.0
16     Chygrynskiy               692              590   457.0  133.0
11  Andrés Iniesta              1442             1442  1290.0  152.0
1     Lionel Messi              1642             1823  1499.0  324.0
```

![ábra](figs/14.png)

- Az `R²` 0,94: aki sok passzt ad, az sokat is kap, és minél előrébb játszik valaki, annál többet kap ugyanannyi leadott passz mellett (`avg_x`: +1 egység kb. +10 kapott passz).
- A **leave-one-out** keresztvalidáció minden játékosra olyan modellel jósol, amelyik őt nem látta. Így az `R²` nem a betanított adaton mért, hanem „új” játékosokra érvényes érték.
- Vigyázat: csak 20 játékos van, és az együtthatók nincsenek standardizálva (az `avg_x` egysége más, mint a passzoké), ezért nem hasonlíthatók össze közvetlenül.
- Kipróbálható: add hozzá az `avg_y`-t vagy a `pass_accuracy`-t, és nézd meg, változik-e az `R²` és a legnagyobb eltérések listája.
