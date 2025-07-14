#include "Cluster.h"
#include "MaximalConsistentSet.h"

bool Cluster::operator<=(const Cluster& other) const {
    return *this->representative <= *other.representative;
}

bool Cluster::operator<=(const MaximalConsistentSet& other) const {
    return *this->representative <= other;
}

void Cluster::show_formulas() {
    std::cout << "{";
    bool first = true;
    for (const std::string& f : Cluster::formulas) {
        if (!first) std::cout << ", ";
        std::cout << f;
        first = false;
    }
    std::cout << "}\n";
}

// TODO:
// - write Processor class, reformat Formula class, separate concerns
// all non data container logic should go there
// - integrate Cluster class to the rest (which other class needs it)

/* 
finding cliques is O(2^n), but i think we don't need this.
what if i build the cluster when i build the graph? 
cuz transitivity i can check <= with the current representative and i'll know the new mcs is in the cluster
*/

