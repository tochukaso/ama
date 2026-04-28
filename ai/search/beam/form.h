#pragma once

#include <string>
#include <vector>
#include <string_view>

#include "../../../core/core.h"

namespace beam
{

namespace form
{

constexpr u8 HEIGHT = 6;
constexpr u8 AREA = HEIGHT * 6;

struct Data
{
    u8 form[HEIGHT][6] = { 0 };
    i8 matrix[AREA][AREA] = { 0 };
    u8 groups = 0;
};

i32 evaluate(Field& field, u8 height[6], const Data& pattern);

i32 evaluate_2(Field& field, u8 height[6], const Data& pattern);

Field get_plan(Field& field, const Data& pattern);

constexpr Data GTR = []
{
    Data pattern = { 0 };

    const u8 dform[HEIGHT][6] = 
    {
        { 0,  0,  0,  0,  0,  0 },
        { 4,  4,  4,  0,  0,  0 },
        { 3,  3,  3,  4,  0,  0 },
        { 1,  2,  5,  0,  0,  0 },
        { 1,  1,  2,  5,  0,  0 },
        { 2,  2,  5,  0,  0,  0 }
    };

    const i8 dmatrix[AREA][AREA] = 
    {
        { 0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  2, -1, -1,  0,  0,  0,  0 },
        { 0, -1,  1, -1,  0, -1,  0,  0 },
        { 0, -1, -1,  2, -1, -1,  0,  0 },
        { 0,  0,  0, -1,  0,  0,  0,  0 },
        { 0,  0, -1, -1,  0,  0,  0,  0 },
        { 0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  0,  0,  0,  0,  0,  0,  0 }
    };

    for (i8 x = 0; x < 6; ++x) {
        for (i8 y = 0; y < HEIGHT; ++y) {
            pattern.form[y][x] = dform[HEIGHT - 1 - y][x];
            pattern.groups = std::max(pattern.groups, dform[HEIGHT - 1 - y][x]);
        }
    }

    for (i8 x = 0; x < AREA; ++x) {
        for (i8 y = 0; y < AREA; ++y) {
            pattern.matrix[y][x] = dmatrix[y][x];
        }
    }

    return pattern;
} ();

constexpr Data SGTR = []
{
    Data pattern = { 0 };

    const u8 dform[HEIGHT][6] = 
    {
        { 0,  0,  0,  0,  0,  0 },
        { 5,  5,  5,  0,  0,  0 },
        { 4,  4,  4,  5,  0,  0 },
        { 1,  1,  3,  6,  0,  0 },
        { 1,  2,  2,  3,  6,  0 },
        { 2,  3,  3,  6,  0,  0 }
    };

    const i8 dmatrix[AREA][AREA] = 
    {
        { 0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  2, -1, -1, -1,  0,  0,  0 },
        { 0, -1,  1, -1,  0,  0,  0,  0 },
        { 0, -1, -1,  1, -1,  0, -1,  0 },
        { 0, -1,  0, -1,  2, -1,  0,  0 },
        { 0,  0,  0,  0, -1,  0,  0,  0 },
        { 0,  0,  0, -1,  0,  0,  0,  0 },
        { 0,  0,  0,  0,  0,  0,  0,  0 }
    };

    for (i8 x = 0; x < 6; ++x) {
        for (i8 y = 0; y < HEIGHT; ++y) {
            pattern.form[y][x] = dform[HEIGHT - 1 - y][x];
            pattern.groups = std::max(pattern.groups, dform[HEIGHT - 1 - y][x]);
        }
    }

    for (i8 x = 0; x < AREA; ++x) {
        for (i8 y = 0; y < AREA; ++y) {
            pattern.matrix[y][x] = dmatrix[y][x];
        }
    }

    return pattern;
} ();

constexpr Data FRON = []
{
    Data pattern = { 0 };

    const u8 dform[HEIGHT][6] = 
    {
        { 0,  0,  0,  0,  0,  0 },
        { 5,  5,  5,  0,  0,  0 },
        { 4,  4,  4,  5,  0,  0 },
        { 1,  1,  3,  6,  0,  0 },
        { 1,  2,  2,  7,  0,  0 },
        { 3,  3,  2,  3,  6,  0 }
    };

    const i8 dmatrix[AREA][AREA] = 
    {
        { 0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  2, -1, -1, -1,  0,  0,  0 },
        { 0, -1,  1, -1,  0,  0,  0, -1 },
        { 0, -1, -1,  1, -1,  0, -1,  0 },
        { 0, -1,  0, -1,  2, -1,  0,  0 },
        { 0,  0,  0,  0, -1,  0,  0,  0 },
        { 0,  0,  0, -1,  0,  0,  0,  0 },
        { 0,  0, -1,  0,  0,  0,  0,  0 }
    };

    for (i8 x = 0; x < 6; ++x) {
        for (i8 y = 0; y < HEIGHT; ++y) {
            pattern.form[y][x] = dform[HEIGHT - 1 - y][x];
            pattern.groups = std::max(pattern.groups, dform[HEIGHT - 1 - y][x]);
        }
    }

    for (i8 x = 0; x < AREA; ++x) {
        for (i8 y = 0; y < AREA; ++y) {
            pattern.matrix[y][x] = dmatrix[y][x];
        }
    }

    return pattern;
} ();

constexpr Data MERI = []
{
    Data pattern = { 0 };

    const u8 dform[HEIGHT][6] = 
    {
        { 5,  5,  5,  0,  0,  0 },
        { 4,  4,  4,  5,  0,  0 },
        { 1,  2,  6,  0,  0,  0 },
        { 1,  1,  3,  7,  0,  0 },
        { 2,  2,  2,  8,  0,  0 },
        { 3,  3,  3,  7,  0,  0 },
    };

    const i8 dmatrix[AREA][AREA] = 
    {
        { 0,  0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  2, -1, -1, -1,  0,  0,  0,  0 },
        { 0, -1,  1, -1, -1,  0,  0,  0, -1 },
        { 0, -1, -1,  1,  0,  0, -1, -1,  0 },
        { 0, -1, -1,  0,  2, -1, -1,  0,  0 },
        { 0,  0,  0,  0, -1,  0,  0,  0,  0 },
        { 0,  0,  0, -1, -1,  0,  0,  0,  0 },
        { 0,  0,  0, -1,  0,  0,  0,  0,  0 },
        { 0,  0, -1,  0,  0,  0,  0,  0,  0 }
    };

    for (i8 x = 0; x < 6; ++x) {
        for (i8 y = 0; y < HEIGHT; ++y) {
            pattern.form[y][x] = dform[HEIGHT - 1 - y][x];
            pattern.groups = std::max(pattern.groups, dform[HEIGHT - 1 - y][x]);
        }
    }

    for (i8 x = 0; x < AREA; ++x) {
        for (i8 y = 0; y < AREA; ++y) {
            pattern.matrix[y][x] = dmatrix[y][x];
        }
    }

    return pattern;
} ();

// 階段積み (Staircase Chain)。
// 各列に同色 3 連を縦積みし、列ごとに 1 段ずつ上にずらす。
// 連鎖はぷよ消去後の段差ズレで隣の縦 3 連と縦並びになり発火する。
//   . . . 4 . .       y=5
//   . . 3 4 . .       y=4
//   . 2 3 4 . .       y=3
//   1 2 3 . . .       y=2
//   1 2 . . . .       y=1
//   1 . . . . .       y=0
// 各 group の 3 セルは同色 (matrix[i][i]=1)、隣り合う列(=隣り合う group)は別色
// (matrix[i][i±1]=-1)。非隣接の列同士に色制約は無く、4 色運用で循環できる。
constexpr Data KAIDAN = []
{
    Data pattern = { 0 };

    const u8 dform[HEIGHT][6] =
    {
        { 0,  0,  0,  4,  0,  0 },
        { 0,  0,  3,  4,  0,  0 },
        { 0,  2,  3,  4,  0,  0 },
        { 1,  2,  3,  0,  0,  0 },
        { 1,  2,  0,  0,  0,  0 },
        { 1,  0,  0,  0,  0,  0 }
    };

    const i8 dmatrix[AREA][AREA] =
    {
        { 0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  1, -1,  0,  0,  0,  0,  0 },
        { 0, -1,  1, -1,  0,  0,  0,  0 },
        { 0,  0, -1,  1, -1,  0,  0,  0 },
        { 0,  0,  0, -1,  1,  0,  0,  0 },
        { 0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  0,  0,  0,  0,  0,  0,  0 },
        { 0,  0,  0,  0,  0,  0,  0,  0 }
    };

    for (i8 x = 0; x < 6; ++x) {
        for (i8 y = 0; y < HEIGHT; ++y) {
            pattern.form[y][x] = dform[HEIGHT - 1 - y][x];
            pattern.groups = std::max(pattern.groups, dform[HEIGHT - 1 - y][x]);
        }
    }

    for (i8 x = 0; x < AREA; ++x) {
        for (i8 y = 0; y < AREA; ++y) {
            pattern.matrix[y][x] = dmatrix[y][x];
        }
    }

    return pattern;
} ();

// 全 form を 1 本に集約。コンパイル時に絞らず、実行時に
// active_mask() で評価対象を切り替える(GTR-only 訓練ビルドはこれで再現)。
// 新パターン (階段連鎖・鍵積み等) を増やすときは、ここに追記し
// names[] にも同じ順序で名前を足す。
constexpr Data list[] = {
    GTR,
    FRON,
    SGTR,
    KAIDAN
};

constexpr usize COUNT = std::size(list);

constexpr std::string_view names[COUNT] = {
    "GTR",
    "FRON",
    "SGTR",
    "KAIDAN"
};

static_assert(COUNT <= 32, "active_mask uses u32; widen if more forms are added");

// 評価対象の form を表すビットマスク。bit i が立っていれば list[i] を評価する。
// 既定値は全 form 有効 (上位互換: 既存ビルドは挙動が変わらない)。
// 書き込みは init 経路のみ、search 中はスレッド間で読み取り専用。
inline u32& active_mask()
{
    static u32 m = (COUNT >= 32) ? 0xFFFFFFFFu : ((1u << COUNT) - 1u);
    return m;
}

inline bool is_active(usize i)
{
    return (active_mask() >> i) & 1u;
}

// 名前列で active_mask を上書き。未知の名前は無視。
// 空配列を渡すと全 form 無効になる(form 評価がスキップされる)。
inline void set_active_by_names(const std::vector<std::string>& selected)
{
    u32 m = 0;
    for (const auto& n : selected) {
        for (usize i = 0; i < COUNT; ++i) {
            if (n == names[i]) {
                m |= (1u << i);
                break;
            }
        }
    }
    active_mask() = m;
}

inline void reset_active_all()
{
    active_mask() = (COUNT >= 32) ? 0xFFFFFFFFu : ((1u << COUNT) - 1u);
}

};

};