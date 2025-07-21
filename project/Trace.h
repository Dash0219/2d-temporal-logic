#pragma once

#include <variant>
#include <vector>

#include "MaximalConsistentSet.h"
#include "Cluster.h"

class Trace {
public: 
    using Element = std::variant<MaximalConsistentSet, Cluster>;
    std::vector<Element> sequence;

    bool operator==(const Trace& other) const;

    bool push_back(Element elem);
    bool join(Trace other);
    void show_sequence();
};
