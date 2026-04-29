#include <cstring>
#include <cstdint>
#include <fstream>

#include "../core/core.h"
#include "../ai/search/beam/beam.h"
#include "../ai/search/beam/eval.h"
#include "../lib/nlohmann/json.hpp"

#define API_EXPORT __attribute__((visibility("default")))

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

API_EXPORT
int ama_native_init_preset(const char* preset_name, const char* config_path) {
    std::ifstream f(config_path);
    if (!f.good()) return -1;
    nlohmann::json js;
    try { f >> js; } catch (...) { return -3; }
    if (!js.contains(preset_name)) return -2;
    try { from_json(js[preset_name], g_weight); }
    catch (...) { return -4; }
    g_inited = true;
    return 0;
}

// field_chars: 78 bytes (13 rows x 6 cols), 'R'/'B'/'Y'/'P'/'.' (ours convention, top-down)
// out: 8 bytes for the single best candidate
//   layout: [axisCol, rotation, score(int32 LE), expectedChain, reserved]
// returns: 0 on success, negative on error
API_EXPORT
int ama_native_suggest(
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
    if (result.candidates.empty()) return -2;

    const auto& best = result.candidates[0];
    int32_t score = (int32_t)best.score;

    out[0] = (uint8_t)best.placement.x;
    out[1] = (uint8_t)best.placement.r;
    out[2] = (uint8_t)( score        & 0xFF);
    out[3] = (uint8_t)((score >>  8) & 0xFF);
    out[4] = (uint8_t)((score >> 16) & 0xFF);
    out[5] = (uint8_t)((score >> 24) & 0xFF);
    out[6] = 0;  // expectedChain placeholder — Candidate has no chain field
    out[7] = 0;  // reserved
    return 0;
}

}
