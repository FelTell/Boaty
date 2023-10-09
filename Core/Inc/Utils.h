#pragma once

// Based on C++ std::clamp
// (https://en.cppreference.com/w/cpp/algorithm/clamp)
#define UTILS_CLAMP(va, lo, hi)                                    \
    ({                                                             \
        typeof(va) _va = (va);                                     \
        (va)           = (_va < (lo) ? (lo)                        \
                                     : (_va > (hi) ? (hi) : _va)); \
        _va<(lo) || _va>(hi);                                      \
    })
