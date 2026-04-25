#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <cstring>
#include <vector>
#include "../core/core.h"
#include "../ai/search/beam/beam.h"
#include "../ai/search/beam/eval.h"
#include "../lib/nlohmann/json.hpp"
using json = nlohmann::json;

// ama color → ours
static char to_ours_char(cell::Type t) {
    switch (t) {
        case cell::Type::RED: return 'R';
        case cell::Type::YELLOW: return 'Y';
        case cell::Type::GREEN: return 'P';
        case cell::Type::BLUE: return 'B';
        default: return '.';
    }
}

static int to_rotation(direction::Type r) {
    return static_cast<int>(r);
}

static std::string field_to_rows(Field& f) {
    char rows[13][7];
    for (int r = 0; r < 13; r++) {
        for (int c = 0; c < 6; c++) {
            int y = 12 - r;
            cell::Type t = f.get_cell((i8)c, (i8)y);
            rows[r][c] = (t == cell::Type::NONE) ? '.' : to_ours_char(t);
        }
        rows[r][6] = '\0';
    }
    std::string out = "[";
    for (int r = 0; r < 13; r++) {
        if (r > 0) out += ",";
        out += "\"";
        out += rows[r];
        out += "\"";
    }
    out += "]";
    return out;
}

static void load_weight(beam::eval::Weight& w, const std::string& preset) {
    std::ifstream file("config.json");
    if (!file.good()) {
        std::cerr << "config.json not found in cwd\n";
        std::exit(1);
    }
    json js;
    file >> js;
    if (js.contains(preset)) {
        from_json(js[preset], w);
    } else {
        std::cerr << "preset '" << preset << "' not found in config.json\n";
        std::exit(1);
    }
}

int main(int argc, char** argv) {
    using namespace std;

    int games = 1;
    uint32_t seed_base = 0;
    string preset = "build";
    string out_path = "selfplay.jsonl";
    int topk = 5;
    int early_stop_chain = 0; // 0 = disabled

    for (int i = 1; i + 1 < argc; i += 2) {
        string a = argv[i];
        string v = argv[i + 1];
        if (a == "--games") games = std::stoi(v);
        else if (a == "--seed") seed_base = (uint32_t)std::stoul(v);
        else if (a == "--weights") preset = v;
        else if (a == "--out") out_path = v;
        else if (a == "--topk") topk = std::stoi(v);
        else if (a == "--early-stop-chain") early_stop_chain = std::stoi(v);
    }

    beam::eval::Weight w;
    load_weight(w, preset);

    ofstream out(out_path);
    if (!out) {
        cerr << "cannot open " << out_path << " for writing\n";
        return 1;
    }

    for (int gid = 0; gid < games; gid++) {
        uint32_t seed = seed_base + (uint32_t)gid;
        auto queue = cell::create_queue(seed);
        Field field;
        int score = 0;
        int max_chain = 0;
        int move_index = 0;
        vector<string> rows;

        for (int i = 0; i < 200; ++i) {
            if (field.get_height(2) > 11) break;

            cell::Queue tqueue;
            tqueue.push_back(queue[(i + 0) % 128]);
            tqueue.push_back(queue[(i + 1) % 128]);

            auto ai_result = beam::search_multi(field, tqueue, w);
            if (ai_result.candidates.empty()) break;

            string row;
            row += "{";
            row += "\"game_id\":" + to_string(gid) + ",";
            row += "\"move_index\":" + to_string(move_index) + ",";
            row += "\"field\":" + field_to_rows(field) + ",";
            row += "\"current_axis\":\""; row += to_ours_char(tqueue[0].first); row += "\",";
            row += "\"current_child\":\""; row += to_ours_char(tqueue[0].second); row += "\",";
            cell::Pair n1 = queue[(i + 1) % 128];
            cell::Pair n2 = queue[(i + 2) % 128];
            row += "\"next1_axis\":\""; row += to_ours_char(n1.first); row += "\",";
            row += "\"next1_child\":\""; row += to_ours_char(n1.second); row += "\",";
            row += "\"next2_axis\":\""; row += to_ours_char(n2.first); row += "\",";
            row += "\"next2_child\":\""; row += to_ours_char(n2.second); row += "\",";
            row += "\"topk\":[";
            int k_emit = std::min((int)ai_result.candidates.size(), topk);
            for (int k = 0; k < k_emit; k++) {
                if (k > 0) row += ",";
                auto& cand = ai_result.candidates[k];
                row += "{\"axisCol\":" + to_string((int)cand.placement.x) + ",";
                row += "\"rotation\":" + to_string(to_rotation(cand.placement.r)) + ",";
                row += "\"score\":" + to_string((long long)cand.score) + "}";
            }
            row += "],";
            row += "\"esport_seed\":" + to_string((long long)seed);
            row += "}";

            rows.push_back(row);

            auto& mv = ai_result.candidates[0];
            field.drop_pair(mv.placement.x, mv.placement.r, tqueue[0]);
            auto mask = field.pop();
            auto chain = chain::get_score(mask);
            score += (int)chain.score;
            if ((int)chain.count > max_chain) max_chain = (int)chain.count;
            move_index++;

            if (early_stop_chain > 0 && (int)chain.count >= early_stop_chain) break;
        }

        for (auto& r : rows) {
            size_t insert_at = r.size() - 1;
            string ext = ",\"final_score\":" + to_string(score) +
                         ",\"final_max_chain\":" + to_string(max_chain);
            r.insert(insert_at, ext);
            out << r << "\n";
        }
        rows.clear();
    }

    return 0;
}
