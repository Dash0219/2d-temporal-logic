#pragma once

#include <variant>
#include <vector>

#include "MaximalConsistentSet.h"
#include "Cluster.h"

class Trace {
public: 
    using Element = std::variant<MaximalConsistentSet, Cluster>;
    std::vector<Element> sequence;

    Element get_front();
    Element get_back();
    bool join(Trace other);
};
