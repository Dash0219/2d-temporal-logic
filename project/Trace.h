#pragma once

#include <variant>
#include <vector>
#include "Cluster.h"

class Trace {
public: 
    using Element = std::variant<MaximalConsistentSet, Cluster>;

    std::vector<Element> sequence;

    Trace();
    Trace(Cluster& cluster);

    bool operator==(const Trace& other) const;

    // bool contains_formula(std::string& str);
    bool push_back(Element elem);
    bool join(Trace other);
    void show_sequence();
    int size();
};


namespace std {
    template <>
    struct hash<Trace> {
        size_t operator()(const Trace& t) const noexcept {
            size_t h = 0;
            for (const auto& elem : t.sequence) {
                size_t elem_hash = std::visit([](auto&& value) {
                    return std::hash<std::decay_t<decltype(value)>>{}(value);
                }, elem);
                h ^= elem_hash + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };
}
