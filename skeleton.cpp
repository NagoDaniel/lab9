// ============================================================================
// Guardiola FC Barcelona passzhálózatai 
// ============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <deque>
#include <map>
#include <cmath>
#include <iomanip>
using namespace std;

const string INPUT_FILE = "passes_clean2.csv";

// (match_id, team) egyértelműen azonosít egy gráfot
struct GraphKey {
    int match_id;
    string team;
    bool operator<(const GraphKey& o) const {
        if (match_id != o.match_id) return match_id < o.match_id;
        return team < o.team;
    }
};

// a passes_clean2.csv egy sora
struct PassRow {
    string team;
    string player1, player2;
    double x1, y1, x2, y2;
    int match_id;
    double elapsed_seconds;
};

struct AvgPlayerPos { double x, y; }; // egy játékos átlagos passz-kiindulópontja

// súlyozott, irányított gráf: N×N szomszédsági mátrix, W[i][j] = passzok i -> j
struct Graph {
    vector<string> players;         // index -> név
    map<string,int> player_index;   // név -> index
    vector<vector<double>> W;

    int index_of(const string& player){
        auto it = player_index.find(player);
        if(it != player_index.end()) return it->second;
        int idx = players.size();
        players.push_back(player);
        player_index[player] = idx;
        return idx;
    }
    int N() const { return players.size(); }
    void init(){ W.assign(N(), vector<double>(N(), 0.0)); }
};

using metrics = map<string, double>;

bool is_barcelona(const GraphKey& k){ return k.team == "Barcelona"; }

vector<string> split_csv_line(const string& line){
    vector<string> fields;
    stringstream ss(line);
    string field;
    while(getline(ss, field, ',')) fields.push_back(field);
    return fields;
}

// fejléc: team,player_1,player_2,x1,y1,x2,y2,match_id,elapsed_seconds
vector<PassRow> read_passes(const string& filename){
    ifstream fin(filename);
    if(!fin){ cerr << "Nem sikerult megnyitni: " << filename << endl; exit(1); }
    string line;
    getline(fin, line); // fejléc átugrása
    vector<PassRow> passes;
    while(getline(fin, line)){
        if(line.empty()) continue;
        auto f = split_csv_line(line);
        PassRow p;
        p.team            = f[0];
        p.player1         = f[1];
        p.player2         = f[2];
        p.x1              = stod(f[3]);
        p.y1              = stod(f[4]);
        p.x2              = stod(f[5]);
        p.y2              = stod(f[6]);
        p.match_id        = stoi(f[7]);
        p.elapsed_seconds = stod(f[8]);
        passes.push_back(p);
    }
    return passes;
}

// (match_id, team) szerint csoportosít, idő szerint rendezve (a csúszóablakhoz)
map<GraphKey, vector<PassRow>> bucket_passes(const vector<PassRow>& passes){
    map<GraphKey, vector<PassRow>> bucket;
    for(const auto& p : passes) bucket[{p.match_id, p.team}].push_back(p);
    for(auto& [key, v] : bucket){
        sort(v.begin(), v.end(), [](const PassRow& a, const PassRow& b){
            return a.elapsed_seconds < b.elapsed_seconds;
        });
    }
    return bucket;
}

// gráf építése passzokból (élsúly = passzok száma).
Graph build_graph(const vector<PassRow>& passes){
    Graph g;
    for(const auto& p : passes){ g.index_of(p.player1); g.index_of(p.player2); }
    g.init();
    for(const auto& p : passes)
        g.W[g.index_of(p.player1)][g.index_of(p.player2)] += 1.0;
    return g;
}

// játékosonkénti átlagos passz-kiindulópont (a centroid_dispersion-hez kell)
vector<AvgPlayerPos> derive_positions(const vector<PassRow>& passes){
    map<string, AvgPlayerPos> sum;
    map<string, int> count;
    for(const auto& p : passes){
        sum[p.player1].x += p.x1;
        sum[p.player1].y += p.y1;
        count[p.player1] += 1;
    }
    vector<AvgPlayerPos> pos;
    for(const auto& [player, c] : count)
        pos.push_back({sum[player].x / c, sum[player].y / c});
    return pos;
}

// átlag és minta-szórás 
pair<double,double> mean_std(const vector<double>& v){
    double mean = 0.0;
    for(double x : v) mean += x;
    mean /= v.size();
    double var = 0.0;
    for(double x : v) var += (x - mean) * (x - mean);
    var /= (v.size() - 1);
    return {mean, sqrt(var)};
}

// ============================================================================
// TODO: a mérőszámok függvényei
// ============================================================================



metrics compute_metrics(const GraphKey& key, const vector<PassRow>& passes, const Graph& g){
    metrics m;
    
   // m["total_passes"] = total_passes(g);
   
    return m;
}

int main(){
    auto passes = read_passes(INPUT_FILE);
    auto passes_by_graph = bucket_passes(passes);
    cout << "Betoltve: " << passes_by_graph.size() << " graf" << endl;

    map<GraphKey, Graph> graphs;
    for(const auto& [key, pass_rows] : passes_by_graph)
        graphs[key] = build_graph(pass_rows);

    cout<< graphs.size() << " graf epitve." << endl;

    map<string, pair<vector<double>, vector<double>>> metric_samples; // metrika név -> (barca, rival) 
    for(const auto& [key, g] : graphs){
        //metrikak kiszamolasa
        metrics m = compute_metrics(key, passes_by_graph[key], g);
        for(const auto& [name, value] : m){ // group by minden metrikara, ellenfel vagy barca
            if(is_barcelona(key)){
                metric_samples[name].first.push_back(value);
            } else {
                metric_samples[name].second.push_back(value);
            }
        }
    }
    for(const auto& [name, samples] : metric_samples){ //kiiratas
        auto [barca_mean, barca_std] = mean_std(samples.first);
        auto [rival_mean, rival_std] = mean_std(samples.second);
        cout << left << setw(26) << name
             << "Barcelona: " << barca_mean << " +/- " << barca_std
             << "   Ellenfelek: " << rival_mean << " +/- " << rival_std
             << endl;
    }
    return 0;
}
