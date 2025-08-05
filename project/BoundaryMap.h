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

    bool is_simple();
    bool is_open();
    bool is_closed();
    bool is_one_point();
    // bool is_fabricated();
    // bool contains_formula(std::string& str);
    // bool has_internal_defects();
    // bool join(BoundaryMap& other, int direction);
    // bool limit(); // TODO: we have a different def than the paper
    // bool shuffle(std::vector<BoundaryMap>& others);

};
