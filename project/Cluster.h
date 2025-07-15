#pragma once

#include <unordered_set>
#include "MaximalConsistentSet.h"


// class MaximalConsistentSet;


class Cluster {
public:
    Cluster(MaximalConsistentSet mcs);
    int size;
    // UNION of all formulas in each mcs in the cluster
    std::unordered_set<std::string> formulas;
    // one mcs to make <= comparisons
    MaximalConsistentSet representative;

    bool operator<=(const MaximalConsistentSet& other) const;
    bool operator<=(const Cluster& other) const;

    void show_formulas();
};