# Adatleírás

FC Barcelona 2009/10 (La Liga), 35 meccs, forrás: StatsBomb Open Data. Példakódok: `PYTHON_VIZ.md`.

- `x`, `y` ∈ [0, 100]; `x` = támadás iránya (0 = saját kapu, 100 = ellenfél kapuja).
- `elapsed_seconds` = perc·60 + másodperc, a 2. félidőn át folyamatosan (a 2. félidő 2700-nál indul); `period` = félidő (1/2).
- `passes_clean2.csv`, `passes_full.csv`: a csere passzai a lecserélt kezdőhöz kerülnek. Máshol valódi név.
- Üres mező = nincs adat.

## `passes_clean2.csv` — 28 485 sor, sikeres passzok 

| oszlop | jelentés |
|--------|----------|
| `team` | passzoló csapat |
| `player_1`, `player_2` | passzoló, fogadó |
| `x1`, `y1` | honnan |
| `x2`, `y2` | hová |
| `match_id` | meccs |
| `elapsed_seconds` | idő |

## `passes_full.csv` — 35 445 sor, minden passz (a sikertelenek is)

| oszlop | jelentés |
|--------|----------|
| `match_id`, `team`, `player_1`, `player_2`, `x1`, `y1`, `x2`, `y2` | mint fent (`player_2` üres, ha nincs fogadó) |
| `period`, `elapsed_seconds` | félidő, idő |
| `completed` | sikeres-e |
| `outcome` | Complete, Incomplete, Out, Pass Offside, Unknown, Injury Clearance |
| `length` | hossz (kb. méter) |
| `height` | Ground Pass, Low Pass, High Pass |
| `pass_type` | Open play, Throw-in, Free Kick, Goal Kick, Corner, Kick Off, Recovery, Interception |
| `under_pressure` | nyomás alatt adta |
| `play_pattern` | hogyan indult a birtoklás (Regular Play, From Throw In, From Corner, From Counter, ...) |
| `possession` | birtoklás azonosítója (egy birtoklás passzai ugyanazt kapják) |
| `cross`, `through_ball` | beadás, ziccerpassz |
| `shot_assist`, `goal_assist` | lövéshez, gólhoz vezető passz |

## `match_summary.csv` — 70 sor, egy csapat egy meccsen

| oszlop | jelentés |
|--------|----------|
| `match_id`, `date`, `match_week` | meccs, dátum, forduló |
| `team`, `opponent` | csapat, ellenfél |
| `is_barcelona`, `is_home` | Barcelona-e, hazai-e |
| `goals_for`, `goals_against` | lőtt, kapott gól |
| `shots`, `shots_on_target` | lövések, kapura menők |
| `passes_attempted`, `passes_completed`, `pass_accuracy` | passzok, sikeresek, arány (0–1) |
| `pass_share` | a meccs passzaiból a csapaté (birtoklás-közelítés, 0–1) |
| `corners`, `fouls`, `pressures` | szögletek, szabálytalanságok, nyomásadások |
| `yellow_cards`, `red_cards` | lapok (a második sárga is piros) |
| `substitutions` | cserék |

## `goals.csv` — 116 sor, egy gól

| oszlop | jelentés |
|--------|----------|
| `match_id`, `team`, `opponent` | meccs, a gólt író csapat, ellenfél (öngólnál is az, akinek javára írják) |
| `player` | szerző (öngólnál az öngólozó) |
| `assist_player` | gólpassz adója (üres, ha nincs) |
| `goal_type` | open/set play, penalty, own goal |
| `period`, `minute`, `elapsed_seconds` | idő |
| `score_for`, `score_against` | állás a gól után a szerző csapat szemszögéből |

## `shots.csv` — 843 sor, egy lövés

| oszlop | jelentés |
|--------|----------|
| `match_id`, `team`, `opponent`, `player` | meccs, csapat, ellenfél, lövő |
| `period`, `minute`, `elapsed_seconds` | idő |
| `x`, `y` | a lövés helye |
| `outcome` | Goal, Saved, Saved to Post, Post, Off T, Wayward, Blocked, Saved Off Target |
| `is_goal`, `on_target` | gól-e, kapura ment-e (Goal, Saved, Saved to Post, Post) |
| `shot_type` | Open Play, Free Kick, Penalty |
| `body_part` | Right Foot, Left Foot, Head, Other |
| `assist_player` | lövés előtti passz adója (üres, ha nincs) |
| `play_pattern` | hogyan indult a birtoklás |

## `key_events.csv` — 502 sor, gól / lap / csere időrendben

| oszlop | jelentés |
|--------|----------|
| `match_id`, `team`, `player` | meccs, csapat, érintett játékos (lapnál a kapó, cserénél a lecserélt) |
| `type` | goal, card, substitution |
| `period`, `minute`, `elapsed_seconds` | idő |
| `detail` | gól: típus és gólpassz; lap: Yellow Card, Second Yellow, Red Card; csere: `off; on: <belépő>` |

## `players.csv` — 364 sor, egy játékos (mindkét csapat)

| oszlop | jelentés |
|--------|----------|
| `team`, `player`, `short_name` | csapat, név, rövid név (Xavi, Messi, ...) |
| `matches`, `starts`, `minutes` | meccsek, kezdések, percek (kb.) |
| `passes_attempted`, `passes_completed`, `passes_received` | leadott, sikeres, kapott passzok |
| `pass_accuracy` | sikeres arány (0–1) |
| `avg_x`, `avg_y` | a sikeres passzok átlagos kiindulópontja |
| `shots`, `goals`, `assists` | lövések, gólok, gólpasszok |
