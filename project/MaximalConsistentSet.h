#pragma once

#include <iostream>
#include <unordered_set>
#include <string>


class Formula;
class Cluster;


class MaximalConsistentSet {
public:
    MaximalConsistentSet(Formula& parent);

    Formula& parent;
    std::unordered_set<int> formulas;
    
    bool operator==(const MaximalConsistentSet& other) const;
    bool operator<=(const MaximalConsistentSet& other) const;
    bool operator<=(const Cluster& other) const;

    void show_formulas() const;
private:

    // bool _compare_identity(const MaximalConsistentSet& other, const std::string& fmla) const;
};


// namespace std {
//     template <>
//     struct hash<MaximalConsistentSet> {
//         size_t operator()(const MaximalConsistentSet& mcs) const noexcept {
//             size_t h = 0;
//             std::hash<std::string> str_hash;
//             for (const auto& f : mcs.formulas) {
//                 h ^= str_hash(f) + 0x9e3779b9 + (h << 6) + (h >> 2);
//             }
//             return h;
//         }
//     };
// }

namespace std {
    template <>
    struct hash<MaximalConsistentSet> {
        size_t operator()(const MaximalConsistentSet& mcs) const noexcept {
            size_t h = 0;
            for (const int& id : mcs.formulas) {
                h ^= id + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };
}
