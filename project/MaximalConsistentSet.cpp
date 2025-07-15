#include "MaximalConsistentSet.h"
#include "Cluster.h"


MaximalConsistentSet::MaximalConsistentSet(const std::unordered_set<std::string>& closure)
    : closure_set(closure) {}

bool MaximalConsistentSet::operator<=(const MaximalConsistentSet& other) const {
    for (const std::string fmla : closure_set) {
        if (fmla[0] == 'F') {
            std::string phi = fmla.substr(1);
            if (other.formulas.contains(phi) && !formulas.contains(fmla) ||
                other.formulas.contains(fmla) && !formulas.contains(fmla)) return false;
        } else if (fmla[0] == 'P') {
            std::string phi = fmla.substr(1);
            if (formulas.contains(phi) && !other.formulas.contains(fmla) ||
                formulas.contains(fmla) && !other.formulas.contains(fmla)) return false;
        }
        // TODO: debug these are wrong i think
        // else if (fmla[0] == 'G') {
        //     std::string phi = fmla.substr(1);
        //     if (formulas.contains(fmla) && !other.formulas.contains(phi) ||
        //         formulas.contains(fmla) && !other.formulas.contains(fmla)) return false;
        // } else if (fmla[0] == 'H') {
        //     std::string phi = fmla.substr(1);
        //     if (other.formulas.contains(fmla) && !formulas.contains(phi) ||
        //         other.formulas.contains(fmla) && !formulas.contains(fmla)) return false;
        // }
    }
    return true;
}

bool MaximalConsistentSet::operator<=(const Cluster& other) const {
    return *this <= other.representative;
}

void MaximalConsistentSet::show_formulas() {
    std::cout << "{";
    bool first = true;
    for (const std::string& f : MaximalConsistentSet::formulas) {
        if (!first) std::cout << ", ";
        std::cout << f;
        first = false;
    }
    std::cout << "}\n";
}
