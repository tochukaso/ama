#include <cstring>
#include <fstream>
#include <emscripten/emscripten.h>
#include "../core/core.h"
#include "../ai/search/beam/beam.h"
#include "../ai/search/beam/eval.h"
#include "../lib/nlohmann/json.hpp"

static beam::eval::Weight g_weight;
static bool g_inited = false;

static cell::Type to_ama(char c) {
    switch (c) {
        case 'R': return cell::Type::RED;
        case 'Y': return cell::Type::YELLOW;
        case 'P': return cell::Type::GREEN;  // ours P == ama GREEN
        case 'B': return cell::Type::BLUE;
        default:  return cell::Type::NONE;
    }
}

extern "C" {

EMSCRIPTEN_KEEPALIVE
int ama_init() {
    std::ifstream f("config.json");
    if (!f.good()) return -1;
    nlohmann::json js; f >> js;
    if (!js.contains("build")) return -2;
    from_json(js["build"], g_weight);
    g_inited = true;
    return 0;
}

// field_chars: 78 bytes (13 rows x 6 cols), 'R'/'B'/'Y'/'P'/'.' (ours convention, top-down)
// out: 40 bytes = 5 candidates x 8 bytes each
//   per candidate: [axisCol, rotation, score(int32 LE), expectedChain, reserved]
// returns: number of candidates written (0..5), or negative on error
EMSCRIPTEN_KEEPALIVE
int ama_suggest(
    const char* field_chars,
    char ca, char cc, char n1a, char n1c, char n2a, char n2c,
    uint8_t* out
) {
    if (!g_inited) return -1;

    Field field;
    for (int r = 0; r < 13; r++) {
        for (int c = 0; c < 6; c++) {
            cell::Type t = to_ama(field_chars[r * 6 + c]);
            if (t != cell::Type::NONE) {
                int y = 12 - r;  // ours r=0 top -> ama y=12
                field.set_cell((i8)c, (i8)y, t);
            }
        }
    }

    cell::Queue q;
    q.push_back({to_ama(ca), to_ama(cc)});
    q.push_back({to_ama(n1a), to_ama(n1c)});
    // beam consumes 2 ply; n2 accepted in the C ABI for forward compatibility.
    (void)n2a; (void)n2c;

    // search_multi requires std::thread (not available in WASM); use single-thread search.
    auto result = beam::search(field, q, g_weight);
    int n = (int)result.candidates.size();
    if (n == 0) return -2;
    if (n > 5) n = 5;

    for (int i = 0; i < n; i++) {
        const auto& cand = result.candidates[i];
        int32_t score = (int32_t)cand.score;
        uint8_t* p = out + i * 8;
        p[0] = (uint8_t)cand.placement.x;
        p[1] = (uint8_t)cand.placement.r;
        p[2] = (uint8_t)(score & 0xFF);
        p[3] = (uint8_t)((score >> 8) & 0xFF);
        p[4] = (uint8_t)((score >> 16) & 0xFF);
        p[5] = (uint8_t)((score >> 24) & 0xFF);
        p[6] = 0;  // expectedChain placeholder — Candidate has no chain field
        p[7] = 0;  // reserved
    }
    return n;
}

}
