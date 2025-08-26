#include "MaximalConsistentSet.h"
#include "Cluster.h"


MaximalConsistentSet::MaximalConsistentSet(const std::unordered_set<std::string>& closure)
    : closure_set(closure) {}

bool MaximalConsistentSet::operator==(const MaximalConsistentSet& other) const {
    // for (auto& fmla : formulas)       if (!other.formulas.contains(fmla)) return false;
    // for (auto& fmla : other.formulas) if (!formulas.contains(fmla))       return false;
    // return true;
    return formulas == other.formulas;
}

bool MaximalConsistentSet::operator<=(const MaximalConsistentSet& other) const {
    for (const std::string& fmla : closure_set) {
        // if (!_compare_identity(other, fmla)) {
        //     return false;
        // } else if (fmla[0] == '~' && fmla.size() > 2) {
        //     std::string fmla_id = std::string(1, '~');
        //     if (fmla[1] == 'F') {
        //         // fmla_id =    
        //     } else if (fmla[1] == 'P') {

        //     } else if (fmla[1] == 'G') {

        //     } else if (fmla[1] == 'H') {

        //     }
        // }









        std::string phi = fmla.substr(1);

        if (fmla[0] == 'F') {
            if (other.formulas.contains(phi) && !formulas.contains(fmla) ||
                other.formulas.contains(fmla) && !formulas.contains(fmla)) return false;
        } else if (fmla[0] == 'P') {
            if (formulas.contains(phi) && !other.formulas.contains(fmla) ||
                formulas.contains(fmla) && !other.formulas.contains(fmla)) return false;
        } else if (fmla[0] == 'G') {
            if (formulas.contains(fmla) && !other.formulas.contains(phi) ||
                formulas.contains(fmla) && !other.formulas.contains(fmla)) return false;
        } else if (fmla[0] == 'H') {
            if (other.formulas.contains(fmla) && !formulas.contains(phi) ||
                other.formulas.contains(fmla) && !formulas.contains(fmla)) return false;
        } 

        // else if (fmla[0] == '~' && fmla.size() > 2) {
        //     std::string negated_phi = std::string(1, '~') + fmla.substr(2); 
        //     if (fmla[1] == 'F') {
                
        //     } else if (fmla[1] == 'P') {

        //     } else if (fmla[1] == 'G') {

        //     } else if (fmla[1] == 'H') {

        //     }
        // }
    }
    return true;
}

bool MaximalConsistentSet::operator<=(const Cluster& other) const {
    return *this <= other.representative;
}

void MaximalConsistentSet::show_formulas() const {
    std::cout << "{";
    bool first = true;
    for (const std::string& f : MaximalConsistentSet::formulas) {
        if (!first) std::cout << ", ";
        std::cout << f;
        first = false;
    }
    std::cout << "}\n";
}

// bool MaximalConsistentSet::_compare_identity(const MaximalConsistentSet& other, const std::string& fmla) const {
//     std::string phi = fmla.substr(1);

//     if (fmla[0] == 'F') {
//         if (other.formulas.contains(phi) && !formulas.contains(fmla) ||
//             other.formulas.contains(fmla) && !formulas.contains(fmla)) return false;
//     } else if (fmla[0] == 'P') {
//         if (formulas.contains(phi) && !other.formulas.contains(fmla) ||
//             formulas.contains(fmla) && !other.formulas.contains(fmla)) return false;
//     } else if (fmla[0] == 'G') {
//         if (formulas.contains(fmla) && !other.formulas.contains(phi) ||
//             formulas.contains(fmla) && !other.formulas.contains(fmla)) return false;
//     } else if (fmla[0] == 'H') {
//         if (other.formulas.contains(fmla) && !formulas.contains(phi) ||
//             other.formulas.contains(fmla) && !formulas.contains(fmla)) return false;
//     }

//     return true;
// }
