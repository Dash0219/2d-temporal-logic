#pragma once

#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <stack>


class MaximalConsistentSet;


class Cluster {
public:
    // UNION of all formulas in each mcs in the cluster
    std::unordered_set<std::string> formulas;
    // one mcs to make <= comparisons
    MaximalConsistentSet* representative;

    bool operator<=(const MaximalConsistentSet& other) const;
    bool operator<=(const Cluster& other) const;

    void show_formulas();
};