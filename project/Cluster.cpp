#include "Formula.h"
#include "Cluster.h"
// #include "MaximalConsistentSet.h"

Cluster::Cluster(Formula& parent, MaximalConsistentSet* mcs)
    : parent(parent), size(1), representative(mcs), formulas(mcs->formulas), sets({mcs}) {}

// if i built the clusters correctly then just comparing the reps should be enough.
bool Cluster::operator==(const Cluster& other) const {
    return *representative <= *(other.representative) && *(other.representative) <= *representative;
}

bool Cluster::operator<=(const Cluster& other) const {
    return *representative <= *(other.representative);
}

bool Cluster::operator<=(const MaximalConsistentSet& other) const {
    return *representative <= other;
}

void Cluster::show_formulas() const {
    std::cout << "{";
    bool first = true;
    for (int id : formulas) {
        const std::string& f = parent.id_to_string[id];
        if (!first) std::cout << ", ";
        std::cout << f;
        first = false;
    }
    std::cout << "}\n";
}

// TODO:
// - write Processor class, reformat Formula class, separate concerns
// all non data container logic should go there
