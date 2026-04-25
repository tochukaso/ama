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
    int build_keys = (int)js["build"].size();
    from_json(js["build"], g_weight);
    g_inited = true;
    // Return number of weight keys read so JS can verify the embed file
    // was actually populated (expect ~30+ for build preset).
    return build_keys;
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

    auto result = beam::search_multi(field, q, g_weight);
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

// Diagnostic: returns the loaded value for a given weight key.
// 0=chain, 1=y, 2=key, 3=chi, 4=shape, 5=well, 6=bump, 7=form,
// 8=link_2, 9=link_3, 10=waste_14, 11=side, 12=nuisance, 13=tear, 14=waste.
EMSCRIPTEN_KEEPALIVE
int ama_diag_weight(int idx) {
    if (!g_inited) return -999999;
    switch (idx) {
        case 0:  return g_weight.chain;
        case 1:  return g_weight.y;
        case 2:  return g_weight.key;
        case 3:  return g_weight.chi;
        case 4:  return g_weight.shape;
        case 5:  return g_weight.well;
        case 6:  return g_weight.bump;
        case 7:  return g_weight.form;
        case 8:  return g_weight.link_2;
        case 9:  return g_weight.link_3;
        case 10: return g_weight.waste_14;
        case 11: return g_weight.side;
        case 12: return g_weight.nuisance;
        case 13: return g_weight.tear;
        case 14: return g_weight.waste;
        default: return -999999;
    }
}

// Diagnostic: decode field_chars into ama's Field, then write per-column heights
// (6 bytes, ama coord order x=0..5) into heights_out and return total cell count.
// Lets JS verify that the bytes it sent are interpreted as the field it expects.
EMSCRIPTEN_KEEPALIVE
int ama_diag_field(const char* field_chars, uint8_t* heights_out) {
    Field field;
    for (int r = 0; r < 13; r++) {
        for (int c = 0; c < 6; c++) {
            cell::Type t = to_ama(field_chars[r * 6 + c]);
            if (t != cell::Type::NONE) {
                int y = 12 - r;
                field.set_cell((i8)c, (i8)y, t);
            }
        }
    }
    u8 hs[6];
    field.get_heights(hs);
    for (int i = 0; i < 6; i++) heights_out[i] = hs[i];
    return (int)field.get_count();
}

}
