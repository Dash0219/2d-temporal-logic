#pragma once


#include <optional>
#include <vector>

#include "MaximalConsistentSet.h"
#include "Cluster.h"
#include "Trace.h"


class BoundaryMap {
public:
    std::optional<MaximalConsistentSet> l, r, t, b;
    std::optional<Trace> n, e, s, w;
    std::optional<Cluster> plus, minus;
    
    BoundaryMap();
    BoundaryMap(Cluster& cluster);
    
    bool is_simple();
    bool is_open();
    bool is_fabricated();
    bool join(BoundaryMap& other, int direction);
    bool limit(); // TODO: we have a different def than the paper
    bool shuffle(std::vector<BoundaryMap>& others);
};
