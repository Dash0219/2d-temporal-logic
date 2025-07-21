#include "Trace.h"

bool Trace::operator==(const Trace& other) const {
    if (sequence.size() != other.sequence.size()) return false;

    int n = sequence.size();
    for (int i = 0; i < n; ++i) {
        bool is_cluster_1 = std::holds_alternative<Cluster>(sequence[i]);
        bool is_cluster_2 = std::holds_alternative<Cluster>(other.sequence[i]);

        if (is_cluster_1 ^ is_cluster_2) return false;
        if (sequence[i] != other.sequence[i]) return false;
    }

    return true;
}

bool Trace::push_back(Element elem) {
    if (
        std::holds_alternative<Cluster>(sequence.back())
     && std::holds_alternative<MaximalConsistentSet>(elem)
     && std::get<Cluster>(sequence.back()) <= std::get<MaximalConsistentSet>(elem))
    {
        sequence.push_back(elem);
        return true;
    }

    if (
        std::holds_alternative<MaximalConsistentSet>(sequence.back())
     && std::holds_alternative<Cluster>(elem)
     && std::get<MaximalConsistentSet>(sequence.back()) <= std::get<Cluster>(elem))
    {
        sequence.push_back(elem);
        return true;
    }

    return false;
}

bool Trace::join(Trace other) {
    if (sequence.size() == 0 || other.sequence.size() == 0) return false;

    if (
        std::holds_alternative<Cluster>(sequence.back())
     && std::holds_alternative<Cluster>(other.sequence.front())
     && std::get<Cluster>(sequence.back()) == std::get<Cluster>(other.sequence.front())) 
    {
        for (auto it = other.sequence.begin() + 1; it != other.sequence.end(); ++it)
            sequence.emplace_back(std::move(*it));    
        return true;
    }
    
    if (
        std::holds_alternative<MaximalConsistentSet>(sequence.back())
     && std::holds_alternative<MaximalConsistentSet>(other.sequence.front())
     && std::get<MaximalConsistentSet>(sequence.back()) == std::get<MaximalConsistentSet>(other.sequence.front())) 
    {
        for (auto it = other.sequence.begin() + 1; it != other.sequence.end(); ++it)
            sequence.emplace_back(std::move(*it));
        return true;
    }

    if (
        std::holds_alternative<MaximalConsistentSet>(sequence.back())
     && std::holds_alternative<Cluster>(other.sequence.front())
     && std::get<MaximalConsistentSet>(sequence.back()) <= std::get<Cluster>(other.sequence.front()))
    {
        for (auto it = other.sequence.begin(); it != other.sequence.end(); ++it)
            sequence.emplace_back(std::move(*it));
        return true;
    }

    if (
        std::holds_alternative<Cluster>(sequence.back())
     && std::holds_alternative<MaximalConsistentSet>(other.sequence.front())
     && std::get<Cluster>(sequence.back()) <= std::get<MaximalConsistentSet>(other.sequence.front()))
    {
        for (auto it = other.sequence.begin(); it != other.sequence.end(); ++it)
            sequence.emplace_back(std::move(*it));
        return true;
    }

    return false;
}

void Trace::show_sequence() {
    std::cout << "sequence length: " << sequence.size() << " elements\n";
    int i = 0;
    for (auto& elem : sequence) {
        std::cout << i << ". ";
        if (std::holds_alternative<MaximalConsistentSet>(elem)) {
            std::cout << "mcs: ";
            std::get<MaximalConsistentSet>(elem).show_formulas();
        } else if (std::holds_alternative<Cluster>(elem)) {
            std::cout << "cluster: ";
            std::get<Cluster>(elem).show_formulas();
        }
    }
}