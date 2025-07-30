#include "Cluster.h"
// #include "MaximalConsistentSet.h"

Cluster::Cluster(MaximalConsistentSet mcs)
    : size(1), representative(mcs), formulas(mcs.formulas), sets({mcs}) {}

// Cluster& Cluster::operator=(const Cluster& other) const {

// }

bool Cluster::operator==(const Cluster& other) const {
    // TODO: hmm? if i built the clusters correctly then just comparing the rep should be enough.
    // depends on if i change the implementation of the ND algo and how i "guess" things

    // if (formulas.size() != other.formulas.size()) return false;

    // for (auto& fmla : formulas)
    //     if (!other.formulas.contains(fmla)) 
    //         return false;

    // for (auto& fmla : other.formulas)
    //     if (!formulas.contains(fmla)) 
    //         return false;
        
    // return true;

    return representative <= other.representative;
}

bool Cluster::operator<=(const Cluster& other) const {
    return representative <= other.representative;
}

bool Cluster::operator<=(const MaximalConsistentSet& other) const {
    return representative <= other;
}

void Cluster::show_formulas() const {
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
