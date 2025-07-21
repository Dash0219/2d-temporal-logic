#pragma once

#include <iostream>
#include <unordered_set>
#include <string>


class Cluster;


class MaximalConsistentSet {
public:
    MaximalConsistentSet(const std::unordered_set<std::string>& closure_set);
    std::unordered_set<std::string> formulas;

    bool operator==(const MaximalConsistentSet& other) const;

    bool operator<=(const MaximalConsistentSet& other) const;
    bool operator<=(const Cluster& other) const;

    void show_formulas();
private:
    const std::unordered_set<std::string>& closure_set;
};
