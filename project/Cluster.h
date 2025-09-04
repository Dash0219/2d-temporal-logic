#pragma once

#include <vector>
#include <unordered_set>
#include "MaximalConsistentSet.h"

class Formula;

class Cluster {
public:
    Cluster(Formula& parent, MaximalConsistentSet* mcs);

    Formula& parent;
    int size;
    // UNION of all formulas in each mcs in the cluster
    std::unordered_set<int> formulas;
    // one mcs to make <= comparisons
    MaximalConsistentSet* representative;
    // for debugging purposes: show all mcs in a cluster
    std::vector<MaximalConsistentSet*> sets;
    
    bool operator==(const Cluster& other) const;
    bool operator<=(const MaximalConsistentSet& other) const;
    bool operator<=(const Cluster& other) const;

    void show_formulas() const;
};

// namespace std {
//     template <>
//     struct hash<Cluster> {
//         size_t operator()(const Cluster& c) const noexcept {
//             size_t h = 0;
//             std::hash<std::string> str_hash;
//             for (const auto& f : c.formulas) {
//                 h ^= str_hash(f) + 0x9e3779b9 + (h << 6) + (h >> 2);
//             }
//             return h;
//         }
//     };
// }
namespace std {
    template <>
    struct hash<Cluster> {
        size_t operator()(const Cluster& c) const noexcept {
            size_t h = 0;
            for (const int& id : c.formulas) {
                h ^= id + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };
}
