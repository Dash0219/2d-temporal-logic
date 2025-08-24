#pragma once


#include <optional>
#include <vector>
#include "Trace.h"


class BoundaryMap {
public:
    bool contains_formula;
    std::optional<MaximalConsistentSet> l, r, t, b;
    std::optional<Trace> n, e, s, w;
    std::optional<Cluster> plus, minus;

    BoundaryMap();
    BoundaryMap(Cluster& cluster, std::string& fmla);
    BoundaryMap(MaximalConsistentSet& irreflexive, std::string& fmla);

    bool operator==(const BoundaryMap& other) const;

    bool is_simple() const;
    bool is_open() const;
    bool is_closed() const;
    bool is_one_point() const;
    // bool is_fabricated();
    // bool contains_formula(std::string& str);
    // bool has_internal_defects();
    // bool join(BoundaryMap& other, int direction);
    // bool limit(); // TODO: we have a different def than the paper
    // bool shuffle(std::vector<BoundaryMap>& others);

};


namespace std {
    template <>
    struct hash<BoundaryMap> {
        size_t operator()(const BoundaryMap& bm) const noexcept {
            size_t h = std::hash<bool>{}(bm.contains_formula);

            auto hash_opt = [&](const auto& opt) {
                if (opt)
                    h ^= std::hash<std::decay_t<decltype(*opt)>>{}(*opt) + 0x9e3779b9 + (h << 6) + (h >> 2);
            };

            hash_opt(bm.l); hash_opt(bm.r); hash_opt(bm.t); hash_opt(bm.b);
            hash_opt(bm.n); hash_opt(bm.e); hash_opt(bm.s); hash_opt(bm.w);
            hash_opt(bm.plus); hash_opt(bm.minus);

            return h;
        }
    };
}
