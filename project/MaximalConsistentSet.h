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

    void show_formulas() const;
private:
    std::unordered_set<std::string> closure_set;
};


namespace std {
    template <>
    struct hash<MaximalConsistentSet> {
        size_t operator()(const MaximalConsistentSet& mcs) const noexcept {
            size_t h = 0;
            std::hash<std::string> str_hash;
            for (const auto& f : mcs.formulas) {
                h ^= str_hash(f) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };
}